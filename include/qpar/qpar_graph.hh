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
 *  \date 12 Dec 2017
 *  \brief a set of componets of graph and fast access graph
 */


#include <vector>
#include <map>
#include <unordered_map>

#include "utils/qlog.hh"

typedef int qvertex;
typedef int qedge;




/*! \brief a class that manages the relationship between edge and vertex
 */
class vertex2edge {

public:
  typedef std::vector<qedge> edges;
  typedef std::vector<edges> vertex2edges;
  typedef edges::iterator edge_iter;

private:
  vertex2edges _vertex2edges; //!< a map between vertex to its edges

  /*! \brief default constructor
   */
  vertex2edge(const unsigned num_of_vertex) {
    _vertex2edges.resize(num_of_vertex);
  }

  /*! \brief add a edge to its vertex
   */
  void add_edge(const qvertex v, const qedge e) {
    _vertex2edges[v].push_back(e);
  }

  /*! \breif get edges
   */
  edges& at(const qvertex v) { return  _vertex2edges[v]; }

  /*! \brief get edges that connects to the vertex
   *  \param qvertex vertex
   *  \param edge_iter
   */
  std::pair<edge_iter, edge_iter> get_edges(const qvertex v) {
    QASSERT(v < int(_vertex2edges.size()));
    return std::make_pair(_vertex2edges[v].begin(), _vertex2edges[v].end());
  }

};


/*! a fast access graph infrastructure, as all the memeory is aclocated in a consequtive space,
 *  for high cache hit
 */
template<class eV, class eE>
class qpr_graph {

public:
  typedef std::unordered_map<eV, qvertex>   e2ivertex;
  typedef std::vector<eV>                   i2evertex;

  typedef std::unordered_map<eE, qedge>     e2iedge;
  typedef std::vector<eE>                   i2eedge;

  typedef std::vector<std::pair<qvertex, qvertex> > edge2vertex;

  /*! \brief default constructor 
   *  \param num_vertex num of vertices in the original graph
   *  \param num_edge num of edges in the original graph
   */
  qpr_graph(unsigned int num_vertex, unsigned int num_edge) : _vertex2edge(num_vertex),
  _vertex_counter(0), _edge_counter(0) {
    _i2evertex.resize(num_vertex);
    _i2eedge.resize(num_edge);
    _edge2vertex.resize(num_edge);
  }

  /*! \brief get the internal vertex
   *  \param eV external vertex
   */
  qvertex get_i_vertex(const eV e_vertex) const {
    return _e2ivertex[e_vertex];
  }

  /*! \brief add vertex in to the graph
   *  \param eV external vertex
   *  \param num_edge vertex degree
   */
  qvertex add_vertex(const eV e_vertex, const unsigned int num_edge) {
    qvertex vertex = _vertex_counter++;
    _vertex2edge.at(vertex).reserve(num_edge);
    _i2evertex[vertex] = e_vertex;
    _e2ivertex[e_vertex] = vertex;
    return vertex;
  }

  /*! \brief add edge to the graph
   *  \param eE external Edge
   *  \param qvertex internal vertex1
   *  \param qvertex internal vertex2
   *  \return qedge the newly created edge
   */
  qedge add_edge(const eE e_edge, const qvertex vertex1, const qvertex vertex2) {
    qedge edge = _edge_counter++;
    _i2eedge[edge] = e_edge;
    _e2iedge[e_edge] = edge;
    _edge2vertex[edge] = std::make_pair<vertex1, vertex2>;
    _vertex2edge.add_edge(vertex1, edge);
    _vertex2edge.add_edge(vertex2, edge);
    return edge;
  }

  /*! \brief add edge to the graph
   *  \param eE external Edge
   *  \param eV external vertex1
   *  \param eV external vertex2
   *  \return qedge the newly created edge
   */
  qedge add_edge(const eE e_edge, const eV vertex1, const eV vertex2) {
    qvertex i_vertex1 = get_i_vertex(vertex1);
    qvertex i_vertex2 = get_i_vertex(vertex2);
    return add_edge(e_edge, i_vertex1, i_vertex2);
  }

  /*! \brief get the internal edge
   *  \param eE external edge
   *  \return qedge internal edge
   */
  qedge get_i_edge(const eE e_edge) {
    return _e2iedge[e_edge];
  }

  /*! \brief get the external edge
   *  \param qedge internal edge
   *  \return eV external edge
   */
  eE get_e_edge(const qedge edge) {
    return _i2eedge[edge];
  }

  /*! \brief get the external vertex
   *  \param qvertex
   */
  eV get_e_vertex(const qvertex vertex) {
    return _i2evertex[vertex];
  }



private:
  qvertex _vertex_counter;    //!< vertex counter
  qedge   _edge_counter;      //!< edge counter

  e2ivertex     _e2ivertex;   //!< external to internal vertex
  i2evertex     _i2evertex;   //!< internal to external vertex

  e2iedge       _e2iedge;     //!< external to internal edge
  i2eedge       _i2eedge;     //!< internal to external edge

  edge2vertex   _edge2vertex; //!< edge to vertices
  vertex2edge   _vertex2edge; //!< vertex to edges


};


#endif

