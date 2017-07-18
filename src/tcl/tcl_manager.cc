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
 * \file tcl_mannager.cc
 * \author Juexiao Su
 * \date 14 Jul 2017
 * \brief tcl utility to manage all tcl commands
 */

#include "tcl/tcl_manager.hh"

TclManager* TclManager::_tcl_manager = NULL;


TclManager* TclManager::getOrCreate() {
  if (_tcl_manager == NULL)
    _tcl_manager = new TclManager;
  return _tcl_manager;
}




