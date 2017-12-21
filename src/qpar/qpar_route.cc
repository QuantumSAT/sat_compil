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


#include "qpar/qpar_route.hh"
#include "qpar/qpar_netlist.hh"
#include "qpar/qpar_routing_graph.hh"
#include "qpar/qpar_routing_cost.hh"
#include "syn/netlist.h"
#include "utils/qlog.hh"

#include <algorithm>


FastRoutingGraph::FastRoutingGraph(RoutingGraph* graph) :
SUPER(graph->getNodeNum(), graph->getEdgeNum()){
  NODES::iterator n_iter = graph->node_begin();
  for (; n_iter != graph->node_end(); ++n_iter) {
    RoutingNode* node = *n_iter;
    unsigned num_edge = node->getEdgeNum();
    add_vertex(node, num_edge);
  }

  EDGES::iterator e_iter = graph->edge_begin();
  for (; e_iter != graph->edge_end(); ++e_iter) {
    RoutingEdge* edge = *e_iter;
    RoutingNode* node1 = edge->getRoutingNode1();
    RoutingNode* node2 = edge->getRoutingNode2();
    add_edge(edge, node1, node2);
  }


}

bool TargetSlackCmp::operator()(const ParWireTarget* tgt1, ParWireTarget* tgt2) const {
  return tgt1->getSlack() < tgt2->getSlack();
}


void QRoute::run() {

  qlog.speak("Route", "Initialize routing...");
  initializeRouting();

  std::vector<ParWireTarget*> targets = _netlist->getTargets();

  // sort based on slack of the target
  std::sort(targets.begin(), targets.end(), TargetSlackCmp);

  bool route_success = false;
  const unsigned int nbr_max_iter = 30;
  unsigned int N = 1;
  for (; N <= nbr_max_iter; ++N) {
    if (shouldExit()) {
      route_success = true;
      break;
    }
  }

}

void QRoute::initiallizeRouting() {

  buildRoutingGraph();

  _cost = new RoutingCostNBR();
  _router = new ParRouter(*_f_graph, *_cost);

  _cost_simple = new RoutingCostSimple();
  _first_router = new ParRouter(*_f_graph, *_cost_simple);

  initializeWireSlack(); 

}

void QRoute::initializeWireSlack() {
  WIRE_ITER w_iter = _netlist->wire_begin();
  for (; w_iter != _netlist->wire_end(); ++w_iter) {
    ParWire* wire = *w_iter;
    wire->setSlack(1.0);
  }

}

void QRoute::buildRoutingGraph() {
  _rr_graph = new RoutingGraph(_dwave_target, par_target);
  _f_graph = new FastRoutingGraph(_rr_graph);
}

void QRoute::routeAllTarget(std::vector<ParWireTarget*>& targets, unsigned iter) {
  ParRouter* router = (iter == 1) ? _first_router : _router;

  std::vector<ParWireTarget*>::iterator tgt_iter = targets.begin();
  for (; tgt_iter != targets.end(); ++tgt_iter) {
    ParWireTarget* tgt = *tgt_iter;
    routeTarget(tgt);
  }

  updateWireSlack();
  updateHistoryCost(N);

}

void QRoute::updateWireSlack() {
  double longest_length = 0;
  WIRE_ITER w_iter = _netlist->wire_begin();
  for (; w_iter != _netlist->wire_end(); ++w_iter) {
    ParWire* wire = *w_iter;
    if (wire->getWireLength() > longest_length)
      longest_length = wire->getWireLength();
  }
  _longest_wire_length = longest_length;

  for (; w_iter != _netlist->wire_end(); ++w_iter) {
    ParWire* wire = *w_iter;
    double slack = 1 - (wire->getWireLength() / longest_length);
    QASSERT(slack >= 0 && slack <= 1);
    wire->setSlack(slack);
  }

}

void QRoute::routeTarget(ParWireTarget* target, ParRoute* router) {


  target->ripupTarget();
  ParWire* wire = target->getWire();

  wire->markUsedRoutingResource();

  double slack = target->getSlack();

  ParElement* source_ele = target->getSourceElement();
  ParElement* target_ele = target->getTargetElement();

  SYN::Pin* src_pin = target->getSourcePin();
  SYN::Pin* tgt_pin = target->getSourcePin();

  RoutingNode* src_node = _rr_graph->getRoutingNode(source_ele, src_pin);
  RoutingNode* tgt_node = _rr_graph->getRoutingNode(target_ele, tgt_pin);

  std::<unordered_set> used = wire->getUsedRoutingNodes();

  if (router->route(src_node, tgt_node, slack, used, target)) {
    updateRoute(target, slack, router);
  } else {
    qlog.spakeError("Cannot find route for %s wire %s pin",
        wire->getName().c_str(),
        target->getName().c_str());
  }

  wire->unmarkUsedRoutingResource();

}


