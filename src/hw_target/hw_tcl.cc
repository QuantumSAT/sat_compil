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

#include <cassert>
#include "hw_target/hw_tcl.hh"
#include "hw_target/hw_param.hh"
#include "hw_target/hw_target.hh"
#include "utils/qlog.hh"


std::string QCOMMAND_init_target::help() const {
  const std::string msg = "init_target -row <int> -col <int> -local <int>";
  return msg;
}

int QCOMMAND_init_target::execute(int argc, const char** argv, std::string& result, ClientData clientData) {
  TclManager* tcl_manager = static_cast<TclManager*>(clientData);
  Tcl_Interp* interp = tcl_manager->getInterp();

  if (!checkOptions(argc, argv)) {
    printHelp();
    return TCL_OK;
  }

  int row_num = -1;
  int col_num = -1;
  int local_num = -1;

  if (!getIntOption(argc, argv, "-row", row_num)) {
    printHelp();
    return TCL_OK;
  }

  if (!getIntOption(argc, argv, "-col", col_num)) {
    printHelp();
    return TCL_OK;
  }

  if (!getIntOption(argc, argv, "-local", local_num)) {
    printHelp();
    return TCL_OK;
  }


  HW_Param* hw_param = HW_Param::getOrCreate();
  hw_param->setMaxRangeX((unsigned)col_num);
  hw_param->setMaxRangeY((unsigned)row_num);
  hw_param->setMaxRangeLocal((unsigned)local_num);

  HW_Loc::setHWParam(hw_param);

  HW_Target_Dwave* target = new HW_Target_Dwave(hw_param);
  qlog.speak("TCL", "Initializing hardware %d...");
  target->initializeTarget();
  HW_Target_Dwave::setHwTarget(target);

}

