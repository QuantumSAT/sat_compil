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


#ifndef QPAR_TARGET_HH
#define QPAR_TAEGET_HH


#include "qpar/qpar_sl_object.hh"
#include "hw_target/hw_loc.hh"

#include <boost/functional/hash.hpp>
#include <vector>
#include <unordered_map>


/*!
 *  \file qpar_target.hh
 *  \author Juexiao Su
 *  \data 06 NOV 2017
 *  \brief class to represent target in placement and routing
 */

class HW_Target_Dwave;
class HW_Cell;

class ParElement;

/*! \brief each cell is a grid in the target
 */
class ParGrid {

public:
  ParGrid(HW_Cell* cell) : _cell(cell),
   _canbeplaced(true) {
    _grid_index = _grid_index_counter;
    ++_grid_index_counter;
  }

  /*! \brief set element to this grid
   */
  void setParElement(ParElement* element);

  /*! \brief get current placed element
   */
  ParElement* getCurrentElement() const;

  /*! \brief get uniq index
   */
  unsigned getIndex() const { return _grid_index; }

  /*! \brief save status
   */
  void save();

  /*! \brief restore status
   */
  void restore();

  /*! \brief check if the grid can be placed
   */
  bool canBePlaced() const { return _canbeplaced; }

  /*! \brief get grid location
   */
  HW_Loc getLoc() const;

  /*! \brief check if grid is ready to move
   */
  bool checkIfReadyToMove() const {
    return _element.checkStatusSame();
  }

private:

  HW_Cell* _cell; //!< the hardware cell to create this grid
  static unsigned _grid_index_counter; //!< an counter to create uniq id for each grid
  unsigned _grid_index; //!< unique id for each grid

  ParSaveAndLoadObject<ParElement*> _element;

  bool _canbeplaced; //!< a flag to indicate if this grid can be used to place

};

class ParGridContainer : public std::vector<ParGrid*> {
  typedef std::vector<ParGrid*> SUPER;
  typedef ParGridContainer SELF;

  struct GridCmp {
    bool operator()(ParGrid* grid1, ParGrid* grid2) {
      return grid1->getIndex() < grid2->getIndex();
    }
  };
public:
  /*! \brief shuffle the order of ParGrid*
   */
  void shuffle();

  /*! \brief sort the containter based on id
   */
  void sortId();

};


class ParTarget {

public:
  typedef std::unordered_map<std::pair<COORD, COORD>, ParGrid*, 
          boost::hash<std::pair<COORD, COORD> > > LocToGrid;

public:
  /*! \brief default constructor
   */
  ParTarget(HW_Target_Dwave* hw_target) : _hw_target(hw_target),
  _maxX(0), _maxY(0) {}

  /*! \brief default destructor
   */
  ~ParTarget();

  /*! \brief initialize Par target
   *  \return void
   */
  void initParTarget();

  /*! \brief get the grid base on coordinator
   *  \param COORD coordinator x
   *  \param COORD coordinator y
   *  \return ParGrid* grid pointer
   */
  ParGrid* getGrid(COORD x, COORD y) const {
    if (_grids.count(std::make_pair(x, y))) {
      return _grids.at(std::make_pair(x, y));
    } else {
      return NULL;
    }
  }

  /*! \brief get the grid vector container
   */
  ParGridContainer& getGrids() { return _grid_vector; }

  /*! \brief get x limit
   */
  COORD getXLimit() const { return _maxX; }

  /*! \brief get y limit
   */
  COORD getYLimit() const { return _maxY; }


private:
  HW_Target_Dwave* _hw_target; //!< hardware target
  ParGridContainer _grid_vector; //!< a vector that stores all grid
  LocToGrid _grids; //!< find the grid based on location

  COORD _maxX; //!< number of cells on x direction
  COORD _maxY; //!< number of cells on y direction

};



#endif
