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

#ifndef TCL_MANAGER_HH
#define TCL_MANAGER_HH

#include <vector>
#include <map>
#include <string>

#include <tcl.h>

/*!
 * \file tcl_mannager.hh
 * \author Juexiao Su
 * \date 11 Jul 2017
 * \brief tcl utility to manage all tcl commands
 */

class QTclCommand;

extern void TclManagerCleanUp();


/*! \class TclManager
 *  \brief This class manages all tcl command
 *
 *  TclManager offers capability to implement your own tcl command with several simple steps
 *  Step to define a new Tcl command using this class
 *  1) Define your tcl command by inheriting QTclCommand
 *  2) Register your tcl command using TclManager::registerCommand()
 *  3) Implement your excute and help function
 */
class TclManager {

  typedef TclManager SELF;
  typedef std::map<std::string, QTclCommand*> NameToCommand;
public:

  /*! \brief get or create a tcl manager
   *  \return TclManager* the pointer of TclManager
   */
  static SELF* getOrCreate();

  /*! \brief set tcl interp
   *  \param inter tcl interpreter
   */
  void setTclInterp(Tcl_Interp* interp) { _interp = interp; }

  /*! \brief register command to tcl interp
   *  \param cmd class QTclCommand
   */
  void registerCommand(QTclCommand* cmd);

  /*! \brief execute tcl command
   *  \param cmd command name
   *  \param ClientData client data
   *  \param Tcl_Interp* a pointer for tcl interpreter
   *  \param argc argument count
   *  \param argv argument vector
   *  \return int TCL status
   */
  static int execute_cmd_main(std::string cmd, ClientData clientData, Tcl_Interp* interp, int argc, const char** argv);

  /*! \brief get interp pointer
   *  \return Tcl_Interp* a pointer for tcl interp
   */
  Tcl_Interp* getInterp() const { return _interp; }


  /*! \brief initialization 
   *  \param argc argument count
   *  \param argv argument vector
   */
  static void init(int argc, char** argv);


  friend void TclManagerCleanUp();

private:
  /*! \brief constructor
   */
  TclManager() : _interp(NULL) {}

  /*! \brief get tcl command
   *  \param cmd_name command name
   *  \return QTclCommand* a pointer for tcl command
   */
  QTclCommand* getCommand(std::string cmd_name) const {
    if (!_name_to_command.count(cmd_name))
      return NULL;
    else
      return _name_to_command.at(cmd_name);
  }




  Tcl_Interp* _interp;            //!< tcl interpreter
  static SELF* _tcl_manager;      //!< self pointer for singleton
  NameToCommand _name_to_command; //!< a map between command and command name


};




#endif


