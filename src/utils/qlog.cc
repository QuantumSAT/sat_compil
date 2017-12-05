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
 * \file qlog.cc
 * \author Juexiao Su
 * \date 11 Jul 2017
 * \brief log utility is implemetned here
 */

#include "utils/qlog.hh"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>


qLog qlog;  //!< global qLog class

const unsigned BUFFER_SIZE = 4096;       //!< specify the max buffer size
static char message_buffer[BUFFER_SIZE]; //!< message buffer


qLog::qLog() :
  _logFile(NULL) {}


qLog::qLog(const char* logFileName) :
  _logFile(NULL) {

    if (_logFile) {
      fclose(_logFile);
    }
    openFile(logFileName);

}

qLog::~qLog() {
  if (_logFile)
    fclose(_logFile);
}

void qLog::setLogFile(const char* logFileName) {
  if (_logFile) {
    fclose(_logFile);
    _logFile = NULL;
  }
  openFile(logFileName);
}

void qLog::speak(const char* step, const char* format, ...) {

  if( _quiet)
    return;

  va_list ap;
  va_start(ap, format);
  vsnprintf(message_buffer, BUFFER_SIZE, format, ap);
  va_end(ap);

  qfprintf(step, message_buffer);

}


void qLog::speakError(const char* format, ...) {

  std::string error = "!!!Error";

  va_list ap;
  va_start(ap, format);
  vsnprintf(message_buffer, BUFFER_SIZE, format, ap);
  va_end(ap);

  qfprintf(error.c_str(), message_buffer);
  abort();

}


void qLog::speakWarning(const char* format, ...) {
  std::string warning = "!!!Warning";
  va_list ap;
  va_start(ap, format);
  vsnprintf(message_buffer, BUFFER_SIZE, format, ap);
  va_end(ap);
  qfprintf(warning.c_str(), message_buffer);
}
 
void qLog::speak(const char* format, ...) {

  va_list ap;
  va_start(ap, format);
  vsnprintf(message_buffer, BUFFER_SIZE, format, ap);
  va_end(ap);


  std::fprintf(stderr, "%s", message_buffer);
  if (_logFile)
    std::fprintf(_logFile, "%s", message_buffer);
}





void qLog::skipLine() {
  speak("============================================================================");
}

void qLog::openFile(const char* logFileName) {

  if (logFileName) {
    _logFile = fopen(logFileName, "w");
    if (_logFile == NULL) {
      std::fprintf(stderr, "*** Error: Cannot open log file %s\n", logFileName);
    }
  }
  fflush(stderr);

}

void qLog::qfprintf(const char* step, const char* message) {

  std::fprintf(stderr, "*    %s : %s\n", step, message);
  if (_logFile)
    std::fprintf(_logFile, " *    %s : %s\n", step, message);
  fflush(stderr);
  fflush(_logFile);

}

void qLog::qfprintf(const char* message, bool cr) {

  std::fprintf(stderr, "%s%s", message, cr?"\n":"");
  if (_logFile)
    std::fprintf(_logFile, "%s%s", message, cr?"\n":"");

  fflush(stderr);
  fflush(_logFile);

}



void qLog::qassert(const char* expr, const char* file, int line) {
  std::fprintf(stderr, "!!! Assertion '%s' failed at %s line %d\n", expr, file, line);
  if (_logFile)
    std::fprintf(_logFile, "!!! Assertion '%s' failed at %s line %d\n", expr, file, line);
  fflush(stderr);
  fflush(_logFile);

  abort();
}


