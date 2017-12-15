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

#ifndef QPAR_ROUTING_GRAPH_HH
#define QPAR_ROUTING_GRAPH_HH


#include "utils/qlog.hh"


class HW_Qubit;
class HW_Interaction;
class HW_Target_Dwave;
class HW_Cell;
class ParTarget;
class ParGrid;

class RoutingNode;
class RoutingEdge;

namespace SYN {
  class Pin;
}

class RoutingGraph : public qpr_graph<RoutingNode*, RoutingEdge*> {

public:
  RoutingGraph(HW_Target_Dwave* dwave_device, ParTarget* par_target);

private:
  HW_Target_Dwave* _dwave_device;
  ParTarget*       _par_target;

  /*! \brief create routing graph for cell
   */
  void createRoutingGraph();


  /*! \brief create routing graph for cell
   *  \brief HW_Cell* hardware cell
   *  \brief ParGrid* placement grid
   */
  void createCellRoutingGraph(HW_Cell* cell, ParGrid* grid);

  /*! \brief create routing graph for intercell connection 
   */
  void createInterCellRoutingGraph();

};


/*! \brief routing node
 */
class RoutingNode {


public:
  RoutingNode(HW_Qubit* qubit);
  RoutingNode(HW_Interaction* interaction);
  RoutingNode(SYN::Pin* pin);

  bool isQubit();
  bool isInteraction();
  bool isPin();

private:
  HW_Qubit*           _qubit;
  HW_Interaction*     _interaction;
  SYN::Pin*           _pin;

  static unsigned _index_counter;

};

/*! \brief routing edge
 */
class RoutingEdge {

public:
  RoutingEdge(RoutingNode* node1, RoutingNode* node2);


private:
  RoutingNode*    node1;
  RoutingNode*    node2;

  static unsigned _index_counter;


};

class RoutingCell {

public:
  RoutingCell(HW_Cell* cell, ParGrid* grid);

private:
  HW_Cell*    _cell;
  ParGrid*    _grid;


};




#endif
