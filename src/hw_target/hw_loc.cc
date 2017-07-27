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

#include "hw_target/hw_loc.hh"
#include "hw_target/hw_param.hh"


#include <cassert>
#include <cmath>

HW_Loc::HW_Loc(COORD cell_x, COORD cell_y, COORD local_index) :
  _cell_x(cell_x),
  _cell_y(cell_y),
  _local_index(local_index)
{
  _interaction_index = std::make_pair(-1, -1);

  if (local_index == -1)
    _global_index = -1;
  else
    _global_index = toGlobalIndex(cell_x, cell_y, local_index);
}


HW_Loc::HW_Loc(COORD qubit_a, COORD qubit_b)
{

  assert(qubit_a != qubit_b);
  COORD min_index = std::min(qubit_a, qubit_b);
  _cell_x = globalIndexToX(min_index);
  _cell_y = globalIndexToY(min_index);
  _local_index = -1;
  _global_index = -1;

  _interaction_index = std::make_pair(
      std::min(qubit_a, qubit_b),
      std::max(qubit_a, qubit_b));
}


bool HW_Loc::isQubit() const { 
  return (_global_index > -1) &&
         (_cell_x > -1) &&
         (_cell_y > -1) &&
         (_local_index > -1) &&
         (_interaction_index.first == -1) &&
         (_interaction_index.second == -1);
}

bool HW_Loc::isInteraction() const { 
  return (_global_index == -1) &&
         (_cell_x > -1) &&
         (_cell_y > -1) &&
         (_local_index == -1) &&
         (_interaction_index.first > -1) &&
         (_interaction_index.second > -1);
}

bool HW_Loc::isCell() const {
  return (_global_index == -1) &&
         (_cell_x > -1) &&
         (_cell_y > -1) &&
         (_local_index == -1) &&
         (_interaction_index.first == -1) &&
         (_interaction_index.second == -1);
}


COORD HW_Loc::toGlobalIndex(COORD x, COORD y, COORD local) {
  HW_Param* hw_param = HW_Param::getOrCreate();
  COORD max_x = hw_param->getMaxRangeX();
  COORD max_y = hw_param->getMaxRangeY();
  COORD max_local_index = hw_param->getMaxRangeLocal();

  //sanity check
  assert(max_x != -1);
  assert(max_y != -1);
  assert(max_local_index != -1);

  COORD cell_index = y * max_x + x;
  COORD global_index = cell_index * max_local_index + local;

  return global_index;
}

COORD globalIndexToX(COORD global_index) {
  HW_Param* hw_param = HW_Param::getOrCreate();
  COORD max_local_index = hw_param->getMaxRangeLocal();
  COORD max_x = hw_param->getMaxRangeX();
  return (global_index / max_local_index) % max_x;
}

COORD globalIndexToY(COORD global_index) {
  HW_Param* hw_param = HW_Param::getOrCreate();
  COORD max_local_index = hw_param->getMaxRangeLocal();
  COORD max_x = hw_param->getMaxRangeX();
  return (global_index / max_local_index) / max_x;
}

COORD globalIndexToLocalIndex(COORD global_index) {
  HW_Param* hw_param = HW_Param::getOrCreate();
  COORD max_local_index = hw_param->getMaxRangeLocal();
  return (global_index % max_local_index);
}
 

