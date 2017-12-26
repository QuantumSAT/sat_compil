/****************************************************************************
 * Copyright (C) 2017 by Juexiao Su                                         *
 *                                                                          *
 * This file is part of QSat.                                               *
 *                                                                          *
 *   QSat is free software: you can redistribute it and/or modify it        *
 *   under the terms of the GNU Lesser General Public License as published  *
 *   by the Free Software Foundation, either version 3 of the License, or   *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   QSat is distributed in the hope that it will be useful,                *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU Lesser General Public License for more details.                    *
 *                                                                          *
 *   You should have received a copy of the GNU Lesser General Public       *
 *   License along with QSat.  If not, see <http://www.gnu.org/licenses/>.  *
 ****************************************************************************/

/*! \file qpar_routing_graph.cc
 *  \author Juexiao Su
 *  \date 14 Dec 2017
 *  \brief dwave routing graph
 */


#include "qpar/qpar_graph.hh"
#include "qpar/qpar_routing_graph.hh"
#include "qpar/qpar_target.hh"
#include "qpar/qpar_netlist.hh"
#include "syn/netlist.h"

#include "hw_target/hw_target.hh"
#include "hw_target/hw_object.hh"
#include "hw_target/hw_loc.hh"
#include "utils/qlog.hh"

#include <sstream>

bool RoutingNodeCmp::operator()(const RoutingNode* node1, const RoutingNode* node2) const {
  return node1->getIndex() < node2->getIndex();
}

bool RoutingEdgeCmp::operator()(const RoutingEdge* edge1, const RoutingEdge* edge2) const {
  return edge1->getIndex() < edge2->getIndex();
}


RoutingGraph::RoutingGraph(HW_Target_Dwave* dwave_device, ParTarget* par_target) :
  _dwave_device(dwave_device), _par_target(par_target) 
{
  createRoutingGraph();
}

RoutingGraph::~RoutingGraph() {

  std::unordered_map<HW_Cell*, RoutingCell*>::iterator c_iter = _cells.begin();
  for (; c_iter != _cells.end(); ++c_iter)
    delete c_iter->second;
  _cells.clear();

  NODES::iterator n_iter = _nodes.begin();
  for (; n_iter != _nodes.end(); ++n_iter) 
    delete (*n_iter);
  _nodes.clear();

  EDGES::iterator e_iter = _edges.begin();
  for (; e_iter != _edges.end(); ++e_iter)
    delete (*e_iter);
  _edges.clear();

}

unsigned RoutingNode::_index_counter = 0;
unsigned RoutingEdge::_index_counter = 0;
double RoutingNode::_congestion_cost = 0.0;

void RoutingGraph::checkRoutingGraphCurrentUsage() const {

  NODES::iterator node_iter = _nodes.begin();
  for (; node_iter != _nodes.end(); ++node_iter) {
    RoutingNode* node = *node_iter;
    if (node->getCurrentlyUsed())
      qlog.speakError("found node is marked as currently used!");
  }

}


RoutingNode* RoutingGraph::getRoutingNode(ParElement* element, SYN::Pin* pin) const {
  HW_Cell* cell = element->getCurrentGrid()->getHWCell();
  RoutingCell* r_cell = _cells.at(cell);
  return r_cell->getRoutingNode(pin);
}


