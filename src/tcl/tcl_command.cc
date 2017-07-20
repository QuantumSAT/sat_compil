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

bool CommandOption::matchFlag(std::string str) {
  std::string cmd = boost::trim(str);
  std::string regex = boost::trim(flag);

  if (regex.length() == 0) {
    if (cmd.length() == 0)
      return true;
    else
      return false;
  }

  std::vector<std::string> str_v;
  boost::split(str_v, str, boost::is_any_of(" "), boost::token_compress_on);





}




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

bool QTclCommand::isValidInt(std::string opt) {
  size_t len = opt.length();
  if (!len) return false;

  if (opt[0] == "-") ++i;
  for (; i < len; ++i) {
    if (!std::isdigit(opt[i]))  
      return false;
  }
  return true;
}

bool QTclCommand::isValidDouble(std::string opt) {
  size_t len = opt.length();
  if (!len) return false;

  if (opt[0] == "-") ++i;
  for (; i < len; ++i) {
    if (!std::isdigit(opt[i]) && (opt[i] != '.'))  
      return false;
  }
  return true;
}

bool QTclCommand::isValidString(std::string opt) {
  size_t len = opt.length();
  if (!len) return false;

  for (size_t i = 0; i < len; ++i) {
    if (opt[i] == '<' || opt[i] == '>')
      return false;
  }
  return true;
}


bool QTclCommand::checkOptions(const int argc, const char** argv) {
  std::string cmd_name = std::string(argv[0]);
  
  for (int i = 1; i < argc; ++i) {
    std::string opt_name = std::string(argv[i]);
    if (!isSeperator(opt_name)) continue;

    std::string argu = "";
    while (++i < objc) {
      std::string para_name(argv[i]);
      if (isSeperator(para_name)) {
        --i;
        break;
      }
      argu += " ";
      argu += para_name;
    }

    
    std::vector<CommandOption>::iterator o_iter = _options.begin();
    for (; o_iter != _options.end(); ++o_iter) {
      if (o_iter->argument == opt_name) {
        if (o_iter->matchFlag(argu))
          break;
      } else {
        qlog.speak("Tcl", "Invalid option arguments %s", argu.c_str());
        printHelp();
        return false;
      }
    }
    if (o_iter == _options.end()) {
      qlog.speak("Tcl", "Invalid option name: %s", opt_name.c_str());
      return false;
    }
  }
  return true;
}




