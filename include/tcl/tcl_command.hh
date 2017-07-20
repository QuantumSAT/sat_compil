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

    bool matchFlag(std::string str);
  };

public:
  /*! \brief constructor
   *  \param command_name tcl command name
   *  \param syntax tcl command syntax
   */
  QTclCommand(const std::string& command_name, const std::string& syntax);


  virtual ~QTclCommand() {}

  /*! \brief execute tcl command
   *  \param argc argument count
   *  \param argv argument vector
   *  \param result returned result
   *  \return int
   */
  virtual int execute(int argc, char** argv, std::string& result) = 0;


  /*! \brief print tcl command help infomation
   */
  void printHelp();

  /*! \brief check if the given string can be cast to int
   *  \param given option name
   *  \return bool
   */
  static bool isValidInt(std::string opt);

  /*! \brief check if the given string can be cast to int
   *  \param given option name
   *  \return bool
   */
  static bool isValidDouble(std::string opt);

  /*! \brief check if the given string can be cast to int
   *  \param given option name
   *  \return bool
   */
  static bool isValidString(std::string opt);


  friend TclManager;

private:

  /*! \brief return a detailed help message
   *  \return std::string help message
   */
  virtual std::string help() const = 0;

  /*! \brief convert arguments to string
   *  \param argc argument count
   *  \param argv argument vector
   *  \return string
   */
  std::string getCommandString(const int argc, const char** argv);


  /*! \brief get argument index
   *  \param argc argument count
   *  \param argv argument vector
   *  \return int argument index
   */
  int getOptionIndex(const int argc, const char** argv, const char* option_name);


  /*! \brief check if certain argument exists
   *  \param argc argument count
   *  \param argv argument vector
   *  \param option_name the name of argument needs to check
   *  \return bool indicate weather the argument exists
   */
  bool isOptionExist(const int argc, const char** argv, const char* option_name);



  /*! \brief check if the input command compatible with syntax
   *  \param argc argument count
   *  \param argv argument vector
   *  \return bool
   */
  bool checkOptions(const int argc, const char** argv);

  /*! \brief split syntax into options
   *  \param str string needs to parse
   *  \param options resulted options
   *  \param sep seperator
   */
  void splitSyntax(const std::string& str, std::vector<CommandOption>& options, std::string sep = " ");

  /*! \brief parse syntax into internal data structure
   *  \param syntax syntax for tcl command
   *  \param bool success
   */
  bool parseSyntax(const std::string& syntax);

  /*! \brief check if string start with seperator "-"
   *  \param given command string
   *  \return bool  
   */
  bool isSeperator(const std::string& name) const;

  std::string _command_name;            //!< command name 
  std::string _syntax;                  //!< command syntax
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

