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
#include "qpar/qpar_router.hh"
#include "qpar/qpar_netlist.hh"
#include "qpar/qpar_routing_graph.hh"
#include "qpar/qpar_routing_cost.hh"
#include "syn/netlist.h"
#include "utils/qlog.hh"

#include <algorithm>
#include <fstream>


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


RoutePath::RoutePath(const std::list<RoutingNode*>& nodes, const std::list<RoutingEdge*>& edges) {
  SUPER::resize(nodes.size());
  _edges.resize(edges.size());
  std::copy(nodes.begin(), nodes.end(), SUPER::begin());
  std::copy(edges.begin(), edges.end(), _edges.begin());
}

bool TargetSlackCmp::operator()(const ParWireTarget* tgt1, const ParWireTarget* tgt2) const {
  return tgt1->getSlack() < tgt2->getSlack();
}

QRoute::~QRoute() {
  if (_cost) delete _cost;
  _cost = NULL;

  if (_cost_simple) delete _cost_simple;
  _cost_simple = NULL;

  if (_router) delete _router;
  _router = NULL;

  if (_first_router) delete _first_router;
  _first_router = NULL;

}


void QRoute::run() {

  qlog.speak("Route", "Initialize routing...");
  initializeRouting();

  std::vector<ParWireTarget*> targets = _netlist->getTargets();

  // sort based on slack of the target
  TargetSlackCmp cmp;
  std::sort(targets.begin(), targets.end(), cmp);

  const unsigned int nbr_max_iter = 30;
  unsigned int N = 1;

  bool routing_suc = false;

  qlog.speak("ROUTE", " +----------+----------------+----------+-------------+-----------------+-------------+");
  qlog.speak("ROUTE", " |  status  |    strategy    |   iter   |longest chain| overflow ration |  overflow   |");
  qlog.speak("ROUTE", " +----------+----------------+----------+-------------+-----------------+-------------+");
  for (; N <= nbr_max_iter; ++N) {

    routeAllTarget(targets, N);
  
    unsigned overflow;
    bool valid = isRoutingValid(targets, overflow);
    //updateWireSlack();

    if (valid) {
      qlog.speak("ROUTE", " |   valid  |  negotiating   |  %4u    |    %6u   |      %5.2f      |     %4u    |",
          N, (unsigned)_longest_wire_length, ((double)overflow/(double)_rr_graph->getNodeNum()), (unsigned)overflow);
      routing_suc = true;
      break;
    } else {
      qlog.speak("ROUTE", " |  invalid |  negotiating   |  %4u    |    %6u   |      %5.2f      |     %4u    |",
          N, (unsigned)_longest_wire_length, ((double)overflow/(double)_rr_graph->getNodeNum()), (unsigned)overflow);

    }

  }
  qlog.speak("ROUTE", " +----------+----------------+----------+-------------+-----------------+-------------+");

  if (!routing_suc)
    qlog.speakError("Routing Failed");

  qlog.speak("ROUTE", " successfully route netlist");

}


bool QRoute::isTargetOverFlow(ParWireTarget* target) {

  RoutePath* path = target->getRoutePath();

  for (size_t i = 0; i < path->size(); ++i) {
    RoutingNode* node = path->at(i);
    if (node->isOverFlow())
      return true;
  }

  return false;

}

bool QRoute::isRoutingValid(std::vector<ParWireTarget*>& targets, unsigned& overflow) {

  //1) first method iterator all targets;
  std::set<RoutingNode*> invalid_nodes1;
  for (size_t i = 0; i < targets.size(); ++i) {
    ParWireTarget* target = targets[i];

    if (target->getDontRoute()) continue;

    RoutePath* path = target->getRoutePath();

    for (size_t i = 0; i < path->size(); ++i) {
      RoutingNode* node = path->at(i);
      if (node->isOverFlow()) {
        invalid_nodes1.insert(node);
        node->setHistoryCost(3 + node->getHistoryCost());
      }
    }
  }

  std::set<RoutingNode*> invalid_nodes2;
  NODES::iterator node_iter = _rr_graph->node_begin();
  for (; node_iter != _rr_graph->node_end(); ++node_iter) {
    RoutingNode* node = *node_iter;
    if (node->isOverFlow())
      invalid_nodes2.insert(node);
  }

  QASSERT(invalid_nodes2.size() == invalid_nodes1.size());

  overflow = (unsigned)invalid_nodes1.size();
  return (overflow == 0);
}

