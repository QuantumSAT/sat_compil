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
#include "qpar/qpar_routing_graph.hh"
#include "utils/qlog.hh"


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
