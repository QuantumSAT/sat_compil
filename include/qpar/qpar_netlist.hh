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

public:
  /*! \brief default constructor
   *  \function constructor
   *  \param SYN::Model netlist from blif file
   */
  ParNetlist(SYN::Model* model);

  /*! \brief defualt destructor
   */
  ~ParNetlist();

  /*! \brief build netlist for placement and routing
   *  \function buildParNetlist()
   *  \return void
   */
  void buildParNetlist();

private:

  ParWireSet _wires; //<! wire container in netlist
  ParElementSet _elements; //<! element container in netlist

  static ParNetlist* _self; 



};



/*! \brief wire that connects different gates
 */
class ParWire {
  
public:
  /*! \brief default constructor
   */
  ParWire(SYN::Net* net);


private:

  /*! \brief build necessary wire data structure
   *  \function buildParWire()
   *  \return void
   */
  void buildParWire();

  SYN::Net* _net; //!< net from synthesis model
  std::vector<ParWireTarget*> _targets; //!< targets on the wire

  ParElement* _source; //!< the source gate of wire
  std::vector<ParElement*> _elements; //!< all elements that connect to this wire

  static unsigned int _wire_index_counter; //!< a wire counter to generate unique id for each ParWire
  unsigned int _wire_index; //!< uniq index for each wire


};

class ParElement {

public:
  /*! \brief default constructor
   */
  ParElement(SYN::Gate* gate);


private:
  SYN::Gate* _gate; //!< gate form synthesis model

  std::vector<ParWire*> _wires; //!< wire container that stores all wires connect to this element
  ParWire* _sink; //!< the wire that driven by this element

  static unsigned int _element_index_counter; //!< a element counter to generate unique id
  unsigned int _element_index; //!< unique index for each element

};



#endif
