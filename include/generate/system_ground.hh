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

#ifndef SYSTEM_GROUND_HH
#define SYSTME_GROUND_HH

#include "generate/system_gen.hh"
#include "utils/qlog.hh"


double calculateEnergy(const QubitConfigs& h, const InteractionConfigs& J, const QubitState& state) {

  double energy = 0.0;
  QubitConfigs::const_iterator q_iter = h.begin();
  for (; q_iter != h.end(); ++q_iter) {
    QASSERT(state.count(q_iter->first));
    energy += q_iter->second.value * state.at(q_iter->first);
  }

  InteractionConfigs::const_iterator i_iter = J.begin();
  for (; i_iter != J.end(); ++i_iter) {
    QASSERT(state.count(i_iter->first.first));
    QASSERT(state.count(i_iter->first.second));

    energy += i_iter->second.value * state.at(i_iter->first.first) * state.at(i_iter->first.second);
  }

  return energy;
}




#endif



