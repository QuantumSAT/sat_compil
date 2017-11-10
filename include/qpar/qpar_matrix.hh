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

#ifndef QPAR_MATRIX_HH
#define QPAR_MATRIX_HH

/*!
 * \file qpar_matrix.hh
 * \author Juexiao Su
 * \date 08 Non 2017
 * \brief a matrix class
 */


#include <vector>

template <class T>
class qpr_matrix {


public:
  /*! \brief default constructor
   */
  qpr_matrix(unsigned x, unsigned y);

  /*! \brief access the data in the matrix
   */
  T& cell(unsigned x, unsigned y);


private:

  std::vector<T> _data; //<! data in the matrix is stored in a vector
  unsigned _maxX; //!< column number of the matrix
  unsigned _maxY; //!< row number of the matrix
};



#endif

