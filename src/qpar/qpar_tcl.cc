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

#include <cstdio>

#include "qpar/qpar_tcl.hh"
#include "qpar/qpar_netlist.hh"
#include "syn/blif.h"
#include "utils/qlog.hh"

namespace SYN {
  extern Model* myTop;
}

std::string QCOMMAND_build_qpar_nl::help() const {
  const std::string msg = "build_qpar_nl <filename>";
  return msg;
}

int QCOMMAND_build_qpar_nl::execute(int argc, const char** argv, std::string& result, ClientData clientData) {
  //TclManager* tcl_manager = static_cast<TclManager*>(clientData);
  //Tcl_Interp* interp = tcl_manager->getInterp();

  result = "OK";
  if (!checkOptions(argc, argv)) {
    printHelp();
    return TCL_OK;
  }

  FILE* fp = fopen(argv[1], "r");
  if (!fp) {
    qlog.speakError("QPAR: Cannot open %s to read",
        argv[1]);
  }

  SYN::BlifReaderWriter reader;
  reader.readDesign(fp);
  fclose(fp);
  ParNetlist netlist(SYN::myTop);

  return TCL_OK;
}

