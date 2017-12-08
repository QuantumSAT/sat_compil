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

#include "hw_target/hw_target.hh"
#include "hw_target/hw_object.hh"
#include "utils/qlog.hh"
#include "qpar/qpar_target.hh"

#include <algorithm>
#include <random>

unsigned ParGrid::_grid_index_counter = 0;

void ParGrid::setParElement(ParElement* element) {
  _element.setStatus(element);
}

void ParGrid::save() {
  _element.saveStatus();
}

void ParGrid::restore() {
  _element.restoreStatus();
}

ParElement* ParGrid::getCurrentElement() const {
  return _element.getStatus();
}

HW_Loc ParGrid::getLoc() const {
  return _cell->getLoc();
}

void ParGridContainer::shuffle() {
  std::mt19937 gen(0);
  std::shuffle(SUPER::begin(), SUPER::end(), gen);
}

void ParGridContainer::sortId() {
  GridCmp cmp;
  std::sort(SUPER::begin(), SUPER::end(), cmp);
}

ParTarget::~ParTarget() {
  for (size_t i = 0; i < _grid_vector.size(); ++i)
    delete _grid_vector[i];

  _grid_vector.clear();
  _grids.clear();

}

void ParTarget::initParTarget() {
  _maxX = _hw_target->getXLimit();
  _maxY = _hw_target->getYLimit();

  HW_Target_abstract::C_ITER c_iter = _hw_target->cell_begin();
  for (; c_iter != _hw_target->cell_end(); ++c_iter) {
    HW_Cell* cell = c_iter->second;
    ParGrid* grid = new ParGrid(cell);
    _grid_vector.push_back(grid);
    _grids.insert(std::make_pair(std::make_pair(c_iter->first.first, c_iter->first.second),
          grid));
    grid->setParElement(NULL);
    grid->save();
  }
  qlog.speak("ParTarget", "%u cells has been constructed", (unsigned)_grid_vector.size());

}


