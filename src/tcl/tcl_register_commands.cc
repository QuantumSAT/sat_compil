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
 * \file tcl_register_commands.cc
 * \author Juexiao Su
 * \date 21 Jul 2017
 * \brief register all tcl commands
 */

#include "tcl/tcl_manager.hh"
#include "tcl/tcl_command.hh"
#include "tcl/tcl_basic_command.hh"

#include "syn/syn_tcl.hh"
#include "hw_target/hw_tcl.hh"
#include "qpar/qpar_tcl.hh"


void registerAllCommands() {

  TclManager* tcl_manager = TclManager::getOrCreate();

  tcl_manager->registerCommand(new QCOMMAND_source("source", "<string>"));
  tcl_manager->registerCommand(new QCOMMAND_hello("hello",""));

  //netlist related
  tcl_manager->registerCommand(new QCOMMAND_read_blif("read_blif","<string>"));
  tcl_manager->registerCommand(new QCOMMAND_write_blif("write_blif","<string>"));

  //hardware related
  tcl_manager->registerCommand(new QCOMMAND_init_target("init_target","-row <int> -col <int> -local <int>"));
  tcl_manager->registerCommand(new QCOMMAND_gen_dwave_nl("gen_dwave_nl",""));

  //placement and routing related
  tcl_manager->registerCommand(new QCOMMAND_build_qpar_nl("build_qpar_nl", ""));
  tcl_manager->registerCommand(new QCOMMAND_init_system("init_system", ""));
  tcl_manager->registerCommand(new QCOMMAND_place("place", ""));
  tcl_manager->registerCommand(new QCOMMAND_check_routing_graph("check_routing_graph", ""));
  tcl_manager->registerCommand(new QCOMMAND_route("route", ""));


}

