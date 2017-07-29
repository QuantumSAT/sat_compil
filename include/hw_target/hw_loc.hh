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

#ifndef HW_LOC_HH
#define HW_LOC_HH

#include <cstdint>
#include <utility>
#include <iostream>

/*!
 *
 * \file hw_loc.hh
 * \author Juexiao Su
 * \date 27 Jul 2017
 * \brief annealer coordinate system
 *
 * Dwave Chimera graph consists of a M x N K4,4 bipartite cell.
 * Here we use a coordinate system which origins at top-left corner to indicate the
 * location of each cell, qubit, interaction
 *
 *        X   X   X.... \
 *      (0,0)           |
 *        X   X   X.... | M rows (Y axis)
 *                      |
 *        X   X   X.... /
 *        \__________/
 *           N columns (X axis)
 *
 *
 *          X   X
 *         (0) (4)
 *          X   X
 *         (1) (5)
 *          X   X
 *         (2) (6)
 *          X   X
 *         (3) (7)
 *      Local qubit index
 *
 *
 */


typedef int64_t COORD;

class HW_Param;
class HW_Loc {

public:

  /*! \brief constructor for qubit/cell loc
   *  \param cell_x x axis coordinate
   *  \param cell_y y axis coordinate
   *  \param local_index qubit local index
   */
  HW_Loc(COORD cell_x, COORD cell_y, COORD local_index);

  /*! \brief constructor for interaction loc
   *  \param qubit_a coordinate for qubit_a
   *  \param qubit_b coordinate for qubit_b
   */
  HW_Loc(COORD qubit_a, COORD qubit_b);

  /*! \brief constructor for interaction loc
   *  \param qubit_a coordinate for qubit_a
   *  \param qubit_b coordinate for qubit_b
   */
  HW_Loc(const HW_Loc& qubit_a, const HW_Loc& qubit_b);

  /*! \brief default constructor
   */
  HW_Loc() : 
    _cell_x(-1),
    _cell_y(-1),
    _local_index(-1),
    _global_index(-1) {
      _interaction_index = std::make_pair(-1, -1);
  }

  /*! \brief copy constructor
   */
  HW_Loc(const HW_Loc&); 

  /*! \brief destructor
   */
  ~HW_Loc() {}

  /*! \brief indicate whether it represents qubit
   *! \return bool
   */
  bool isQubit() const; 

  /*! \brief indicate whether it represents cell
   *! \return bool
   */
  bool isCell() const;

  /*! \brief indicate whether it represents interaction
   *! \return bool
   */
  bool isInteraction() const;

  /*! \brief get X coordinate
   */
  COORD getLocX() const { return _cell_x; }

  /*! \brief get Y coordinate
   */
  COORD getLocY() const { return _cell_y; }

  /*! \brief get local index
   */
  COORD getLocalIndex() const { return _local_index; }

  /*! \brief get global index
   */
  COORD getGlobalIndex() const { return _global_index; }


  /*  \brief convert local index to global index
   *  \param x 
   *  \param y
   *  \param local
   */
  static COORD toGlobalIndex(COORD x, COORD y, COORD local);

  /*  \brief convert global index to cell x
   */
  static COORD globalIndexToX(COORD global_index);

  /*  \brief convert global index to cell y
   */
  static COORD globalIndexToY(COORD global_index);

  /*  \brief convert global index to cell local index
   */
  static COORD globalIndexToLocalIndex(COORD global_index);

  /* \brief set hw parameter
   */
  static void setHWParam(HW_Param* param) { _hw_param = param; }

  friend std::ostream& operator<< (std::ostream& stream, const HW_Loc& loc);

private:

  COORD                     _cell_x;                //!< cell x that indicates column number
  COORD                     _cell_y;                //!< cell y that inidcates row number
  COORD                     _local_index;           //!< indicates local qubit index
  COORD                     _global_index;          //!< global index which is the same with index defined by D-Wave
  std::pair<COORD, COORD>   _interaction_index;     //!< global index pair for interaction

  static HW_Param*          _hw_param;              //!< hardware parameter




};





#endif


