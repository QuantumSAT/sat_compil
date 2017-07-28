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

#ifndef HW_OBJECT_HH
#define HW_OBJECT_HH

/*!
 *
 * \file hw_object.hh
 * \author Juexiao Su
 * \date 27 Jul 2017
 * \brief annealer object: cell, qubit, interaction
 *
 */

#include "hw_target/hw_loc.hh"

#include <vector>

/*! \brief this is the abstract class for all annealer component
 *  \class HW_Object
 */
class HW_Object {

public:
  enum HW_OBJ_TYPE {HW_CELL, HW_QUBIT, HW_INTERACTION};

  /*! \brief default constructor
   */
  HW_Object(const HW_Loc& loc) :
    _loc(loc) {}

  /*! \brief default destructor
   */
  virtual ~HW_Object() {}

  /*! \brief pure virtual function to indicate the type of the class
   */
  virtual HW_OBJ_TYPE getType() const = 0;


private:
  HW_Loc _loc; //!< location of the object

};


class HW_Cell;
class HW_Qubit;
class HW_Interaction;


class HW_Cell : public HW_Object {


public:
  HW_Cell()


}


/* \brief HW_Qubit is the class to model the qubit on annealer
 */
class HW_Qubit : public HW_Object {

public:
  typedef std::vector<HW_Interaction*> INTERACTIONS;
  typedef INTERACTIONS::iterator INTER_ITER;
  typedef INTERACTIONS::const_iterator INTER_ITER_CONST;

  /*! \brief default constructor
   */
  HW_Qubit(const COORD x, const COORD y, const COORD local, double max_weight = 2.0, double min_weight = -2.0);

  /*! \brief iterators
   */
  INTER_ITER interaction_begin() { return _interactions.begin(); }
  /*! \brief iterators
   */
  INTER_ITER interaction_end() { return _interactions.end(); }
  /*! \brief iterators
   */
  INTER_ITER_CONST interaction_begin() const { return _interactions.begin(); }
  /*! \brief iterators
   */
  INTER_ITER_CONST interaction_end() const { return _interactions.end(); }

private:
  HW_Qubit(const HW_Qubit&); //!< non-copyable

  HW_Cell* _cell;     //!< the cell that owns this qubit
  INTERACTIONS _interactions;   //!< interactions that connect to this qubit
  double _max_weight;     //!< the max weight
  double _min_weight;     //!< the min weight

  bool _enable;     //!< indicate if the qubit is enabled;
};


class HW_Interaction : public HW_Object {

public: 
  /*! \brief default constructor 
   */
  HW_Interaction(HW_Qubit* qubit1, HW_Qubit* qubit2, double max_weight = 1.0, double min_weight = -1.0);

  /*! \brief get from qubit
   */
  HW_Qubit* getFrom() const { return _from_qubit; }

  /*! \brief get to qubit
   */
  HW_Qubit* getTo() const { return _to_qubit; }

private:
  HW_Interaction(const HW_Interaction&); //!< non-copyable

  HW_Qubit* _from_qubit; //!< from qubit is the qubit with smaller global id
  HW_Qubit* _to_qubit;   //!< to qubit is the qubit with larger global id

  double _max_weight; //!< max weight can be applied to interaction
  double _min_weight; //!< min weight can be applied to interaction

  bool _enable; //! indicate if the interaction is enabled;

}



#endif


