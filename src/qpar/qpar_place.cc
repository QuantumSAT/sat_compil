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

/*!
 * \file qpar_place.cc
 * \author Juexiao Su
 * \date 29 Oct 2017
 * \brief class to implemement data structure and algorithm for placement
 */


#include "qpar/qpar_place.hh"
#include "qpar/qpar_target.hh"
#include "qpar/qpar_netlist.hh" 


#include "utils/qlog.hh"


QPlace::~QPlace() {
    if (_used_matrix)
      delete _used_matrix;
    _used_matrix = NULL;
}

void QPlace::run() {
  initilizePlacement();
}


void QPlace::initilizePlacement() {
  ParGridContainer& grids = _hw_target->getGrids();
  grids.shuffle();
  unsigned grid_index = 0;

  ELE_ITER ele_iter = _netlist->element_begin();
  for (; ele_iter != _netlist->element_end(); ++ele_iter) {
    ParElement* element = *ele_iter;
    if (!element->isMovable()) {
      qlog.speakError("Placement does not support fixed element!");
    }
    _movable_elements.push_back(element);

    while (!grids[grid_index]->canBePlaced() && grid_index < grids.size())
      ++grid_index;

    if (grid_index == grids.size()) 
      qlog.speakError("Placement does not have enough resources to performance placement");

    element->setGrid(grids[grid_index]);
    grids[grid_index]->setParElement(element);
    ++grid_index;
  }


  // initilize bounding box for each wire
  WIRE_ITER w_iter = _netlist->wire_begin();
  for(; w_iter != _netlist->wire_end(); ++w_iter) {
    ParWire* wire = *w_iter;
    wire->initializeBoundingBox();
  }

  COORD y_limit = _hw_target->getYLimit();
  COORD x_limit = _hw_target->getXLimit();

  QASSERT(y_limit);QASSERT(x_limit);
  _used_matrix = new qpr_matrix<unsigned>(x_limit, y_limit);

  // initilize grid utilization matrix
  if (_hw_target->getGrid(0,0)->getCurrentElement())
    _used_matrix->cell(0,0) = 1;
  else
    _used_matrix->cell(0,0) = 0;

  for (COORD y = 1; y < y_limit; ++y) {
    if (_hw_target->getGrid(0,y)->getCurrentElement())
      _used_matrix->cell(0,y) = _used_matrix->cell(0,y-1) + 1;
    else
      _used_matrix->cell(0,y) = _used_matrix->cell(0,y-1);
  }

  for (COORD x = 1; x < x_limit; ++x) {
    if (_hw_target->getGrid(x, 0)->getCurrentElement())
      _used_matrix->cell(x, 0) = _used_matrix->cell(x-1,0) - 1;
    else
      _used_matrix->cell(x, 0) = _used_matrix->cell(x-1,0);
  }

  for (COORD x = 1; x < x_limit; ++x) {
    for (COORD y = 1; y < y_limit; ++y) {
      unsigned used_element = 
        _used_matrix->cell(x-1, y) +
        _used_matrix->cell(x, y-1) -
        _used_matrix->cell(x-1, y-1);
      if (_hw_target->getGrid(x,y)->getCurrentElement())
        _used_matrix->cell(x,y) = used_element + 1;
      else
        _used_matrix->cell(x,y) = used_element;
    }
  }
}

void QPlace::usedMatrixSanityCheck(unsigned x, unsigned y) {
  unsigned sum = 0;
  for (COORD i = 0; i < x; ++i) {
    for (COORD j = 0; j < y; ++j) {
      if (_hw_target->getGrid(x,y)->getCurrentElement())
        ++sum;
    }
  }
  QASSERT(_used_matrix->cell(x, y) == sum);
}

void QPlace::checkIfReadyToMove() {
  WIRE_ITER w_iter = _netlist->wire_begin();
  for(; w_iter != _netlist->wire_end(); ++w_iter) {
    ParWire* wire = *w_iter;
    QASSERT(wire->isReadyToMove());
  }

  ELE_ITER ele_iter = _netlist->element_begin();
  for (; ele_iter != _netlist->element_end(); ++ele_iter) {
    ParElement* element = *ele_iter;
    QASSERT(element->isReadyToMove());
  }

  for (COORD x = 1; x < x_limit; ++x) {
    for (COORD y = 1; y < y_limit; ++y) {
      QASSERT(_hw_target->getGrid(x,y)->isReadyToMove());
    }
  }


}


void QPlace::tryMove() {
  //1) check if they placer is ready to move
  checkIfReadyToMove();

  COORD& x = std::numeric_limits<COORD>::max();
  COORD& y = std::numeric_limits<COORD>::max();

  ParElement* ele = NULL;
  generateMove(ele, x, y);

  findAffectedElementsAndWires(ele, x, y);


}





