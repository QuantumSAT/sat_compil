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

#ifndef PLACE_HH
#define PLACE_HH


/*!
 * \file place.hh
 * \author Juexiao Su
 * \date 22 Oct 2017
 * \brief class to handle all place related functions
 */

namespace SYN {
  class Model;
}

class Grid;
class ParNetlist;


class Place {


public:
  /*! \brief default constructor for Place
   *  \param ParNetlist* netlist used in placement and routing
   *  \param HW_Target_Dwave* hardware target to describe device
   */
  Place::Place(ParNetlist* netlist, HW_Target_Dwave* hw_target) {

  }

  /*! \brief initialize placement related data structure
   *! \return void
   */
  void buildPlacementData();

private:

  ParNetlist* _netlist; //<! netlist to specify input problem
  HW_Target_Dwave* _hw_target; //<! hardware target



};



#endif

