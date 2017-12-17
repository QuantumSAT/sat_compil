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


#include <unordered_set>
#include <set>
#include <vector>

#include "utils/qlog.hh"


class HW_Qubit;
class HW_Interaction;
class HW_Target_Dwave;
class HW_Cell;
class ParTarget;
class ParGrid;
class ParElement;

class RoutingNode;
class RoutingEdge;
class RoutingCell;

namespace SYN {
  class Pin;
}


/*! \brief used in routing node set to make sure deterministic behavior
 */
struct RoutingNodeCmp {
  bool operator()(const RoutingNode* node1, const RoutingNode* node2) const;
};

/*! \brief used in routing edge set to make sure deterministic behavior
 */
struct RoutingEdgeCmp {
  bool operator()(const RoutingEdge* edge1, const RoutingEdge* edge2) const;
}





/*! \brief in the embedding routing graph, the routing graph can be only decided after placement is finished
 */
class RoutingGraph : public qpr_graph<RoutingNode*, RoutingEdge*> {

public:

  typedef std::set<RoutingNode*, RoutingNodeCmp> NODES
  typedef std::set<RoutingEdge*, RoutingEdgeCmp> EDGES

  /*! \brief default routing graph constructor based on device and placement result
   */
  RoutingGraph(HW_Target_Dwave* dwave_device, ParTarget* par_target);

  ~RoutingGraph

private:
  HW_Target_Dwave* _dwave_device;
  ParTarget*       _par_target;

  std::unordered_map<HW_Cell*, RoutingCell*> _cells;
  NODES _nodes;
  EDGES _edges;

  /*! \brief create routing graph for cell
   */
  void createRoutingGraph();


  /*! \brief create routing graph for cell
   *  \brief HW_Cell* hardware cell
   *  \brief ParGrid* placement grid
   */
  void createCellRoutingGraph(ParGrid* grid);


  /*! \brief create routing graph for intercell connection 
   */
  void createInterCellRoutingGraph();

  /*! \brief build fast access routing graph
   */
  void buildFastRoutingGraph();

};


/*! \brief routing node
 */
class RoutingNode {


public:
  RoutingNode(HW_Qubit* qubit);
  RoutingNode(HW_Interaction* interaction);
  RoutingNode(SYN::Pin* pin);

  /*! \brief check if the routing node is a qubit
   */
  bool isQubit();

  /*! \brief check if the routing node is a interaction
   */
  bool isInteraction();

  /*! \brief check if the routing node is a pin(pseudo node)
   */
  bool isPin();

  /*! \brief get node index
   */
  unsigned getIndex() const { return _node_index; }

private:
  HW_Qubit*           _qubit;         //!< represents hardware qubit
  HW_Interaction*     _interaction;   //!< represents hardware interaction
  HW_Cell*            _cell;          //!< the owner hardware cell
  SYN::Pin*           _pin;           //!< netlist pin

  static unsigned _index_counter;
  unsigned _node_index;

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
