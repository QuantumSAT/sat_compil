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


void RoutingTester::testRoutingGraph() {
  HW_Target_Dwave* hw_target = _par_system->_hw_target;
  ParTarget* par_target = _par_system->_par_target;

  RoutingGraph graph(hw_target, par_target);

}
