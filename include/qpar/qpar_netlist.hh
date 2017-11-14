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

class ParGrid;

/*! \brief used in wire set to have deterministic behavior
 */
struct ParWireCmp {

  /*! \brief compare two wire
   *  \function operator()(ParWire*, ParWire*)
   *  \return bool
   */
  bool operator()(ParWire* wire1, ParWire* wire2);

};

struct ParWireTargetCmp {

  /*! \brief compare two ParWireTarget
   *  \function operator()
   *  \return bool
   */
  bool operator()(ParWireTarget* tgt1, ParWireTarget* tgt2);

};

struct ParElementCmp {

  /*! \brief compare two ParWire
   *  \function operator()
   *  \return bool
   */
  bool operator()(ParElement* elem1, ParElement* elem2);
};




typedef std::set<ParWire*, ParWireCmp> ParWireSet;
typedef std::set<ParWireTarget*, ParWireTargetCmp> ParTargetSet;
typedef std::set<ParElement*, ParElementCmp> ParElementSet;
typedef std::set<ParElement*, ParElementCmp>::iterator ELE_ITER;
typedef std::set<ParWire*, ParWireCmp>::iterator WIRE_ITER;

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

  


private:
  /*! \brief build netlist for placement and routing
   *  \function buildParNetlist()
   *  \return void
   */
  void buildParNetlist();

  ParNetlist(const ParNetlist&); //<! non-copyable

  SYN::Model* _syn_netlist; //<! netlist form synthesis
  ParWireSet _wires; //<! wire container in netlist
  ParElementSet _elements; //<! element container in netlist

  std::vector<ParWireTarget*> _all_targets; //!< all wire target



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
      SYN::Pin* src_pin, SYN::Pin* tgt_pin);

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
private:
  ParElement* _source; //<! source element
  ParElement* _target; //<! target element

  SYN::Pin* _src_pin; //<! source pin
  SYN::Pin* _tgt_pin; //<! target pin

  bool _dontRoute; //<! an indicator to decide wheterh it needs routing


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

  /*! \brief build necessary wire data structure
   *  \function buildParWire()
   *  \return std::vector<ParWireTarget*> return target vector
   */
  std::vector<ParWireTarget*>& buildWireTarget(
      const std::unordered_map<SYN::Gate*, ParElement*>& gate_to_par_element);

  /*! \brief initialize bounding box based on the placement
   */
  void initializeBoundingBox();

  /*! \brief sanity check to check if the wire ready to move
   */
  bool isReadyToMove() const { return _bouding_box.checkStatusSame(); };

private:


  SYN::Net* _net; //!< net from synthesis model
  std::vector<ParWireTarget*> _targets; //!< targets on the wire

  ParElement* _source; //!< the source gate of wire
  ParElementSet _elements; //!< all elements that connect to this wire

  static unsigned int _wire_index_counter; //!< a wire counter to generate unique id for each ParWire
  unsigned int _wire_index; //!< uniq index for each wire
  ParSaveAndLoadObject<BoundingBox> _bounding_box; //!< bounding box 



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

  /*! \brief add wire that connects to this element
   *  \function addWire(ParWire* wire)
   *  \param ParWire* wire
   *  \return void
   */
  void addWire(ParWire* wire);

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

private:
  SYN::Gate* _gate; //!< gate form synthesis model

  std::vector<ParWire*> _wires; //!< wire container that stores all wires connect to this element
  ParWire* _sink; //!< the wire that driven by this element. not used

  static unsigned int _element_index_counter; //!< a element counter to generate unique id
  unsigned int _element_index; //!< unique index for each element

  bool _movable; //!< inidicate is this can be moved

  ParSaveAndLoadObject<ParGrid*> _grid; //!< the grid that this element is snap on




};



#endif