void RoutingGraph::createRoutingGraph() {
  qlog.speak("Routing Graph", "build routing graph...");
  qlog.speak("Routing Graph", "build local routing graph for each cell...");
  for (COORD x = 0; x < _par_target->getXLimit(); ++x) {
    for (COORD y = 0; y < _par_target->getYLimit(); ++y) {
      ParGrid* grid = _par_target->getGrid(x, y);
      RoutingCell* cell = new RoutingCell(grid, this);
      HW_Cell* hw_cell = grid->getHWCell();
      _cells.insert(std::make_pair(hw_cell, cell));
    }
  }
  qlog.speak("Routing Graph", "%lu nodes and %lu edges are created",
      _nodes.size(),
      _edges.size());

  qlog.speak("Routing Graph", "build inter-cell routing graph...");
  

  HW_Target_abstract::I_ITER interac_iter = _dwave_device->inter_cell_interac_begin();
  for (; interac_iter != _dwave_device->inter_cell_interac_end(); ++interac_iter) {
    HW_Interaction* interac = interac_iter->second;
    //std::stringstream ss;
    //ss << interac->getLoc();
    //qlog.speak("JSU_DEBUG", "%s", ss.str().c_str());
    HW_Qubit* qubit1 = interac->getFrom();
    HW_Qubit* qubit2 = interac->getTo();

    COORD x1 = qubit1->getLoc().getLocX();
    COORD y1 = qubit1->getLoc().getLocY();
    COORD local1 = qubit1->getLoc().getLocalIndex();

    COORD x2 = qubit2->getLoc().getLocX();
    COORD y2 = qubit2->getLoc().getLocY();
    COORD local2 = qubit2->getLoc().getLocalIndex();

    RoutingCell* cell1 = _cells[qubit1->getCell()];
    RoutingCell* cell2 = _cells[qubit2->getCell()];

    ParGrid* grid1 = _par_target->getGrid(x1, y1);
    ParGrid* grid2 = _par_target->getGrid(x2, y2);

    if (grid1->getCurrentElement())
      local1 = local1 % 4;

    if (grid2->getCurrentElement())
      local2 = local2 % 4;

    RoutingNode* inter_node = new RoutingNode(interac);
    RoutingNode* rr_node1 = cell1->getRoutingNode(local1);
    RoutingNode* rr_node2 = cell2->getRoutingNode(local2);
    _nodes.insert(inter_node);

    RoutingEdge* edge1 = new RoutingEdge(rr_node1, inter_node);
    RoutingEdge* edge2 = new RoutingEdge(inter_node, rr_node2);
    _edges.insert(edge1);
    _edges.insert(edge2);
  }
  qlog.speak("Routing Graph", "routing graph created %lu nodes %lu edges",
      _nodes.size(),
      _edges.size());

  //sanityCheck();
}


void RoutingGraph::sanityCheck() const {
  NODES::iterator n_iter = _nodes.begin();
  for (; n_iter != _nodes.end(); ++n_iter) {
    RoutingNode* node = *n_iter;
    node->sanityCheck();
  }
}


RoutingNode::RoutingNode() :
  _qubit(NULL),
  _interaction(NULL),
  _pin(NULL),
  _isLogicalQubit(false),
  _load(0),
  _history_cost(0.0),
  _capacity(1)
{
  _node_index = _index_counter;
  ++_index_counter;
}

RoutingNode::RoutingNode(HW_Qubit* qubit, bool logical) :
  _qubit(qubit),
  _interaction(NULL),
  _pin(NULL),
  _isLogicalQubit(logical),
  _load(0),
  _history_cost(0.0),
  _capacity(1)
{
  _node_index = _index_counter;
  ++_index_counter;
}

RoutingNode::RoutingNode(HW_Interaction* iter) :
  _qubit(NULL),
  _interaction(iter),
  _pin(NULL),
  _isLogicalQubit(false),
  _load(0),
  _history_cost(0.0),
  _capacity(1)
{
  _node_index = _index_counter;
  ++_index_counter;
}

RoutingNode::RoutingNode(SYN::Pin* pin) :
  _qubit(NULL),
  _interaction(NULL),
  _pin(pin),
  _history_cost(0.0),
  _isLogicalQubit(false),
  _capacity(1)
{
  _node_index = _index_counter;
  ++_index_counter;
}


void RoutingNode::sanityCheck() const {
  std::stringstream ss;
  ss << "Node Type:: ";
  if      (isQubit()) {
    ss << "Qubit ";
    ss << "Logic: " << (_isLogicalQubit ? "true " : "false ");
    ss << "Loc: " << _qubit->getLoc();
  }
  else if (isInteraction()) {
    ss << "Interaction ";
    ss << "Loc: " << _interaction->getLoc();
  }
  else if (isPin()) {
    ss << "Pin ";
  }
  else QASSERT(0);

  qlog.speak("Routing Node", "%s| has %lu neighbors: ", ss.str().c_str(),
      _edges.size());

  EDGES::iterator e_iter = _edges.begin();
  for (; e_iter != _edges.end(); ++e_iter) {
    RoutingEdge* edge = *e_iter;
    RoutingNode* dnode = edge->getOtherNode(this);
    QASSERT(edge->getOtherNode(dnode) == this);
    std::stringstream ss1;
    if      (dnode->isQubit()) {
      ss1 << "Qubit ";
      ss1 << "Logic: " << (dnode->_isLogicalQubit ? "true " : "false ");
      ss1 << "Loc: " << dnode->_qubit->getLoc();
    }
    else if (dnode->isInteraction()) {
      ss1 << "Interaction ";
      ss1 << "Loc: " << dnode->_interaction->getLoc();
    }
    else if (dnode->isPin()) {
      ss1 << "Pin ";
    }
    else QASSERT(0);
    qlog.speak("Routing Node", "    %s", ss1.str().c_str());
  }
}




