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

#include "hw_target/hw_object.hh"


HW_Cell::HW_Cell(COORD x, COORD y, HW_Target_abstract* hw_target) :
 HW_Object(x, y, -1),
 _hw_target(hw_target) {
  buildQubitsAndInteractions(); 
}

void HW_Cell::buildQubitsAndInteractions() {

  //1) create all local qubits
  for (COORD i = 0; i < 8; ++i) {
    HW_Qubit* qubit = new HW_Qubit(_loc.getLocX(), _loc.getLocY(), i, this);
    _qubits.insert(std::make_pair(i, qubit));
  }

  //2) create all local interactions
  for (COORD i = 0; i < 4; ++i) {
    for (COORD j = 4; j < 8; ++j) {
      HW_Interaction* interaction = new HW_Interaction(
          _qubits[i]->_loc.getGlobalIndex(), _qubits[i]_->_loc.getGlobalIndex(), this);
      _interactions.insert(std::make_pair(std::make_pair(
              _qubits[i]->_loc.getGlobalIndex(),
              _qubits[j]->_loc.getGlobalIndex()), interaction));
    }
  }
}

HW_Qubit::HW_Qubit(const COORD x, const COORD y, const COORD local, HW_Cell* cell, double max_weight, double min_weight) :
  HW_Object(x, y, local),
  _cell(cell),
  _max_weight(max_weight),
  _min_weight(min_weight),
  _enable(true) {}



