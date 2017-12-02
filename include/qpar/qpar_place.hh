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

#include "qpar/qpar_matrix.hh"
#include "qpar/qpar_utils.hh"


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
class PlacementCost;


class QPlace {


public:
  /*! \brief default constructor for Place
   *  \param ParNetlist* netlist used in placement and routing
   *  \param ParTarget* hardware target to describe device
   */
  QPlace(ParNetlist* netlist, ParTarget* hw_target) :
   _netlist(netlist),
   _hw_target(hw_target),
   _annealer(NULL),
   _placement_cost(NULL),
   _current_total_cost(0.0) {
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

  void dumpCurrentPlacement(std::string filename = "current.place") const;

private:

  ParNetlist* _netlist; //<! netlist to specify input problem
  ParTarget* _hw_target; //<! hardware target

  /*! \brief initilize placement by random assign element to each grid
   */
  void initializePlacement();

  /*! \brief incremental update use matrix
   *  \param COORD from x coord
   *  \param COORD from y coord
   *  \param COORD to x coord
   *  \param COORD to x coord
   *  \return void
   */
  void updateUseMatrix(COORD from_x, COORD from_y, COORD to_x, COORD to_y);

  qpr_matrix<unsigned>* _used_matrix; //!< a matrix inidicate the number of used cell

  /*! \brief check all used matrix
   *  \return void
   */
  void usedMatrixSanityCheck();

  /*! \brief check if the incremental update the value correctly
   * use a brutal force method
   */
  void usedMatrixSanityCheck(unsigned x, unsigned y);

  /*! \brief check if the placer is ready to move
   */
  void checkIfReadyToMove();

  /*! \brief move a random element to a random place
   */
  bool tryMove();

  /*! \brief populate all the move information
   */
  void updateMove();

  /*! \brief commit the move action
   */
  void commitMove();

  /*! \brief restore to previous status
   */
  void restoreMove();

  /*! \brief compute the cost for a give placement
   *  \return double cost
   */
  double computeTotalCost();

  /*! \brief get initial annealing temperature based on random experiments
   */
  float getStartingT();

  void std::vector<ParElement*> _movable_elements; //!< a vector container to store all movable element

  void generateMove(ParElement* &element, COORD& x, COORD& y);

  /*! \brief given the source element and the destination coordinate collect all affected wires and elements
   */
  void findAffectedElementsAndWires(ParElement* element, COORD x, COORD y);

  double getStdDev(unsigned num, double ave, double s_square) const;

  RandomGenerator _random_gen; //!< a random nubmer generator

  Annealer* _annealer; //!< a annealer manager

  PlacementCost* _placement_cost; //!< placement cost 

  std::vector<ParElement*> _affected_elements; //!< a container to store the affected elemnts
  ParWireSet _affected_wires; //!< a container to store the affected wires

  double _current_total_cost; //!< to record the total cost

};



#endif

