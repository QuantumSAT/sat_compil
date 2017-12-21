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

#ifndef QPAR_ROUTER_HH
#define QPAR_ROUTER_HH


class FastRoutingGraph;
class RoutingCost;
class RoutingNode;
class ParWireTarget;

#include <unordered_set>

/*!
 * \file qpar_router.hh
 * \author Juexiao Su
 * \date 20 Dec 2017
 * \brief router which implements shortest path algorithm
 */

class ParRouter {


public:
  ParRouter(FastRoutingGraph& graph, RoutingCost& cost) :
    _graph(graph), _cost(cost) {}



  /*! route target
   */
  bool route(RoutingNode* src, RoutingNode* tgt, double slack, std::unordered_set<RoutingNode*>& used, ParWireTarget* target);

private:
  FastRoutingGraph& _graph; //!< routing graph;
  RoutingCost& _cost; //!< routing cost;


};







#endif

