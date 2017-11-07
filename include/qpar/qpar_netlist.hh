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
#include <set>


/*!
 * \file qpar_netlist.hh
 * \author Juexiao Su
 * \date 22 Oct 2017
 * \brief class to convert generic logic gate netlist to a light weight netlist for placement and routing
 */

namespace SYN {
  class Model;
  class Net;
}

class ParWire;
class ParWireTarget;
class ParElement;

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

  /*! \brief compare two PairWireTarget
   *  \function operator()
   *  \return bool
   */
  bool operator()(ParWireTarget* tgt1, ParWireTarget* tgt2);

};

struct ParWireCmp {

  /*! \brief compare two PairWire
   *  \function operator()
   *  \return bool
   */
  bool operator()(ParWire* wire1, ParWire* wire2);

};

struct ParElementCmp {

  /*! \brief compare two PairWire
   *  \function operator()
   *  \return bool
   */
  bool operator()(ParElement* elem1, ParElement* elem2);
};




typedef std::set<PairWire*, PairWireCmp> ParWireSet;
typedef std::set<PairWireTarget*, PairWireTargetCmp> ParTargetSet;
typedef std::set<PairElement*, ParElementCmp> ParElementSet;


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

  /*! \brief build netlist for placement and routing
   *  \function buildParNetlist()
   *  \return void
   */
  void buildParNetlist();

  static SELF* getOrCreate(SYN::Model* model);

private:
  /*! \brief default constructor
   *  \function constructor
   *  \param SYN::Model netlist from blif file
   */
  ParNetlist(SYN::Model* model);

  ParNetlist(const ParNetlist&); //<! non-copyable

  SYN::Model* _syn_netlist //<! netlist form synthesis
  ParWireSet _wires; //<! wire container in netlist
  ParElementSet _elements; //<! element container in netlist


  static SELF* _self; 



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

private:

  /*! \brief build necessary wire data structure
   *  \function buildParWire()
   *  \return std::vector<ParWireTarget*> return target vector
   */
  std::vector<ParWireTarget*>& buildWireTarget(
      const std::unordered_set<SYN::Gate*, ParElement*>& gate_to_par_element);

  SYN::Net* _net; //!< net from synthesis model
  std::vector<ParWireTarget*> _targets; //!< targets on the wire

  ParElement* _source; //!< the source gate of wire
  ParElementSet _elements; //!< all elements that connect to this wire

  static unsigned int _wire_index_counter; //!< a wire counter to generate unique id for each ParWire
  unsigned int _wire_index; //!< uniq index for each wire


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

private:
  SYN::Gate* _gate; //!< gate form synthesis model

  std::vector<ParWire*> _wires; //!< wire container that stores all wires connect to this element
  ParWire* _sink; //!< the wire that driven by this element. not used

  static unsigned int _element_index_counter; //!< a element counter to generate unique id
  unsigned int _element_index; //!< unique index for each element

};



#endif
