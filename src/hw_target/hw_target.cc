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
#include "hw_target/hw_param.hh"


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


void HW_Target_Dwave::initializeTarget() {
  
  COORD x_num = _hw_param->getMaxRangeX();
  COORD y_num = _hw_param->getMaxRangeY();


  //1) initialize all qubits and interactions in cells
  for (COORD x = 0; x < x_num; ++x) {
    for (COORD y = 0; y < y_num; ++y) {
      HW_Cell* cell = new HW_Cell(x, y, this);
      _loc_to_cell.insert(std::make_pair(std::make_pair(x, y), cell));
    }
  }

  //2) initialize inter-cell interactions
  for (COORD x = 0; x < x_num; ++x) {
    for (COORD y = 0; y < y_num; ++y) {
      //2.1) build vertical cells
      if (y != (y_num - 1))
        buildInterCellInteractions(x, y, x, y + 1, true);
      //2.2) build horizontal cells
      if (x != (x_num - 1))
        buildInterCellInteractions(x, y, x + 1, y, false);
    }
  }

}

void HW_Target_Dwave::buildInterCellInteractions(COORD x1, COORD y1, COORD x2, COORD y2, bool vertical) {
  if (vertical) {
    for (COORD local = 0; local < 4; ++local) {
      COORD qbit_index1 = HW_Loc::toGlobalIndex(x1,y1,local);
      COORD qbit_index2 = HW_Loc::toGlobalIndex(x2,y2,local);
      HW_Qubit* qubit1 = _loc_to_qubit.at(qbit_index1);
      HW_Qubit* qubit2 = _loc_to_qubit.at(qbit_index2);
      HW_Interaction* interac = new HW_Interaction(qubit1,qubit2,NULL);
      addInteraction(qbit_index1,qbit_index2,interac);
    }
  } else {
    for (COORD local = 4; local < 8; ++local) {
      COORD qbit_index1 = HW_Loc::toGlobalIndex(x1,y1,local);
      COORD qbit_index2 = HW_Loc::toGlobalIndex(x2,y2,local);
      HW_Qubit* qubit1 = _loc_to_qubit.at(qbit_index1);
      HW_Qubit* qubit2 = _loc_to_qubit.at(qbit_index2);
      HW_Interaction* interac = new HW_Interaction(qubit1,qubit2,NULL);
      addInteraction(qbit_index1,qbit_index2,interac);
    }
  }
}



