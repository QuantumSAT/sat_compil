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
#include <unordered_map>
#include <set>
#include <map>
#include <vector>

#include "hw_target/hw_loc.hh"
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
};




typedef std::set<RoutingNode*, RoutingNodeCmp> NODES;
typedef std::set<RoutingEdge*, RoutingEdgeCmp> EDGES;

/*! \brief in the embedding routing graph, the routing graph can be only decided after placement is finished
 */
class RoutingGraph {

public:


  /*! \brief default routing graph constructor based on device and placement result
   */
  RoutingGraph(HW_Target_Dwave* dwave_device, ParTarget* par_target);

  /*! \brief destrctor to free space for all cell, node and edge
   */
  ~RoutingGraph();

  NODES::iterator node_begin() { return _nodes.begin(); }
  NODES::iterator node_end() { return _nodes.end(); }

  EDGES::iterator edge_begin() { return _edges.begin(); }
  EDGES::iterator edge_end() { return _edges.end(); }

  /*! \brief get node number
   */
  unsigned getNodeNum() const { return (unsigned)_nodes.size(); }

  /*! \brief get edge number
   */
  unsigned getEdgeNum() const { return (unsigned)_edges.size(); }

  friend class RoutingCell;
  friend class RoutingTester;

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
  void createInterCellRoutingGraph(ParGrid* grid1, ParGrid* grid2, bool vertical);

  /*! \brief routing graph sanity check
   */
  void sanityCheck() const;
};


/*! \brief routing node
 */
class RoutingNode {


public:

  /*! \brief construct Routing node based on qubit
   */
  RoutingNode(HW_Qubit* qubit, bool logical = false);

  /*! \brief construct Routing node based on interaction
   */
  RoutingNode(HW_Interaction* interaction);

  /*! \brief construct Routing node based on netlist pin
   */
  RoutingNode(SYN::Pin* pin);

  /*! \brief check if the routing node is a qubit
   */
  bool isQubit() const {
    return _qubit;
  }

  /*! \brief check if the routing node is a interaction
   */
  bool isInteraction() const {
    return _interaction;
  }

  /*! \brief check if the routing node is a pin(pseudo node)
   */
  bool isPin() const {
    return _pin;
  }

  /*! \brief get node index
   */
  unsigned getIndex() const { return _node_index; }

  /*! \brief add edge to this routing node
   */
  void addEdge(RoutingEdge* edge) {
    _edges.insert(edge);
  }

  /*! \brief get number of edge that connects to this node
   */
  unsigned getEdgeNum() const {
    return (unsigned)_edges.size();
  }

  /*! \brief get edges 
   */
  EDGES& getEdges() { return _edges; }

  /*! \brief routing graph sanity check
   */
  void sanityCheck() const;

private:

  /*! \brief default constructor
   */
  RoutingNode();

  HW_Qubit*           _qubit;         //!< represents hardware qubit
  HW_Interaction*     _interaction;   //!< represents hardware interaction
  //HW_Cell*            _cell;          //!< the owner hardware cell
  SYN::Pin*           _pin;           //!< netlist pin

  static unsigned _index_counter;
  unsigned _node_index;

  RoutingCell*       _rr_cell;        //!< the routing cell that owns this node
  EDGES              _edges;          //!< edges that connect to this node

  bool               _isLogicalQubit;  //!< indicates if the qubit is combined with two physical qubits

};

/*! \brief routing edge, routing node1 always has smaller index number
 */
class RoutingEdge {

public:
  /*! \brief default constructor for routing edge
   */
  RoutingEdge(RoutingNode* node1, RoutingNode* node2);
 
  /*! \brief get routing node1
   */
  RoutingNode* getRoutingNode1() const { return _node1; }

  /*! \brief get routing node2
   */
  RoutingNode* getRoutingNode2() const { return _node2; }

  /*! \brief get edge index
   */
  unsigned getIndex() const { return _index; }


  RoutingNode* getOtherNode(const RoutingNode* node) const {
    if (node == _node1) return _node2;
    else if (node == _node2) return _node1;
    else
      QASSERT(0);
  }

private:
  RoutingNode*    _node1;
  RoutingNode*    _node2;

  static unsigned _index_counter;
  unsigned _index;

};


/*! for a unplaced cell     for a placed cell
 *  x ----- x               x ----- x
 * (0)     (4)             (pin)   (0)
 *
 *  x ----- x               x ----- x
 * (1)     (5)             (pin)   (1)
 *
 *  x ----- x               x ----- x
 * (2)     (6)             (pin)   (2)
 *
 *  x ----- x                       x
 * (3)     (7)
 */
class RoutingCell {

public:
  /*! \brief default constructor
   */
  RoutingCell(ParGrid* grid, RoutingGraph* graph);

  /*! \brief get routing node based on local index
   */
  RoutingNode* getRoutingNode(const COORD local) const {
    if (_index_to_node.count(local))
      return _index_to_node.at(local);
    else 
      return NULL;
  }

  /*! \brief get routing node based on netlist pin
   */
  RoutingNode* getRoutingNode(SYN::Pin* pin) const {
    if (_pin_to_node.count(pin))
      return _pin_to_node.at(pin);
    else
      return NULL;
  }

  std::vector<RoutingNode*>& getNodes() { return _nodes; }

  

private:
  ParGrid*      _grid;                  //!< Placement and routing grid
  RoutingGraph* _graph;                 //!< Routing graph

  std::map<COORD, RoutingNode*>     _index_to_node;  //!< local index to node
  std::map<SYN::Pin*, RoutingNode*> _pin_to_node;   //!< netlist pin to node
  std::vector<RoutingNode*>         _nodes;

  std::vector<RoutingEdge*>         _edges; //!< edges owned by this cell

  /*! \brief based on placed or unplaced cell to init routing graph
   */
  void initCellRoutingGraph();

};




#endif
