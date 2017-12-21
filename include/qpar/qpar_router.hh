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

#ifndef QPAR_ROUTER_HH
#define QPAR_ROUTER_HH


#include "qpar/qpar_graph.hh"

#include <unordered_set>
#include <priority_queue>
#include <vector>
#include <algorithm>


class FastRoutingGraph;
class RoutingCost;
class RoutingNode;
class ParWireTarget;


/*!
 * \file qpar_router.hh
 * \author Juexiao Su
 * \date 20 Dec 2017
 * \brief router which implements shortest path algorithm
 */


/*! \brief customized priority queue that helps to store more information
 */

/*! \brief priority element, will keep track current length 
 */
typedef std::pair<double, qvertex> QElement;

/*! \breif priority queue element
 */
typedef std::pair<double, QElement> PElement;

struct PQCompare {

  bool operator()(PElement& e1, PElement& e2) {
    if (e1.first < e2.first)
      return false;
    else if (e1.first > e2.first)
      return true;
    else
      return e1.second.second > e2.second.second; //break tie by qvertex
  }

};


class QPriorityQueue : public std::priority_queue<PElement, std::vector<PElement>, PQCompare> {

public:
  typedef std::priority_queue<PElement, std::vector<PElement>, PQCompare> SUPER;

public:
  /*! \brief copy the internal vector
   */
  std::vector<PElement> getVector() {
    std::vector<PElement> re(this->c.begin(), this->c.end());
    std::sort(re.begin(), re.end(), this->comp);
    std::reverse(re.begin(), re.end());
  }

  /*! \brief push element into priority queue
   */
  void push(double p, const QElement& ele) {
    SUPER::push(std::make_pair(p, ele));
  }

  /*! \brief pop the top of the pqueue
   */
  std::pair<double, QElement> pop() {
    if (this->size() == 0)
      qlog.speakError("priority queue", "Cannot pop an empty queue!");
    PElement top_ele = SUPER::top();
    SUPER::pop();
    return top_ele;
  }

};

class ParRouter {


public:

  /*! \brief default constructor
   */
  ParRouter(FastRoutingGraph& graph, RoutingCost& cost) :
    _graph(graph), _cost(cost) {
      _visited_node.resize(graph.get_vertex_num());
    }

  /*! \brief route target
   */
  bool route(RoutingNode* src, RoutingNode* tgt, double slack, std::unordered_set<RoutingNode*>& used, ParWireTarget* target);

  /*! \brief back trace to find the path
   */
  void buildRoutePath(std::list<RoutingNode*>& path);


private:
  FastRoutingGraph& _graph; //!< routing graph;
  RoutingCost& _cost; //!< routing cost;

  /*! \brief add all routed resource to current routing tree, so that
   *         new route will branch out from existing tree, note that
   *         a route is always a tree
   */
  void pushRoutedNodes(std::unordered_set<RoutingNode*>& used);

  void expandNeighbors(QPriorityQueue* pqueue, qvertex current_vertex, double real_length, double slack, ParWireTarget* target);

  qvertex popBestVertex(QPriorityQueue* pqueue, double& current_cost, double& real_cost);

  std::vector<double> _visisted_node;
  std::vector<qedge> _from_edge;
  qvertex _source;
  qvertex _target;

};







#endif

