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
#ifndef QPAR_SYSTEM_HH
#define QPAR_SYSTME_HH


#include "qpar/qpar_netlist.hh"
#include "qpar/qpar_utils.hh"

namespace SYN {
  class Model;
};

class ParNetlist;
class ParTarget;

/*! \brief a status struct to inidcate the Par status
 */
struct ParStatus {
  bool hasTargetInit;
  bool hasDesignInit;
  bool hasPlaced;
  bool hasRouted;

  ParStatus() :
    hasPlaced(false),
    hasRouted(false),
    hasTargetInit(false),
    hasDesignInit(false) {}
};

/*! \brief modulize each functionality to avoid a huge class 
 */
class ParSystem {

public:
  /*! \brief default constructor
   */
  ParSystem(SYN::Model* syn_netlist, HW_Target_Dwave* hw_target) :
    _syn_netlist(syn_netlist),
    _hw_target(hw_target),
    _par_netlist(NULL),
    _par_target(NULL),
    _rand_gen(NULL) {}

  /*! \brief default destructor
   */
  ~ParSystem();

  /*! \brief initialize placement routing data structure
   *  \return void
   */
  void initSystem();


  /*! \brief initialize placement target based on hardware target
   *  \return void
   */
  void initHardware();

  /*! \brief initialize placement and routing netlist
   */
  void initNetlist();

  /*! \brief perform constraints placement
   *  \return void
   */
  void doPlacement();

  /*! \brief perform chain routing
   *  \return void
   */
  void doRoute();

  /* \brief perform configuration generation
   */
  void doGenerate();

private:
 
  Model* _syn_netlist; //<! netlist from synthesis tool
  HW_Target_Dwave* _hw_target; //<! hardware target

  ParNetlist* _par_netlist;
  ParTarget* _par_target;

  ParStatus _status;
  RandomGenerator* _rand_gen;


};




#endif
