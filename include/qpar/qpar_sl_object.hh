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

#ifndef QPAR_SL_OBJECT_HH
#define QPAR_SL_OBJECT_HH

/*! 
 * \file qpar_sl_object.hh
 * \author Juexiao Su
 * \data 06 Nov 2017
 * \brief class of object with save and load function
 *
 * Make sure the template class T has operator= overload
 * default save and restore function will use operator=
 */


template <class T>
class ParSaveAndLoadObject {

public:

  /*!\brief default constructor
   */
  ParSaveAndLoadObject() {}

  /*! \brief default constructor
   */
  ParSaveAndLoadObject(const T& init_status):
  _cur_status(init_status),
  _pre_status(init_status) {}

  /* \brief default destructor
   */
  ~ParSaveAndLoadObject() {}

  /*! \brief save current status to previous status
   */
  virtual void saveStatus();

  /*! \brief restore to previous status
   */
  virtual void restoreStatus();

  /*! \brief get current status
   */
  const T& getStatus() const { return _cur_status; }

  /*! \brief accessor
   */
  T& getStatus() { return _cur_status; }

  /*! \brief set current status
   */
  void setStatus(T status) { _cur_status = status; }

  bool checkStatusSame() const {
    return _current_status == _pre_status;
  }


private:
  T _cur_status; //!< current status
  T _pre_status; //!< previous status

};

//#define QPAR_SL_OBJ ParSaveAndLoadObject;


#endif
