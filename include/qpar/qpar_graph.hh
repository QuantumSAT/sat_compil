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

#ifndef QPAR_GRAPH_HH
#define QPAR_GRAPH_HH



/*! \file qpar_graph.hh
 *  \author Juexiao SU
 *  \data 12 Dec 2017
 *  \brief a set of componets of graph and fast access graph
 */


#include <vectro>
#include <map>

typedef int qvertex;
typedef int qedge;


/*! \brief a class that manages the containing relation ship between edge and vertex
 */
class vertex_to_edges {

public:
  typedef std::vector<qedge> edges;
  typedef std::vector<edges> vertex2edges;

private:
  vertex2edges _vertex2edges; //!< a map between vertex to its edges

  vertex_to_edges(unsigned num_of_vertex) {

  }

  void init_vertex_vector(const qvertex, unsigned num_of_edge) {
  
  }

  /*! \brief add a edge to its vertex
   */
  void add_edge(const qvertex v, const qedge e) {
    _vertex2edges[v] = e;
  }



};



#endif

