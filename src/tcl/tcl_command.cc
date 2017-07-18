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
 * \file tcl_command.cc
 * \author Juexiao Su
 * \date 14 Jul 2017
 * \brief tcl command
 */


#include <boost/algorithm/string.hpp>
#include "utils/qlog.hh"
#include "tcl/tcl_command.hh"




QTclCommand::QTclCommand(const std::string& command_name, const std::string& syntax) :
  _command_name(command_name),
  _syntax(syntax) {
    parseSyntax(syntax);
}


bool QTclCommand::checkOptions(const int argc, const char** argv) {


  QASSERT(0);
  return false;

}

bool QTclCommand::parseSyntax(const std::string& syntax) {

}

