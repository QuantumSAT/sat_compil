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
 * \file tcl_basic_command.cc
 * \author Juexiao Su
 * \date 21 Jul 2017
 * \brief basic tcl command
 */

#include "tcl/tcl_command.hh"
#include "tcl/tcl_manager.hh"
#include "tcl/tcl_basic_command.hh"

#include "utils/qlog.hh"

#include <string>


std::string QCOMMAND_source::help() const {
  const std::string msg = "source <tcl_file>";
  return msg;
}

int QCOMMAND_source::execute(int argc, const char** argv, std::string &result, ClientData clientData) {
  result = "";
  TclManager* tcl_manager = static_cast<TclManager*>(clientData);
  Tcl_Interp* interp = tcl_manager->getInterp();

  if (argc != 2) {
    printHelp();
    return TCL_OK;
  }

  qlog.speak("TCL", "Rource %s ...", argv[1]);

  if (Tcl_EvalFile(interp, argv[1]) == TCL_ERROR) {
    const char* error_msg = Tcl_GetVar(interp, "errorInfo", 0);
    qlog.speakError("TCL_ERROR: %s", error_msg);
  }

  return TCL_OK;
}
std::string QCOMMAND_hello::help() const {
  const std::string msg = "syntax: help";
  return msg;
}

int QCOMMAND_hello::execute(int argc, const char** argv, std::string& result, ClientData clientData) {
  qlog.speak("TCL", "Hello World!");
  return TCL_OK;
}










