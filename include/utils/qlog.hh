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

#ifndef QLOG_HH
#define QLOG_HH



/*!
 * \file qlog.hh
 * \author Juexiao Su
 * \date 11 Jul 2017
 * \brief log utility for quantum sat compiler
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
   * \param logFileName set log output file to logFileName
   */
  void setLogFile(const char* logFileName);

  /*!
   * \brief log function with step name
   * \param step step name
   * \param format format the output
   */
  void speak(const char* step, const char* format, ...) __attribute__((format(printf, 3,4)));


  /*!
   * \brief warning function with step name
   * \param step step name
   * \param format format the output
   */
  void speakError(const char* format, ...) __attribute__((format(printf, 2,3)));


  /*!
   * \brief log function with step name
   * \param format format the output
   */
  void speak(const char* format, ...) __attribute__((format(printf, 2,3)));


  /*!
   * \brief customize assert function
   * \param cond assert condition
   * \param msg assert message
   * \param file file name
   * \param line line number
   */
  void assert(const char* msg, const char* file, const int line) __attribute__((noreturn));



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
   * \param step step name
   * \param message message to print
   */
  void qfprintf(const char* step, const char* message);

  /*!
   * \brief customized fprintf
   * \param message message to print
   */
  void qfprintf(const char* message, bool cr=false);



  FILE* _logFile;     //!< log file handler
  bool _quiet;        //!< set the the qlog in silent mode


};


extern qLog qlog;
#define QASSERT(EXPR) ((EXPR)? (void)0 : qlog.assert(#EXPR, __FILE__, __LINE__))



#endif


