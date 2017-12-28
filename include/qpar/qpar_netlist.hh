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

#ifndef QPAR_NETLIST_HH
#define QPAR_NETLIST_HH

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <set>

#include "qpar/qpar_sl_object.hh"
#include "qpar/qpar_utils.hh"
#include "hw_target/hw_loc.hh"


/*!
 * \file qpar_netlist.hh
 * \author Juexiao Su
 * \date 22 Oct 2017
 * \brief class to convert generic logic gate netlist to a light weight netlist for placement and routing
 */

namespace SYN {
  class Model;
  class Net;
  class Pin;
  class Gate;
}

class ParWire;
class ParWireTarget;
class ParElement;

class RoutingNode;

class ParGrid;
class RoutePath;

/*! \brief used in wire set to have deterministic behavior
 */
struct ParWireCmp {

  /*! \brief compare two wire
   *  \function operator()(ParWire*, ParWire*)
   *  \return bool
   */
  bool operator()(const ParWire* wire1, const ParWire* wire2) const;

};

struct ParWireTargetCmp {

  /*! \brief compare two ParWireTarget
   *  \function operator()
   *  \return bool
   */
  bool operator()(const ParWireTarget* tgt1, const ParWireTarget* tgt2) const;

};

struct ParElementCmp {

  /*! \brief compare two ParWire
   *  \function operator()
   *  \return bool
   */
  bool operator()(const ParElement* elem1, const ParElement* elem2) const;
};




typedef std::set<ParWire*, ParWireCmp> ParWireSet;
typedef std::set<ParWireTarget*, ParWireTargetCmp> ParTargetSet;
typedef std::set<ParElement*, ParElementCmp> ParElementSet;
typedef std::set<ParElement*, ParElementCmp>::iterator ELE_ITER;
typedef std::set<ParWire*, ParWireCmp>::iterator WIRE_ITER;
typedef std::vector<ParWire*>::iterator WIRE_ITER_V;

/* a class to support incremental bounding box update
 */
class BoundingBox {

public:
  /*! default constructor
   */
  BoundingBox() :
    _bound(-1,-1,-1,-1),
    _edge_num(-1,-1,-1,-1) {}

  /*! constructor
   */
  BoundingBox(Box bound, Box edge) :
    _bound(bound),
    _edge_num(edge) {}


  /*! \brief set bounding box
   */
  void setBoundingBox(Box bound) { _bound = bound; }

  /*! \brief set edge box
   */
  void setEdgeBox(Box edge) { _edge_num = edge; }

  /*! \brief get bounding box
   */
  Box& getBoundBox() { return _bound; }

  /*! \brief get edge number box
   */
  Box& getEdgeBox() { return _edge_num; }

  bool operator==(const BoundingBox& rhs) const {
    return (this->_bound == rhs._bound &&
            this->_edge_num == rhs._edge_num);
  }


private:
  Box _bound; //!< bounding box value
  Box _edge_num; //!< number of element on the edge of box


};




/*! \brief ParNetlist is a light weight netlist to 
 *         represent logic netlist, but mainly focuses
 *         on connectivity
 *
 */
class ParNetlist {
typedef ParNetlist SELF;
public:

  /*! \brief defualt destructor
   */
  ~ParNetlist();

  /*! \brief default constructor
   *  \function constructor
   *  \param SYN::Model netlist from blif file
   */
  ParNetlist(SYN::Model* model);

  ELE_ITER element_begin() { return _elements.begin(); }
  ELE_ITER element_end() { return _elements.end(); }

  WIRE_ITER wire_begin() { return _wires.begin(); }
  WIRE_ITER wire_end() { return _wires.end(); }


  WIRE_ITER model_wire_begin() { return _model_wires.begin(); }
  WIRE_ITER model_wire_end() { return _model_wires.end(); }

  /*! \brief get number of wires in the netlist
   */
  size_t getWireNum() const { return _wires.size(); }

  /*! \brief set top netlist based on syn netlist
   */ 
  static void setTopNetlist(SELF* netlist) { _top_netlist = netlist; }

  /*! \brief get top netlist
   */
  static SELF* getTopNetlist() { return _top_netlist; }

  /*! \brief get all wire targets
   */
  std::vector<ParWireTarget*>& getTargets() { return _all_targets; }

  /*! \brief get total number element in the netlist
   */
  size_t getElementNumber() const { return _elements.size(); }


private:
  /*! \brief build netlist for placement and routing
   *  \function buildParNetlist()
   *  \return void
   */
  void buildParNetlist();

  ParNetlist(const ParNetlist&); //<! non-copyable

  SYN::Model* _syn_netlist; //<! netlist form synthesis
  ParWireSet _wires; //<! wire container in netlist
  ParWireSet _model_wires; //<! wire conainter for wires that only connect to top model port
  ParElementSet _elements; //<! element container in netlist