void QRoute::initializeRouting() {

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

void QRoute::routeAllTarget(std::vector<ParWireTarget*>& targets, unsigned iter) {
  ParRouter* router = (iter == 1) ? _first_router : _router;

  std::vector<ParWireTarget*>::iterator tgt_iter = targets.begin();
  for (; tgt_iter != targets.end(); ++tgt_iter) {
    ParWireTarget* tgt = *tgt_iter;
    routeTarget(tgt, router);
  }

  updateWireSlack();
  RoutingNode::setCongestionCost(RoutingNode::getCongestionCost() + 4);
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

void QRoute::routeTarget(ParWireTarget* target, ParRouter* router) {

  if (target->getDontRoute()) return;

  target->ripupTarget();
  ParWire* wire = target->getWire();

  wire->markUsedRoutingResource();

  double slack = target->getSlack();

  ParElement* source_ele = target->getSourceElement();
  ParElement* target_ele = target->getTargetElement();

  SYN::Pin* src_pin = target->getSourcePin();
  SYN::Pin* tgt_pin = target->getTargetPin();

  RoutingNode* src_node = _rr_graph->getRoutingNode(source_ele, src_pin);
  RoutingNode* tgt_node = _rr_graph->getRoutingNode(target_ele, tgt_pin);

  QASSERT(src_node);
  QASSERT(tgt_node);

  std::unordered_set<RoutingNode*> used = wire->getUsedRoutingNodes();

  if (router->route(src_node, tgt_node, slack, used, target)) {
    updateRoute(target, slack, router);
  } else {
    qlog.speakError("Cannot find route for %s wire %s pin",
        wire->getName().c_str(),
        target->getName().c_str());
  }

  wire->unmarkUsedRoutingResource();

}

void QRoute::checkLoad() {

  WIRE_ITER w_iter = _netlist->wire_begin();
  for (; w_iter != _netlist->wire_end(); ++w_iter) {
    ParWire* wire = *w_iter;
    wire->checkRoutingNodeUsage();
  }


  std::unordered_map<RoutingNode*, unsigned> node_usage;

  WIRE_ITER w_iter = _netlist->wire_begin();
  for (; w_iter != _netlist->wire_end(); ++w_iter) {
    ParWire* wire = *w_iter;
    std::unordered_set<RoutingNode*>& routing_nodes = getUsedRoutingNodes();
    std::unordered_set<RoutingNode*>::iterator n_iter = routing_nodes.begin();
    for (; n_iter != routing_nodes.end(); ++n_iter) {
      RoutingNode* node = *n_iter;
      if (node_usage.count(node))
        ++node_usage[node];
      else
        node_usage[node] = 1;
    }
  }


  NODES::iterator node_iter = _rr_graph->node_begin();
  for (; node_iter != _rr_graph->node_end(); ++node_iter) {
    RoutingNode* node = *node_iter;
    if (node_usage.count(node))
      QASSERT(node->getLoad() == node_usage[node]);
    else
      QASSERT(node->getLoad() == 0);
  }

}

void QRoute::updateRoute(ParWireTarget* target, double& slack, ParRouter* router) {
  std::list<RoutingNode*> nodes;
  std::list<RoutingEdge*> edges;
  router->buildRoutePath(nodes, edges);

  RoutePath* old_route = target->getRoutePath();
  if (old_route)
    delete old_route;

  RoutePath* new_route = new RoutePath(nodes, edges);
  target->setRoutePath(new_route);
  target->getWire()->updateWireRoute(new_route);
}

void QRoute::printAllRoute(std::string filename) {

  std::ofstream outfile;
  outfile.open(filename.c_str());
  std::vector<ParWireTarget*> targets = _netlist->getTargets();
  for (size_t i = 0; i < targets.size(); ++i) {
    targets[i]->printRoute(outfile);
  }
  outfile.close();
}


