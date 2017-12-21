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

#ifndef QPAR_ROUTING_COST_HH
#define QPAR_ROUTING_COST_HH


class RoutingNode;
class ParWireTarget;

class RoutingCost {

public:
  RoutingCost() {}
  virtual ~RoutingCost() {}
  virtual double compute_cost(RoutingNode* node,
                              ParWireTarget* tgt,
                              double slack,
                              double current_length
                              ) = 0;


};

/*! \brief negotiation based routing cost
 */
class RoutingCostNBR {
public:
  RoutingCostNBR() {}
  virtual ~RoutingCostNBR() {}
  virtual double compute_cost(RoutingNode* node,
                              ParWireTarget* tgt,
                              double slack,
                              double current_length);


};

/*! \brief each node will have 1 cost
 */
class RoutingCostSimple {
  RoutingCostSimple() {}
  virtual ~RoutingCostSimple() {}
  virtual double compute_cost(RoutingNode* node,
                              ParWireTarget* tgt,
                              double slack,
                              double current_length);
};



#endif


