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

#ifndef HW_PARAM_HH
#define HW_PARAM_HH

#include "hw_target/hw_loc.hh"

#include <utility>
#include <cassert>


class HW_Param {

typedef HW_Param SELF;

public:


  /*! \brief destructor
   */
  ~HW_Param() {}

  /*! \brief get max x axis 
   */
  COORD getMaxRangeX() const {
    assert(_max_cell_x != -1);
    return _max_cell_x;
  }


  /*! \brief get max y axis 
   */
  COORD getMaxRangeY() const {
    assert(_max_cell_y != -1);
    return _max_cell_y;
  }

  /*! \brief get max local index
   */
  COORD getMaxRangeLocal() const {
    assert(_max_local_index != -1);
    return _max_local_index;
  }

  /*! \brief set coordinate max range
   */
  void setMaxRangeX(const COORD& x_max) {
    _max_cell_x = x_max;
  }

  /*! \brief set coordinate max range
   */
  void setMaxRangeY(const COORD& y_max) {
    _max_cell_y = y_max;
  }

  /*! \brief set coordinate max range
   */
  void setMaxRangeLocal(const COORD& local) {
    _max_local_index = local;
  }

  /*! \brief get or create a hw paramter class
   */
  static SELF* getOrCreate() {
    if (_self == NULL)
      _self = new HW_Param();
    return _self;
  }


private:

  /*! \brief constructor
   */
  HW_Param() : 
    _max_cell_x(-1),
    _max_cell_y(-1),
    _max_local_index(-1)
  {}


  /*
   */
  static SELF* _self;

  // hardware related parameter
  COORD         _max_cell_x;          //!< max x 
  COORD         _max_cell_y;          //!< max y 
  COORD         _max_local_index;     //!< max local index


};



#endif


