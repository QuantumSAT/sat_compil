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
#include <cassert>

inline int execute_cmd(ClientData clientData, Tcl_Interp* interp, int argc, const char** argv) {
  std::string cmd_name = std::string(argv[0]);
  return TclManager::getOrCreate()->execute_cmd_main(cmd_name, clientData, interp, argc, argv);

}

/*! \brief Tcl manager clean up function
 */
void TclManagerCleanUp() {
  delete TclManager::_tcl_manager;
  TclManager::_tcl_manager = NULL;
}


extern int TclInitProc(Tcl_Interp* interp);
extern void registerAllCommands();


void TclManager::init(int argc, char** argv) {
  //1) create tcl manager;
  assert(getOrCreate());

  //2) create tcl interp
  Tcl_Interp* interp = Tcl_CreateInterp();
  getOrCreate()->setTclInterp(interp);

  //3) register all commands
  registerAllCommands();

  if (argc == 2) {
    if (Tcl_EvalFile(interp, argv[1]) == TCL_ERROR) {
      const char* error_msg = Tcl_GetVar(interp, "errorInfo", 0);
      qlog.speakError("%s",error_msg);
    }
  }
  else {
    qlog.speak("Usage", "interactive mode is not supported!");
    qlog.speak("Usage", "qsat <tcl>");
  }

}

TclManager* TclManager::_tcl_manager = NULL;


TclManager* TclManager::getOrCreate() {
  if (_tcl_manager == NULL)
    _tcl_manager = new TclManager;
  return _tcl_manager;
}

void TclManager::registerCommand(QTclCommand* cmd) {
  assert(cmd);

  if (_name_to_command.count(cmd->_command_name)) {
    qlog.speakError("Redefine of Tcl command %s", cmd->_command_name.c_str());
  }

  _name_to_command[cmd->_command_name] = cmd;

  Tcl_CreateCommand(_interp, cmd->_command_name.c_str(), execute_cmd, this, NULL);
}

int TclManager::execute_cmd_main(std::string cmd, ClientData clientData, Tcl_Interp*, const int argc, const char** argv) {

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




