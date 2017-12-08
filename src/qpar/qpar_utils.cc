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

#include "qpar/qpar_utils.hh"

#include <cmath>

/*!
 * \file qpar_utils.cc
 * \author Juexiao Su
 * \date 07 Nov 2017
 * \brief a collection of utilities used in qpar
 */


unsigned RandomGenerator::uRand(unsigned min, unsigned max) {
  std::uniform_int_distribution<unsigned>gen(min, max);
  return gen(_gen);
}

int RandomGenerator::iRand(int min, int max) {
  std::uniform_int_distribution<int>gen(min, max);
  return gen(_gen);
}

float RandomGenerator::fRand(float min, float max) {
  std::uniform_real_distribution<float>gen(min, max);
  return gen(_gen);
}


void Annealer::updateT(float sr) {
  if (sr > annealer_step_1)
    _current_t *= annealer_cool_speed1;
  else if (sr > annealer_step_2)
    _current_t *= annealer_cool_speed2;
  else if (sr > annealer_step_3 || _radius_limit > 1.f)
    _current_t *= annealer_cool_speed3;
  else
    _current_t *= annealer_cool_speed4;
}

void Annealer::updateMoveRadius(float sr) {
  _radius_limit *= (1.0f - annealer_best_rate + sr);
  _radius_limit = std::min(_radius_limit, _radius_max);
  _radius_limit = std::max(_radius_limit, 1.0f);
}


bool Annealer::shouldAccept(float delta_c) {
  if (delta_c <= 0)
    return true ;

  if (_current_t <= 0.0f)
    return false;

  bool accept = true;

  accept = _rand->fRand(0.0, 1.0) < std::exp(-_boltzmann*delta_c/_current_t);
  return accept;

}

bool Annealer::shouldExit(float unit_cost) {
  return (_current_t < 0.08 * unit_cost);
}

bool Box::isInBox(int x, int y) const {

  return (x >= xl_ &&
          x <= xr_ &&
          y >= yt_ &&
          y <= yb_);

}

