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
#include <cstring>

#include "utils/qlog.hh"
#include "tcl/tcl_command.hh"




QTclCommand::QTclCommand(const std::string& command_name, const std::string& syntax) :
  _command_name(command_name),
  _syntax(syntax) {
    splitSyntax(_syntax, _options);
}

void QTclCommand::printHelp() {
  qlog.speak("\tcmd_name : %s\t\tsyntax: %s\n", _command_name, _syntax);
}

bool QTclCommand::isSeperator(const std::string& name) const {
  return name[0] == '-' && name.length()>1 && !isdigit(name[1]);
}

std::string QTclCommand::getCommandString(const int argc, const char** argv) {
  std::string result;
  for (int i = 0; i < argc; ++i) {
    std::string cmd_name = std::string(argv[i]);

    if (i != 0) cmd_name = " " + cmd_name;
    result += cmd_name;
  }

  return result;
}

int QTclCommand::getOptionIndex(const int argc, const char** argv, const char* option_name) {
  for (int i = 1; i < argc; ++i) {
    if (std::strcmp(option_name, argv[i]) == 0)
      return i;
  }
  return -1;
}

bool QTclCommand::isOptionExist(const int argc, const char** argv, const char* option_name) {
  if (getOptionIndex(argc, argv, option_name) > -1)
    return true;
  else 
    return false;
}

void QTclCommand::splitSyntax(const std::string& str, std::vector<CommandOption>& options, std::string sep) {

  std::vector<std::string> opts;
  boost::split(opts, str, boost::is_any_of(sep), boost::token_compress_on);
  size_t opts_num = options.size();
  for (size_t i = 0; i < opts_num; ++i) {
    while (i < opts_num && !isSeperator(opts[i])) ++i;

    if (i == opts_num) break;

    CommandOption cmd_opt;
    cmd_opt.argument = opts[i];

    std::string flag;
    while(++i < opts_num) {
      if (isSeperator(opts[i])) {
        --i;
        break;
      }
      flag += " ";
      flag += opts[i];
    }
    cmd_opt.flag = flag;
    options.push_back(cmd_opt);
  }
}





bool QTclCommand::checkOptions(const int argc, const char** argv) {

  return false;
}




