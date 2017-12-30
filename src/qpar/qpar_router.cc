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

#include "qpar/qpar_router.hh"
#include "qpar/qpar_routing_graph.hh"
#include "qpar/qpar_netlist.hh"
#include "qpar/qpar_routing_cost.hh"


#include <limits>

bool ParRouter::route(RoutingNode* src, RoutingNode* tgt, double slack, std::unordered_set<RoutingNode*>& used, ParWireTarget* target) {

  _from_edge.clear();
  _source = _graph.get_i_vertex(src);
  _target = _graph.get_i_vertex(tgt);

  if (_source == _target) {
    qlog.speak("Router", "Find ParWireTarget with same source and target");
    return true;
  }

  for (size_t i = 0; i < _visited_node.size(); ++i)
    _visited_node[i] = std::numeric_limits<double>::infinity();

  QPriorityQueue* pqueue = new QPriorityQueue;
  _visited_node[_source] = 0.0;

  qvertex cur_vertex = _source;
  double real_cost = 0.0;
  double current_cost = 0.0;


  expandNeighbors(pqueue, cur_vertex, real_cost, slack, target);

  if (pqueue->empty()) {
    qlog.speak("Router", "Cannot find route for %s:%s", target->getWire()->getName().c_str(),
        target->getName().c_str());
    delete pqueue;
    return false;
  }

  do {
    cur_vertex = popBestVertex(pqueue, current_cost, real_cost);
    expandNeighbors(pqueue, cur_vertex, real_cost, slack, target);

    if (pqueue->empty() || cur_vertex == _target) {
      if (cur_vertex == _target) {
        delete pqueue;
        return true;
      } else {
        qlog.speakError("Priority queue is empty, cannot find path");
      }
    }

  } while (true);

  delete pqueue;
  return false;

}

void ParRouter::expandNeighbors(QPriorityQueue* pqueue, qvertex current_vertex, double real_length, double slack, ParWireTarget* target) {

  std::pair<vertex2edge::edge_iter , vertex2edge::edge_iter> edge_iter_pair = _graph.get_edges(current_vertex);
  vertex2edge::edge_iter e_iter = edge_iter_pair.first;
  for (; e_iter != edge_iter_pair.second; ++e_iter) {

    qedge cur_edge = *e_iter;
    qvertex target_vertex = _graph.get_other_vertex(cur_edge, current_vertex);

    RoutingNode* e_target_vertex = _graph.get_e_vertex(target_vertex);
    if (!e_target_vertex->isEnabled())  continue;

    double cost = _cost.compute_cost(e_target_vertex, target, slack, real_length);

    double new_real_length = real_length + 1; //proceed one node
    double new_cost = cost + _visited_node[current_vertex];

    if (_visited_node[target_vertex] <= new_cost) continue;

    pqueue->push(new_cost, std::make_pair(new_real_length, target_vertex));
    _visited_node[target_vertex] = new_cost;
    _from_edge[target_vertex] = cur_edge;
  }

}

qvertex ParRouter::popBestVertex(QPriorityQueue* pqueue, double& current_cost, double& real_cost) {
  qvertex best_v;
  PElement pele = pqueue->pop();
  current_cost = pele.first;
  real_cost = pele.second.first;
  best_v = pele.second.second;
  return best_v;
}

void ParRouter::buildRoutePath(std::list<RoutingNode*>& path, std::list<RoutingEdge*>& edges) {
  path.clear();
  edges.clear();
  qvertex current_vertex = _target;
  while (current_vertex != _source) {
    RoutingNode* node = _graph.get_e_vertex(current_vertex);
    path.push_front(node);

    qedge from_edge = _from_edge[current_vertex];
    RoutingEdge* e_edge = _graph.get_e_edge(from_edge);
    edges.push_front(e_edge);
    current_vertex = _graph.get_other_vertex(from_edge, current_vertex);
  }

  RoutingNode* node = _graph.get_e_vertex(current_vertex);
  path.push_front(node);
}



