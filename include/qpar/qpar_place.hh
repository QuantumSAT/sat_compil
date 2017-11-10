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

#ifndef QPAR_PLACE_HH
#define QPAR_PLACE_HH

#include "qpar_matrix.hh"


/*!
 * \file qpar_place.hh
 * \author Juexiao Su
 * \date 22 Oct 2017
 * \brief class to handle all place related functions
 */

namespace SYN {
  class Model;
}

class Grid;
class ParNetlist;
class ParTarget;


class QPlace {


public:
  /*! \brief default constructor for Place
   *  \param ParNetlist* netlist used in placement and routing
   *  \param ParTarget* hardware target to describe device
   */
  QPlace(ParNetlist* netlist, ParTarget* hw_target) :
   _netlist(netlist),
   _hw_target(hw_target) {
  }

  ~QPlace();

  /*! \brief initialize placement related data structure
   *! \return void

   build placement bin, coordinate, algorithm data structure
   */
  void buildPlacementData();

  /*! \brief execute placement
   */
  void run();

private:

  ParNetlist* _netlist; //<! netlist to specify input problem
  ParTarget* _hw_target; //<! hardware target

  /*! \brief initilize placement by random assign element to each grid
   */
  void initilizePlacement();

  qpr_matrix<unsigned>* _used_matrix; //!< a matrix inidicate the number of used cell

};



#endif

