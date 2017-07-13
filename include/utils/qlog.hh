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

#ifndef QLOG_H
#define QLOG_H



/*!
 * \file main.cc
 * \author Juexiao Su
 * \date 11 Jul 2017
 * \brief log utility for quantum annealer
 */


#include <string>
#include <stdlib.h>


class qLog {


public:

  /*!
   * \brief default constructor
   */
  qLog();

  /*!
   * \brief constructor with logfile set
   */
  qLog(const char* logFileName);

  /*! 
   * \brief default destrucor
   */
  ~qLog();

  /*!
   * \brief change log file name
   * \param set log output file to logFileName
   */
  void setLogFile(const char* logFileName);

  /*!
   * \brief log function with step name
   * \param step name
   * \param format the output
   */
  void speak(const char* step, const char* format, ...) __attribute__((format(printf, 3,4)));

  /*!
   * \breif basic log function
   * \param print out message
   * \param start a new line
   */
  void speak(const char* message, bool cr = false);



  /*!
   * \brief skip a line
   */
  void skipLine();


private:

  /*!
   * \brief open log file
   * \param logFileName name of log file
   */
  void openFile(const char* logFileName);

  /*!
   * \brief customized fprintf
   * \param step name
   * \param message to print
   */
  void qfprintf(const char* step, const char* message);

  FILE* _logFile;     //!< log file handler
  bool _quiet;        //!< set the the qlog in silent mode


};

extern qLog qlog;


#endif


