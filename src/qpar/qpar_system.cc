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


/*! \file qpar_system.cc
 *  \author Juexiao Su
 *  \date 07 Nov 2017
 *  \brief this is the main class to handle all placement and routing behavior
 */


#include "qpar/qpar_system.hh"
#include "qpar/qpar_target.hh"
#include "qpar/qpar_place.hh"
#include "utils/qlog.hh"

ParSystem::~ParSystem() {

  if (_par_netlist) delete _par_netlist;
  if (_par_target) delete _par_target;
  if (_rand_gen) delete _rand_gen;

  _par_netlist = NULL;
  _par_target = NULL;
  _rand_gen = NULL;
}


void ParSystem::initSystem() {
  qlog.speak("Initialize", "Build hardware target");
  initHardware();
  _status.hasTargetInit = true;

  qlog.speak("Initialize", "Build netlist");
  initNetlist();
  _status.hasDesignInit = true;
}

void ParSystem::initNetlist() {
  QASSERT(_par_netlist);
  _par_netlist = new ParNetlist(_syn_netlist);
}

void ParSystem::initHardware() {
  QASSERT(_par_target == NULL);
  _par_target = new ParTarget(_hw_target);
  _par_target->initParTarget();
}

void ParSystem::doPlacement() {

  //check system status
  if (_status.hasTargetInit && _status.hasDesignInit) {
    QPlace placer(_par_netlist, _par_target);
    placer.run();
  } else {
    qlog.speakError("Cannot run placement because target or design has not been initilized");
  }

}



