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

#ifndef HW_TARGET_HH
#define HW_TARGET_HH

#include "hw_loc.hh"
#include "hw_param.hh"

#include <boost/functional/hash.hpp>

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>

class HW_Qubit;
class HW_Cell;
class HW_Interaction;

class HW_Target_abstract {

public:

  /*! \brief default constructor
   */
  HW_Target_abstract() {}

  /*! \brief default destructor
   */
  virtual ~HW_Target_abstract() {}

  /*! \brief get qubit by its location
   *  \return a pointer of HW_Qubit
   */
  virtual HW_Qubit* getQubit(const HW_Loc& loc) const = 0;


  /*! \brief get interaction by its location
   *  \return a pointer of HW_Interaction
   */
  virtual HW_Interaction* getInteraction(const HW_Loc& loc) const = 0;
  
};


class HW_Target_Dwave : public HW_Target_abstract {

  typedef std::unordered_map<COORD, HW_Qubit*> LocToQubit;
  typedef std::unordered_map<std::pair<COORD,COORD>, HW_Interaction*, 
          boost::hash<std::pair<COORD,COORD> > > LocToCellInteraction;
public:
  /*! \brief default constructor
   *  \param hw_param hardware related paramter
   */
  HW_Target_Dwave(HW_Param* hw_param) : _hw_param(hw_param) {}

  /*! \brief get the qubit by its location
   *  \param loc the qubit location
   */
  virtual HW_Qubit* getQubit(const HW_Loc& loc) const;

  /*! \brief get the interaction by its location
   *  \param loc the interaction location
   */
  virtual HW_Interaction* getInteraction(const HW_Loc& loc) const;

  /*! \brief get the cell by its location
   *  \param loc the cell location
   */
  HW_Cell* getCell(const HW_Loc& loc) const;

  /*! \brief default destructor
   */
  virtual ~HW_Target_Dwave();

private:

  /*! \brief initialize hardware
   */
  void initializeTarget();

  HW_Param* _hw_param;              //!< a paramter class which holds all hw info

  LocToQubit            _loc_to_qubit;         //!< a map between location and qubit
  LocToCellInteraction  _loc_to_cell;          //!< a map between location and cell
  LocToCellInteraction  _loc_to_interaction;   //!< a map between location and interaction


};



#endif


