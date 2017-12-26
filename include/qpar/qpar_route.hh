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
#include <list>

class RoutingNode;
class RoutingEdge;
class RoutingGraph;
class HW_Target_Dwave;

class ParTarget;
class ParNetlist;
class ParWireTarget;

class ParRouter;
class RoutingCost;


class FastRoutingGraph : public qpr_graph<RoutingNode*, RoutingEdge*> {
typedef qpr_graph<RoutingNode*, RoutingEdge*> SUPER;

public:
  FastRoutingGraph(RoutingGraph* graph);
  friend class RoutingTester;

};

/*! \brief this class records the route path
 */
class RoutePath : public std::vector<RoutingNode*> {

  typedef std::vector<RoutingNode*> SUPER;
public:
  RoutePath(const std::list<RoutingNode*>& nodes, const std::list<RoutingEdge*>& edges);

  /*! get edges in the path
   */
  std::vector<RoutingEdge*>& getEdges() { return _edges; }


private:
  std::vector<RoutingEdge*> _edges; //!< edges in between every routing nodes in the path


};


struct TargetSlackCmp {

  bool operator()(const ParWireTarget* tgt1, const ParWireTarget* tgt2) const;

};

class QRoute {
public:
  /*! \brief defualt constructor for Routing
   *  \param ParNetlist* netlist used in placement and routing
   *  \param ParWireTarget* hardware target to describe device
   *  \param HW_Target_Dwave* hardware target
   */

  QRoute(ParNetlist* netlist,
      RoutingGraph* rr_graph, FastRoutingGraph* f_graph) :
    _netlist(netlist),
    _rr_graph(rr_graph),
    _f_graph(f_graph)
  {
  }

  /*! \brief default destructor
   */
  ~QRoute();


  /*! \brief execute routing
   */
  void run();

  /*! \brief get routing graph
   */ 
  RoutingGraph* getRoutingGraph() const {
    return _rr_graph;
  }

  /*! \brief print route of each target
   */
  void printAllRoute(std::string filename);

private:
  ParNetlist* _netlist; //!< netlist infomation

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
  void routeTarget(ParWireTarget* target, ParRouter* router);

  /*! \brief build routing path and update the routing graph accordingly
   */
  void updateRoute(ParWireTarget* target, double &slack, ParRouter* router);

  /*! \brief check if the current routing is a valid solution
   */
  bool isRoutingValid(std::vector<ParWireTarget*>& targets, unsigned& overflow);


  /*! \brief check if a given target is overflow
   */
  bool isTargetOverFlow(ParWireTarget* target);


};




#endif

