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

#include "qpar/qpar_routing_cost.hh"
#include "qpar/qpar_routing_graph.hh"



double RoutingCostNBR::compute_cost(RoutingNode* node, ParWireTarget* tgt, double slack, double current_length) {
  unsigned load = node->getLoad();
  unsigned capacity = node->getCapacity();

  bool used = node->getCurrentlyUsed();
  unsigned try_add_load = 0;
  if (used) {
    try_add_load = load;
  } else {
    try_add_load = load + 1;
  }

  double base_delay = 1.0;
  double congestion_cost = getCongestionCost(try_add_load, capacity);
  double history_cost = node->getHistoryCost();

  slack = std::min(slack, 0.95);

  double cost = base_delay * (1 - slack) + slack * (base_delay + history_cost + congestion_cost);
  //qlog.speak("ROUTE COST", "base delay %f, history_cost %f, congestion cost %f",
  //    base_delay, history_cost, congestion_cost);

  return cost;

}

double RoutingCostNBR::getCongestionCost(unsigned load, unsigned capacity) {

  if (load <= capacity) return 0.0;

  unsigned overflow = load - capacity;

  double cost = overflow * RoutingNode::getCongestionCost();

  return cost;

}

double RoutingCostSimple::compute_cost(RoutingNode* node, ParWireTarget* tgt, double slack, double current_length) {
  return 1.0;
}
