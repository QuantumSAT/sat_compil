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

void QPlace::run() {
  initilizePlacement();
}


void QPlace::initilizePlacement() {
  ParGridContainer& grids = _hw_target->getGrids();
  grids.shuffle();
  unsigned grid_index = 0;

  ELE_ITER ele_iter = _netlist.element_begin();
  for (; ele_iter != _netlist.end(); ++ele_iter) {
    ParElement* element = *ele_iter;
    if (!element->isMovable()) {
      qlog.speakError("Cannot support fixed element!");
    }

    while (!grids[grid_index]->canBePlaced() && grid_index < grids.size())
      ++grid_index;

    if (grid_index == grids.size()) 
      qlog.speakError("Placement does not have enough resources to performance placement");

    element->setGrid(grids[grid_index]);
    grids->setParElement(element);
    ++grid_index;
  }

}





