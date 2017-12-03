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

#ifndef QPAR_UTILS_HH
#define QPAR_UTILS_HH

/*! 
 * \file qpar_utils.hh
 * \author Juexiao Su
 * \data 07 Nov 2017
 * \brief a collection of utilities used in qpar
 */

#include <random>


/* \brief this is a random generator used in the entire binary
 * 
 * The sequence is deterministic
 */
class RandomGenerator {

  typedef RandomGenerator SEFL;
public:
  /*! \brief default constructor
   */
  RandomGenerator() : _gen(2) {}

  /*! \brief construtor with random seed
   */
  RandomGenerator(int seed) : _gen(seed) {}

  /*! \brief generate a random unsigned value within given range
   *  \param unsigned min
   *  \param unsigned max
   *  \return unsigned random number
   */
  unsigned uRand(unsigned min, unsigned max);

  /*! \brief generate a random signed value within given range
   *  \param signed min
   *  \param signed max
   *  \return signed random number
   */
  int iRand(int min, int max);

  /*! \brief gnerate a random float value within given range
   *  \param float min
   *  \param float max
   *  \return float random number
   */
  float fRand(float min, float max);

private:
  std::mt19937 _gen; //!< random number engine
};



class Annealer {

  const float annealer_best_rate = 0.44f;
  const float annealer_step_1 = 0.96f;
  const float annealer_step_2 = 0.8f;
  const float annealer_step_3 = 0.15f;
  const float annealer_cool_speed1 = 0.2f;
  const float annealer_cool_speed2 = 0.75f;
  const float annealer_cool_speed3 = 0.85f;
  const float annealer_cool_speed4 = 0.6f;

public:

  /*! brief default construtor
   */
  Annealer(float init_t, float b_factor, float r_max) :
    _initial_t(init_t),
    _current_t(init_t),
    _radius_max(r_max),
    _radius_limit(r_max),
    _boltzmann(b_factor) {
      _rand = new RandomGenerator;
    }

  /*\brief default destructor
   */
  ~Annealer() {
    if (_rand) delete _rand;
    _rand = NULL;
  }

  /*! \brief get current temperature
   *  \return float current temperature
   */
  float getCurrentT() const { return _current_t; }


  /*! \brief update temperature by given success rate
   *  \param float success rate 
   */
  void updateT(float success_rate);

  /*! \brief update temperature by given success rate
   *  \param float success rate 
   */
  void updateMoveRadius(float success_rate);

  /*! \brief decide whether the manipulation should be accepted
   *  \param float delta_cost the cost reduction
   *  \return bool
   */
  bool shouldAccept(float delta_cost);

  /*! \brief decide whether the annealing procedure should be exit
   *  \param float unit_cost is a normalized bounding box cost
   */
  bool shouldExit(float unit_cost);

  /*! \brief get radius limit
   */
  float getRLimit() const { _radius_limit; }

  /*! \brief set radius limit
   */
  void setRLimit(float val) { _radius_limit = val; }

  /*! \brief set current annealing temperature
   */
  void setCurrentT(float t) {
    _current_t = t;
  }

  /*! \brief set initial annealing temperature
   */
  void setInitT(float t) {
    _initial_t = t;
  }

private:

  RandomGenerator* _rand; //!< random nubmer gen

  float _initial_t; //!< intial temperature
  float _current_t; //!< current temperature

  float _radius_max; //!< maxium allowed move radius
  float _radius_limit; //!< the radius limit during annealing
  float _boltzmann; //!< boltzmann factor

};

/*! \brief box class to record boundary value
 */
class Box {
  public:
    /*! \brief default constructor
     */
    Box(int xl, int xr, int yt, int yb) :
      xl_(xl),
      xr_(xr),
      yt_(yt),
      yb_(yb) {}

    /*! \brief get left x value
     */
    int xl() const { return xl_ ; }
    /*! \brief set left x value
     */
    void set_xl(int xl) { xl_ = xl ; }
    /*! \brief increse left x value
     */
    void incr_xl() { xl_++ ; }
    /*! \brief decrese left x value
     */
    void decr_xl() { xl_-- ; }

    /*! \brief set right x value
     */
    int xr() const { return xr_ ; }
    /*! \brief set right x value
     */
    void set_xr(int xr) { xr_ = xr ; }
    /*! \brief increse right x value
     */
    void incr_xr() { xr_++ ; }
    /*! \brief decrese right x value
     */
    void decr_xr() { xr_-- ; }

    /*! \brief get top y value
     */
    int yt() const { return yt_ ; }
    /*! \brief set top y value
     */
    void set_yt(int yt) { yt_ = yt ; }
    /*! \brief increse top y value
     */
    void incr_yt() { yt_++ ; }
    /*! \brief decrese top y value
     */
    void decr_yt() { yt_-- ; }

    /*! \brief get bottom y value
     */
    int yb() const { return yb_ ; }
    /*! \brief set bottom y value
     */
    void set_yb(int yb) { yb_ = yb ; }
    /*! \brief increse bottom y value
     */
    void incr_yb() { yb_++ ; }
    /*! \brief decerase bottom y value
     */
    void decr_yb() { yb_-- ; }

    /*! \brief get width
     */
    int width() const { return xr_ - xl_; }
    /*! \brief get height
     */
    int height() const { return yt_ - yb_; }

    void operator=(const Box& other) {
      xl_ = other.xl_ ;
      xr_ = other.xr_ ;
      yt_ = other.yt_ ;
      yb_ = other.yb_ ;
    }

    bool operator==(const Box& right) const {
      return ((*this).xl_ == right.xl_) &&
             ((*this).xr_ == right.xr_) &&
             ((*this).yt_ == right.yt_) &&
             ((*this).yb_ == right.yb_);
    }

    /*! \brief check if a give point within the box
     *  \return bool
     */
    bool isInBox(int x, int y) const;

  private:
    int xl_ ;
    int xr_ ;
    int yt_ ;
    int yb_ ;
} ;




#endif
