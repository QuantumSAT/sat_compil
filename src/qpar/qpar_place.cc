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

    if (_annealer)
      delete _annealer;
    _annealer = NULL;
}

void QPlace::run() {
  initializePlacement();
}


void QPlace::initializePlacement() {

  // initialize annealer
  float max_r = std::max(_hw_target->getXLimit(), _hw_target->getYLimit());
  _annealer = new Annealer(100.0, 1.0, max_r);


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

void QPlace::usedMatrixSanityCheck() {
  for (unsigned i = 0; i < _used_matrix->getSizeX(); ++i)
    for (unsigned j = 0; j < _used_matrix->getSizeY(); ++y)
      usedMatrixSanityCheck(x, y);
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

void QPlace::generateMove(ParElement* &element, COORD& x, COORD& y) {
  unsigned ele_i = _random_gen->uRand(0, _movable_elements.size());
  element = _movable_elements[ele_i];

  COORD ele_x = element->getX();
  COORD ele_y = element->getY();

  float r_limit = _annealer->getRLimit();

  COORD x_range_min = (ele_x >= r_limit) : ele_x - r_limit ? 0;
  COORD x_range_max = ((ele_x + r_limit) > _hw_target->getXLimit()) ? _hw_target->getXLimit() : (ele_x + r_limit);
  QASSERT((ele_x - x_range_min) < r_limit);
  QASSERT((x_range_max - ele_x) < r_limit);


  COORD y_range_min = (ele_y >= r_limit) : ele_y - r_limit ? 0;
  COORD y_range_max = ((ele_y + r_limit) > _hw_target->getYLimit()) ? _hw_target->getYLimit() : (ele_y + r_limit);
  QASSERT((ele_y - y_range_min) < r_limit);
  QASSERT((y_range_max - ele_y) < r_limit);

  x = _random_gen->iRand(x_range_min, x_range_max);
  y = _random_gen->iRand(y_range_min, y_range_max);
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

void QPlace::findAffectedElementsAndWires(ParElement* element, COORD dest_x, COORD dest_y) {
  QASSERT(_affected_elements.size() == 0);
  element->save();

  ParGrid* src_grid = element->getCurrentGrid();
  src_grid->save();

  ParGrid* tgt_grid = _hw_target->getGrid(dest_x, dest_y);
  tgt_grid->save();


  ParElement* tgt_element = tgt_grid->getCurrentElement();
  bool is_swap = tgt_element;
  
  

  if (is_swap) {
    tgt_element->save();
    _affected_elements.push_back(element);
    _affected_elements.push_back(tgt_element);
    tgt_grid->setParElement(element);
    src_grid->setParElement(tgt_element);

    element->setGrid(tgt_grid);
    tgt_element->setGrid(src_grid);
  } else {
    _affected_elements.push_back(element);
    tgt_grid->setParElement(element);
    src_grid->setParElement(NULL);

    element->setGrid(tgt_grid);

    //only swap to an empty grid need to update the use matrix
    updateUseMatrix(src_grid->getLoc().getLocX(), src_grid->getLoc().getLocY(),
                  dest_x, dest_y);

    //sanity check
    usedMatrixSanityCheck();
  }


}

void QPlace::updateUseMatrix(COORD from_x, COORD from_y, COORD to_x, COORD to_y) {
  QASSERT(from_x != to_x || from_y != to_y);
  bool x_inc = to_x >= from_x;
  bool y_inc = to_y >= from_y;

  if (x_inc == y_inc) {
    for (unsigned i = (x_inc)?from_x:to_x; i < _used_matrix->getSizeX(); ++i) {
      for (unsigned j = (x_inc)?from_y:to_y; j <= ((x_inc)?to_y:from_y - 1); ++j) {
        if (x_inc)
          --_used_matrix->cell(i, j);
        else
          ++_used_matrix->cell(i, j);
      }
    }

    for (unsigned i = (x_inc)?from_x:to_x; i <= ((x_inc)?to_x:from_x - 1); ++i) {
      for (unsigned j = (x_inc)?to_y:from_y; j < _used_matrix->getSizeY(); ++j) {
        if (x_inc)
          --_used_matrix->cell(i, j);
        else
          ++_used_matrix->cell(i, j);
      }
    }

  } else if ( x_inc && !y_inc) {
    for (unsigned i = (x_inc)?from_x:to_x; i <= ((x_inc)?to_y:from_y - 1); ++i) {
      for (unsigned j = (x_inc)?from_y:to_y; j < _used_matrix->getSizeY(); ++j) {
        if (x_inc)
          --_used_matrix->cell(i, j);
        else
          ++_used_matrix->cell(i, j);
      }
    }

    for (unsigned i = (x_inc)?to_x:from_x; i < _used_matrix->getSizeX(); ++i) {
      for (unsigned j = (x_inc)?to_y:from_y; j < ((x_inc)?from_y:to_y - 1); ++j) {
        if (x_inc)
          ++_used_matrix->cell(i, j);
        else
          --_used_matrix->cell(i, j);
      }
    }
  }


}





