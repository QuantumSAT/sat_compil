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



#include "qpar/qpar_routing_graph.hh"
#include "qpar/qpar_system.hh"
#include "qpar/qpar_target.hh"
#include "qpar/qpar_routing_test.hh"
#include "qpar/qpar_route.hh"


void RoutingTester::testRoutingGraph() {
  HW_Target_Dwave* hw_target = _par_system->_hw_target;
  ParTarget* par_target = _par_system->_par_target;
  RoutingGraph graph(hw_target, par_target);
  FastRoutingGraph fast_g(&graph);
  qlog.speak("Fast Graph", "node num %d, edge num %d",
      fast_g._vertex_counter, fast_g._edge_counter);
 
  //check consistency between internal and external graph
  NODES::iterator n_iter = graph._nodes.begin();
  for (; n_iter != graph._nodes.end(); ++n_iter) {
    RoutingNode* node = *n_iter;
    EDGES::iterator e_iter = node->getEdges().begin();
    for (; e_iter != node->getEdges().end(); ++e_iter) {
      RoutingEdge* edge = *e_iter;
      RoutingNode* other_n = edge->getOtherNode(node);
      qvertex inode = fast_g.get_i_vertex(node);
      qedge iedge = fast_g.get_i_edge(edge);
      qvertex i_other_node = fast_g.get_other_vertex(iedge, inode);
      QASSERT(fast_g.get_e_vertex(i_other_node) == other_n);
    }

  }
  
}
