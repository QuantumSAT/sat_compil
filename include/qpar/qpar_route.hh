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

#ifndef QPAR_ROUTE_HH
#define QPAR_ROUTE_HH

/*!
 * \file qpar_route.hh
 * \author Juexiao Su
 * \date 18 Dec 2017
 * \brief routing
 */

#include "qpar_graph.hh"

class RoutingNode;
class RoutingEdge;
class RoutingGraph;
class HW_Target_Dwave;

class ParNetlist;
class ParTarget;

class ParRouter;


class FastRoutingGraph : public qpr_graph<RoutingNode*, RoutingEdge*> {
typedef qpr_graph<RoutingNode*, RoutingEdge*> SUPER;

public:
  FastRoutingGraph(RoutingGraph* graph);
  friend class RoutingTester;

};


struct TargetSlackCmp {

  bool operator()(const ParWireTarget* tgt1, const ParWireTarget* tgt2) const;

}

class QRoute {
public:
  /*! \brief defualt constructor for Routing
   *  \param ParNetlist* netlist used in placement and routing
   *  \param ParTarget* hardware target to describe device
   *  \param HW_Target_Dwave* hardware target
   */

  QRoute(HW_Target_Dwave* dwave, ParNetlist* netlist, ParTarget* target) :
    _dwave_target(dwave),
    _netlist(netlist),
    _par_target(target),
    _rr_graph(NULL),
    _f_graph(NULL)
  {
  }

  /*! \brief default destructor
   */
  ~QRoute() {
    if (_cost) delete _cost;
    _cost = NULL;

    if (_f_graph) delete _f_graph;
    _f_graph = NULL;

    if (_rr_graph) delete _rr_graph;
    _rr_graph = NULL;
  }


  /*! \brief execute routing
   */
  void run();

private:
  HW_Target_Dwave* _dwave_target; //!< hardware target
  ParNetlist* _netlist; //!< netlist infomation
  ParTarget* _par_target; //!< placement and routing target

  RoutingGraph* _rr_graph; //!< routing graph
  FastRoutingGraph* _f_graph; //!< fast routing graph

  RoutingCost* _cost;
  RoutingCost* _cost_simple;


  ParRouter* _router; //!< router used in rest routing iterations
  ParRouter* _first_router; //!< router used in routing first iteration

  double _longest_wire_length; //!< longest wire length

  /*! \brief initialize necessary datastructure for routing
   */
  void initializeRouting();

  /*! \brief build routing graph and fast routing graph
   */
  void buildRoutingGraph();


  /*! \brief initialize wire slack
   */
  void initializeWireSlack();

  /*! \brief update wire slack
   */
  void updateWireSlack();

  /*! \brief exit condition for routing
   */
  bool shouldExit();

  /*! \brief route each target one by one;
   */
  void routeAllTarget(std::vector<ParWireTarget*>& targets, unsigned iter);


  /*! \brief update history cost in the nbr algorithm
   */
  void updateHistoryCost(unsigned iter);

  /*! \brief route single target
   */
  void routeTarget(ParWireTarget* target);

  /*! \brief build routing path and update the routing graph accordingly
   */
  void updateRoute(ParTarget* target, double &slack, ParRouter* router);


};




#endif

