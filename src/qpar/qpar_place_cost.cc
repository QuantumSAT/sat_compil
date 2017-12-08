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

#include "qpar/qpar_place_cost.hh"
#include "qpar/qpar_netlist.hh"

#include "utils/qlog.hh"

#include <cmath>
#include <cassert>

//data from ICCAD 1994 960-965 by Eric Cheng
const float PlacementCost::cross_count[50] = {  
  1.0000f, 1.0000f, 1.0000f, 1.0828f, 1.1536f, 
  1.2206f, 1.2823f, 1.3385f, 1.3991f, 1.4493f,
  1.4974f, 1.5455f, 1.5937f, 1.6418f, 1.6899f, 
  1.7304f, 1.7709f, 1.8114f, 1.8519f, 1.8924f,
  1.9288f, 1.9652f, 2.0015f, 2.0379f, 2.0743f, 
  2.1061f, 2.1379f, 2.1698f, 2.2016f, 2.2334f,
  2.2646f, 2.2958f, 2.3271f, 2.3583f, 2.3895f, 
  2.4187f, 2.4479f, 2.4772f, 2.5064f, 2.5356f,
  2.5610f, 2.5864f, 2.6117f, 2.6371f, 2.6625f, 
  2.6887f, 2.7148f, 2.7410f, 2.7671f, 2.7933f
};




double CongestionAwareCost::computeCost(ParWire* wire, const qpr_matrix<unsigned>& used_matrix) {

  //this is a model wire that connects only to the top module port
  if (wire->getElementNumber() <= 1) return 0.0;

  BoundingBox current_bb = wire->getCurrentBoundingBox();
  Box bbox = current_bb.getBoundBox();
  Box ebox = current_bb.getEdgeBox();

  unsigned width = bbox.xr() - bbox.xl() + 1;
  unsigned hight = bbox.yb() - bbox.yt() + 1;

  unsigned number_of_cell = width * hight;

  unsigned xl = bbox.xl();
  unsigned yt = bbox.yt();

  unsigned xr = bbox.xr();
  unsigned yb = bbox.yb();

  unsigned used_cell = used_matrix.cell(xr, yb) +
                       ((xl == 0 || yt == 0) ? 0 : used_matrix.cell(xl - 1, yt - 1)) -
                       ((xl == 0) ? 0 : used_matrix.cell(xl - 1, yb)) -
                       ((yt == 0) ? 0 : used_matrix.cell(xr, yt - 1));

  double fill_rate = double(used_cell)/double(number_of_cell);
  QASSERT(fill_rate <= 1.0);

  fill_rate = std::pow(fill_rate, 1.3);
  unsigned number_of_ele = wire->getElementNumber();

  if (number_of_ele <= 49)
    fill_rate = fill_rate * PlacementCost::cross_count[number_of_ele];
  else
    fill_rate = fill_rate * static_cast<float>(2.7933 + 0.02616 * (number_of_ele - 50));

  double cost = fill_rate * (width + hight);

  return cost;

}
