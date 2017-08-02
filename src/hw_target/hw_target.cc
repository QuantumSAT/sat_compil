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


HW_Target_abstract::~HW_Target_abstract() {
  for (Q_ITER qiter = _loc_to_qubit.begin(); 
        qiter != _loc_to_qubit.end(); ++qiter) {
    delete qiter->second;
  }

  _loc_to_qubit.clear();

  for (I_ITER iter = _loc_to_interaction.begin();
        iter != _loc_to_interaction.end(); ++iter)
    delete iter->second;

  _loc_to_interaction.clear();

}