RoutingEdge::RoutingEdge(RoutingNode* node1, RoutingNode* node2)
{
  _index = _index_counter;
  ++_index_counter;

  if (node1->getIndex() < node2->getIndex()) {
    _node1 = node1;
    _node2 = node2;
  } else {
    _node1 = node2;
    _node2 = node1;
  }

  _node1->addEdge(this);
  _node2->addEdge(this);
  
}


RoutingCell::RoutingCell(ParGrid* grid, RoutingGraph* graph) :
  _grid(grid),
  _graph(graph)
{
  initCellRoutingGraph();
}


void RoutingCell::initCellRoutingGraph() {
 
  typedef std::vector<SYN::Pin*>::iterator PIN_ITER;

  if (_grid->getCurrentElement()) {
    //1) build pin node
    HW_Cell* cell = _grid->getHWCell();
    ParElement* par_ele = _grid->getCurrentElement();
    SYN::Gate* syn_gate = par_ele->getSynGate();
    PIN_ITER pin_iter = syn_gate->begin();
    for (; pin_iter != syn_gate->end(); ++pin_iter) {
      SYN::Pin* pin = *pin_iter;
      RoutingNode* node = new RoutingNode(pin);
      _pin_to_node.insert(std::make_pair(pin, node));
      _graph->_nodes.insert(node);
      _nodes.push_back(node);
    }

    //2) build qubit node TODO:change hard-coded index
    for (COORD i = 0; i < 4; ++i) {
      HW_Qubit* qubit = cell->getQubit(i);
      RoutingNode* node = new RoutingNode(qubit, true);
      _index_to_node.insert(std::make_pair(i, node));
      _graph->_nodes.insert(node);
      _nodes.push_back(node);
    }


    //3) build edges
    std::map<COORD, RoutingNode*>::iterator q_iter;
    std::map<SYN::Pin*, RoutingNode*>::iterator p_iter;
    for (p_iter = _pin_to_node.begin(); 
         p_iter != _pin_to_node.end(); ++p_iter) {

      q_iter = _index_to_node.begin();
      for (; q_iter != _index_to_node.end(); ++q_iter) {
        RoutingNode* pin_node = p_iter->second;
        RoutingNode* qu_node = q_iter->second;
        RoutingEdge* edge = new RoutingEdge(pin_node, qu_node);
        _edges.push_back(edge);
        _graph->_edges.insert(edge);
      }
    }

  } else {
    HW_Cell* cell = _grid->getHWCell();

    //1) build qubits node
    HW_Cell::QUBITS& qubits = cell->getQubits();
    HW_Cell::QUBITS::iterator q_iter = qubits.begin();
    for (; q_iter != qubits.end(); ++q_iter) {
      HW_Qubit* qubit = q_iter->second;
      RoutingNode* node = new RoutingNode(qubit);
      _index_to_node.insert(std::make_pair(q_iter->first, node));
      _graph->_nodes.insert(node);
      _nodes.push_back(node);
    }

    //2) build interaction node and edges
    HW_Cell::INTERACTIONS& interactions = cell->getInteractions();
    HW_Cell::INTERACTIONS::iterator i_iter = interactions.begin();
    for (; i_iter != interactions.end(); ++i_iter) {
      HW_Interaction* interac = i_iter->second;
      RoutingNode* node = new RoutingNode(interac);
      _nodes.push_back(node);
      _graph->_nodes.insert(node);
      COORD qubit1_coord = HW_Loc::globalIndexToLocalIndex(i_iter->first.first);
      COORD qubit2_coord = HW_Loc::globalIndexToLocalIndex(i_iter->first.second);

      RoutingNode* node1 = _index_to_node.at(qubit1_coord);
      RoutingNode* node2 = _index_to_node.at(qubit2_coord);

      RoutingEdge* edge1 = new RoutingEdge(node1, node);
      _edges.push_back(edge1);
      _graph->_edges.insert(edge1);

      RoutingEdge* edge2 = new RoutingEdge(node, node2);
      _edges.push_back(edge2);
      _graph->_edges.insert(edge2);
    }

  }

  

}
