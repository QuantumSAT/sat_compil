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
#include "hw_target/hw_target.hh"
#include "hw_target/hw_loc.hh"
#include "utils/qlog.hh"


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
unsigned RouitngEdge::_index_counter = 0;


void RoutingGraph::createRoutingGraph() {

  for (COORD x = 0; x < _par_target->getXLimit(); ++x) {
    for (COORD y = 0; y < _par_target->getYLimit(); ++y) {
      ParGrid* grid = _par_target->getGrid(x, y);
      createCellRoutingGraph(grid);
    }
  }



}
