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

/*!
 * \file tcl_mannager.cc
 * \author Juexiao Su
 * \date 14 Jul 2017
 * \brief tcl utility to manage all tcl commands
 */

#include "tcl/tcl_manager.hh"
#include "tcl/tcl_command.hh"
#include "utils/qlog.hh"

#include <cstring>

inline int execute_cmd(ClientData* clientData, Tcl_Interp* interp, int argc, const char** argv) {
  std::string cmd_name = std::string(argv[0]);
  return TclManager::getOrCreate()->execute_cmd_main(cmd_name, clientData, interp, argc, argv);

}

TclManager* TclManager::_tcl_manager = NULL;


TclManager* TclManager::getOrCreate() {
  if (_tcl_manager == NULL)
    _tcl_manager = new TclManager;
  return _tcl_manager;
}

int TclManager::execute_cmd_main(std::string cmd, ClientData* clientData, Tcl_Interp*, const int argc, const char** argv) {

  QTclCommand* command = _tcl_manager->getCommand(cmd);
  if (!command) {
    qlog.speak("TCL","Cannot find tcl command %s", cmd.c_str());
    return TCL_OK;
  }

  if (!command->checkOptions(argc, argv)) {
    return TCL_OK;
  }

  std::string tcl_result;

  int status = TCL_OK;
  status = command->execute(argc, argv, tcl_result, clientData);
  char* res = strdup(tcl_result.c_str());
  Tcl_SetResult(_tcl_manager->_interp, res, TCL_VOLATILE);
  free(res);

  return status;
  

}