  std::vector<ParWireTarget*> _all_targets; //!< all wire target

  static SELF* _top_netlist; //<! top netlist that is used by different step


};

/*! \brief a target includes a source and a destination
 * a ParWire may contain multiple targets if the wire is
 * a multi fan-out wire
 */
class ParWireTarget {
public:
 
  /*! \brief default constructor
   */
  ParWireTarget(ParElement* source, ParElement* dest, 
      SYN::Pin* src_pin, SYN::Pin* tgt_pin, ParWire* wire);

  /*! \brief default destructor
   */
  ~ParWireTarget() {}

  /*! \brief set the dont route flag
   *  \param bool value
   *  \return void
   */
  void setDontRoute(bool val) { _dontRoute = val; }

  /*! \brief get dont route flag
   */
  bool getDontRoute() const { return _dontRoute; }

  /*! \brief get target uniqId
   */
  unsigned getUniqId() const {
    return _target_index; 
  }

  /*! \brief get target slack
   */
  double getSlack() const;

  /*! \brief get source element
   */
  ParElement* getSourceElement() const {
    return _source;
  }

  /*! \brief get target element
   */
  ParElement* getTargetElement() const {
    return _target;
  }

  /*! \brief get pin name
   */
  std::string getName() const;

  /*! \brief get source pin
   */
  SYN::Pin* getSourcePin() const {
    return _src_pin;
  }

  /*! \brief get target pin
   */
  SYN::Pin* getTargetPin() const {
    return _tgt_pin;
  }

  /*! \brief ripup existing route of target
   */
  void ripupTarget();

  /*! \brief get route path
   */
  RoutePath* getRoutePath() const {
    return _route;
  }

  /*! \brief set route path
   */
  void setRoutePath(RoutePath* route) {
    _route = route;
  }

  /*! \brief get wire
   */
  ParWire* getWire() const {
    return _wire;
  }

  /*! \brief print route;
   */
  void printRoute(std::ostream& out) const;

private:
  ParElement* _source; //<! source element
  ParElement* _target; //<! target element

  SYN::Pin* _src_pin; //<! source pin
  SYN::Pin* _tgt_pin; //<! target pin

  ParWire* _wire; //!< wire that owns this target

  bool _dontRoute; //<! an indicator to decide wheterh it needs routing
  unsigned _target_index; //<! target index
  static unsigned _wire_target_counter; //!< index counter

  RoutePath* _route; //<! current route of the target;


};


/*! \brief wire that connects different gates
 */
class ParWire {
  
public:
  /*! \brief default constructor
   */
  ParWire(SYN::Net* net);

  /*! \brief default destructor
   * delete all wire targets managed by this wire
   */
  ~ParWire();

  /*! \brief add element that conencts to this wire
   *  \function addElement(ParElement* elem)
   *  \param ParElement* a element pointer
   */
  void addElement(ParElement* elem) {
    _elements.insert(elem);
  }

  size_t getElementNumber() const {
    return _elements.size();
  }

  /*! \brief build necessary wire data structure
   *  \function buildParWire()
   *  \return std::vector<ParWireTarget*> return target vector
   */
  std::vector<ParWireTarget*>& buildWireTarget(
      const std::unordered_map<SYN::Gate*, ParElement*>& gate_to_par_element,
      ParElementSet& elements);

  /*! \brief initialize bounding box based on the placement
   */
  void initializeBoundingBox();

  /*! \brief sanity check to check if the wire ready to move
   */
  bool isReadyToMove() const { return _bounding_box.checkStatusSame(); };

  std::string getName() const;

  /*! \brief incrementally update bounding box
   */
  void updateBoundingBox(COORD from_x, COORD from_y, COORD to_x, COORD to_y);


  /*! \brief get current cost
   *  \return double cost
   */
  double getCurrentCost() const;

  /*! \brief get saved cost
   *  \return double cost
   */
  double getSavedCost() const;

  /*! \brief save cost
   */
  void saveCost();

  /*! \brief set cost
   */
  void setCost(double val);

  /*! \brief restore the wire to the state before swap
   */
  void restore();

  /*! \brief get current bounding box
   */
  BoundingBox getCurrentBoundingBox() const;

  /*! \brief get saved bounding box
   */
  BoundingBox getSavedBoundingBox();

  /*! \brief save bouding box
   */
  void saveBoundingBox();

  /*! \brief set current bounding box
   */
  void setBoundingBox(BoundingBox box);

  /*! \brief check if incremetal update is the same with 
   *         brute force
   */
  bool sanityCheck();

  /*! \brief get wire uniq id 
   */
  unsigned int getUniqId() const { return _wire_index; }

  /*! \brief get the only element
   */
  ParElement* getUniqElement();


