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

#ifndef QPAR_PLACE_COST_HH
#define QPAR_PLACE_COST_HH


/*! \file qpar_place_cost.hh
 *  \author Juexiao Su
 *  \date 28 Nov 2017
 *  \brief placement cost function
 */

#include "qpar/qpar_matrix.hh"

class ParWire;


class PlacementCost {
public:
  static const float cross_count[50];
  virtual double computeCost(ParWire* wire, const qpr_matrix<unsigned>& used_matrix) = 0;
};

class BoundingBoxCost : public PlacementCost {
public:
  virtual double computeCost(ParWire* wire, const qpr_matrix<unsigned>& used_matrix);
};

class CongestionAwareCost : public PlacementCost {
public:
  virtual double computeCost(ParWire* wire, const qpr_matrix<unsigned>& used_matrix);
};



#endif


