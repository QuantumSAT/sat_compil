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

#ifndef TCL_COMMAND_HH
#define TCL_COMMAND_HH

/*!
 * \file tcl_command.hh
 * \author Juexiao Su
 * \date 14 Jul 2017
 * \brief tcl command
 */


#include <string>
#include <vector>

class TclManager;




/*! \brief QTclCommand is the interface class all all tcl command
 *
 * It provies a simple API to create customize tcl command
 */
class QTclCommand {


  /*! \brief It stores the argument and its type flag
   */
  struct CommandOption {
    std::string argument;
    std::string flag;
  };

public:
  /* \brief constructor
   * \param command_name tcl command name
   * \param syntax tcl command syntax
   */
  QTclCommand(const std::string& command_name, const std::string& syntax);


  virtual ~QTclCommand() {}

  /*  \brief execute tcl command
   *  \param argc argument count
   *  \param argv argument vector
   *  \param result returned result
   *  \return int
   */
  virtual int execute(int argc, char** argv, std::string& result) = 0;

  /*  \brief return a detailed help message
   *  \return std::string help message
   */
  virtual std::string help() const = 0;

  friend TclManager;

private:

  bool checkOptions(const int argc, const char** argv);

  bool parseSyntax(const std::string& syntax);

  std::string _command_name;    //!< command name 
  std::string _syntax;          //!< command syntax

  std::vector<CommandOption> _options   //!< vector to hold all command options


};

#define TCL_COMMAND_DEFINE(CMD)                   \
  class CMD : public QTclCommand {                \
    typedef QTclCommand SUPER;                    \
  public:                                         \
    CMD(const std::string& command_name, const std::string& syntax) : SUPER(command_name, syntax) {}  \
  protected:                                      \
    virtual ~CMD() {}                             \
  public:                                         \
    virtual int execute(int argc, char** argv, std::string& result);  \
    virtual std::string  help() const;                    \
  };            


#endif

