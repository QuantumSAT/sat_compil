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

#ifndef QPAR_ROUTING_TEST_HH
#define QPAR_ROUTING_TEST_HH

/*! \file qpar_routing_test.hh
 *  \author Juexiao SU
 *  \date 18 Dec 2017
 *  \brief a testing/debug facility to check basic function in routing
 */



class ParSystem;

class RoutingTester {

public:
  RoutingTester(ParSystem* par_system) :
    _par_system(par_system) 
  {
  }

  /*! \brief run basic routing graph test
   */
  void testRoutingGraph();

private:

  ParSystem* _par_system; //!< placement and routing system


};

#endif

