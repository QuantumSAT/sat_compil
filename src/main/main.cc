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
 * \file main.cc
 * \author Juexiao Su
 * \date 11 Jul 2017
 * \brief main function entry point
 */


#include "utils/qlog.hh"

#include <string>
#include <iostream>

extern std::string getBanner();
extern char* getBuildTime();



int main(int argc, char** argv) {

  std::cout << getBanner();
  std::cout << "Build Time: " << getBuildTime() << std::endl;

  qlog.setLogFile("QSat.log");
  qlog.speak("Test", "Hello World!");

  return 0;

}