  /*! \brief get slack
   */
  double getSlack() const { return _slack; }

  /*! \brief set slack
   */
  void setSlack(double slack) { _slack = slack; }


  /*! \brief mark the used routing resource on the routing graph
   */
  void markUsedRoutingResource();

  /*! \brief unmark the used routing resource
   */
  void unmarkUsedRoutingResource();

  /*! \brief get all used routing nodes
   */
  std::unordered_set<RoutingNode*>& getUsedRoutingNodes() { return _routing_nodes; }

  /*! \brief get routing node usage info
   */
  const std::unordered_map<RoutingNode*, unsigned>& getRoutingNodeUsage() {
    return _routing_node_usage;
  }

  /*! \brief populate removal information to wire
   */
  void removeUsedRoutingNode(RoutingNode* node);

  /*! \brief get wire length
   */
  double getWireLength() const { return (double)(_routing_nodes.size()); }

  /*! \brief update wire related route info
   */
  void updateWireRoute(RoutePath* route);


  /*! \brief print self
   */
  void printSelf() const;

  /*! \brief for model wire get its only gate Pin
   */
  SYN::Pin* getUniqElementPin() const;

  /*! \brief check if routing node usage match each route of targets
   */
  void checkRoutingNodeUsage();

private:
  /*! \brief re-compute bouding box
   */
  void recomputeBoundingBox();


  SYN::Net* _net; //!< net from synthesis model
  std::vector<ParWireTarget*> _targets; //!< targets on the wire

  ParElement* _source; //!< the source gate of wire
  ParElementSet _elements; //!< all elements that connect to this wire

  static unsigned int _wire_index_counter; //!< a wire counter to generate unique id for each ParWire
  unsigned int _wire_index; //!< uniq index for each wire
  ParSaveAndLoadObject<BoundingBox> _bounding_box; //!< bounding box 
  ParSaveAndLoadObject<double> _cost; //!< cost of the wire


  //routing related data
  double _slack; //!< the wire slack
  std::unordered_map<RoutingNode*, unsigned> _routing_node_usage; //!< a record of routing node usage
  std::unordered_set<RoutingNode*> _routing_nodes; //!< all used routing nodes



};

/*! \brief ParElement is the element will be directly mapped to hardware
 *
 * Each ParElement will only have one output, as each gate after will only have one output.
 * The above assumption can be extented
 */
class ParElement {

public:
  /*! \brief default constructor
   */
  ParElement(SYN::Gate* gate);
  ParElement(SYN::Pin* pin);

  /*! \brief add wire that connects to this element
   *  \function addWire(ParWire* wire)
   *  \param ParWire* wire
   *  \return void
   */
  void addWire(ParWire* wire);

  /*! \brief disconnect a wire
   */
  void disconnectWire(ParWire* wire);

  /*! \brief check is this element can be moved
   */
  bool isMovable() const { return _movable; }

  /*! \brief set the grid that will have this element
   */
  void setGrid(ParGrid* grid);


  /*1 \brief get current grid
   */
  ParGrid* getCurrentGrid() const;

  /*! \brief save current status
   */ 
  void save();

  /*! \brief restore to previous status
   */
  void restore();

  /*! \brief get the x coordinate of the element if the element is placed
   */
  COORD getX() const;

  /*! \brief get the y coordinate of the element if the element is placed
   */
  COORD getY() const;

  /*! \brief check if the element is ready to move
   */
  bool isReadyToMove() const {
    return _grid.checkStatusSame();
  }

  /*! \brief write the placmeent result back to ParNetlist
   */
  void updatePlacement();

  /*! \brief get element name
   */
  std::string getName() const;

  /*! \brief get the current placement of the element
   */
  std::pair<COORD,COORD> getPlacement() const;

  /*! \brief get uniq id for the element
   */
  unsigned int getUniqId() const {
    return _element_index;
  }

  /*! \brief get netlist gate
   */
  SYN::Gate* getSynGate() const { return _gate; }

  /*! \brief get netlist pin
   */
  SYN::Pin* getPin() const { return _pin; }


  WIRE_ITER_V begin() { return _wires.begin(); }
  WIRE_ITER_V end() { return _wires.end(); }

private:
  SYN::Gate* _gate; //!< gate form synthesis model
  SYN::Pin* _pin; //!< pin from nelist model pin

  std::vector<ParWire*> _wires; //!< wire container that stores all wires connect to this element
  ParWire* _sink; //!< the wire that driven by this element. not used

  static unsigned int _element_index_counter; //!< a element counter to generate unique id
  unsigned int _element_index; //!< unique index for each element

  bool _movable; //!< inidicate is this can be moved

  ParSaveAndLoadObject<ParGrid*> _grid; //!< the grid that this element is snap on

  COORD _x;  //!< post placement result

  COORD _y;  //!< post placement result




};



#endif
