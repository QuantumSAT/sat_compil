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


template <class T>
qpr_matrix<T>::qpr_matrix(unsigned x, unsigned y) :
  _maxX(x), _maxY(y) {
    _data.resize(x * y);
}

template <class T>
T &qpr_matrix<T>::cell(unsigned x, unsigned y) {
  QASSERT(x < _maxX);
  QASSERT(y < _maxY);
  return _data[y * _maxX + x];
}

template <class T>
const T &qpr_matrix<T>::cell(unsigned x, unsigned y) const {
  QASSERT(x < _maxX);
  QASSERT(y < _maxY);
  return _data.at(y * _maxX + x);
}



