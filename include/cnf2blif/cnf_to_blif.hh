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

#ifndef CNF_TO_BLIF_HH
#define CNF_TO_BLIF_HH


#include <vector>
#include <string>
#include <fstream>

/*!
 * \file cnf_to_blif.hh
 * \author Juexiao Su
 * \date 26 Dec 2017
 * \brief convert cnf to blif format
 */

namespace SYN {
  class Model;
}

class SatReaderWriter {
  public:
    /*! \brief default constructor
     */
    SatReaderWriter(SYN::Model* model = NULL): model_(model) {}
    ~SatReaderWriter() {}


    /*! \brief convert cnf format to blif
     */
    bool cnf2blif(std::fstream& infile, const std::string& gen_fname);

    /*! \brief check model correctness
     */
    void checkModel();

  private:

    /*! \brief
     */
    bool genBlif(const std::vector<std::vector<long>>& clauses, 
        const long& n_variable,
        const std::string& gen_fname);


    void genSingleAndGate(std::fstream& outfile, std::string in1, std::string in2, std::string in3, std::string in4, std::string out);
    void genSingleAndGate(std::fstream& outfile, std::string in1, std::string in2, std::string in3, std::string out);
    void genSingleAndGate(std::fstream& outfile, std::string in1, std::string in2, std::string out);

    /*! \brief check if the string is a integer
     */
    inline bool isInteger(const std::string & s)
    {
      if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;

      char * p ;
      std::strtol(s.c_str(), &p, 10) ;

      return (*p == 0) ;
    }

  private:
    SYN::Model* model_;


};





#endif

