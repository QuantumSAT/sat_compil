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

#include "cnf2blif/cnf_tcl.hh"
#include "cnf2blif/cnf_to_blif.hh"

#include "utils/qlog.hh"

std::string QCOMMAND_cnf2blif::help() const {
  const std::string msg = "cnf2blif <cnf> <blif>";
  return msg;
}

int QCOMMAND_cnf2blif::execute(int argc, const char** argv, std::string& result, ClientData clientData) {
  result = "OK";

  if (!checkOptions(argc, argv)) {
    printHelp();
    return TCL_OK;
  }

  std::string cnfname = argv[1];
  std::string blifname = argv[2];
  std::fstream infile;
  infile.open(cnfname.c_str(), std::ios::in);

  if (!infile.is_open()) {
    qlog.speak("CNF2BLIF", "Cannot open cnf file %s", cnfname.c_str());
    return TCL_OK;
  }

  qlog.speak("CNF2BLIF", "Convert cnf %s to %s blif",
      cnfname.c_str(), blifname.c_str());

  SatReaderWriter file_work;
  file_work.cnf2blif(infile, blifname);
  infile.close();
  return TCL_OK;
}
