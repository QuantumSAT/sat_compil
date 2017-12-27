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

#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <string>
#include <queue>
#include <cassert>
#include <sstream>

#include "cnf2blif/cnf_to_blif.hh"
#include "utils/qlog.hh"

bool SatReaderWriter::cnf2blif(std::fstream& infile, const std::string& gen_fname) {
  long n_variable = -1, n_clause = -1;
  long clause_cnt = 0;
  std::unordered_set<long> variable_set;
  std::vector<std::vector<long> > clauses;
  std::vector<long> clause;
  std::string oneline;

  while (!infile.eof()) {
    getline(infile, oneline);
    //cout<<oneline<<endl;
    // Case 1. ignore empty line
    if (oneline.length() == 0) continue;
    // Case 2. ignore comment line
    if (oneline[0] == 'c') continue;
    // Case 3. problem line
    if (oneline[0] == 'p') {
      bool var_record = false;
      bool clause_record = false;

      std::istringstream iss(oneline);
      std::string each_word;
      while(iss >> each_word) {
        if (isInteger(each_word)) {
          char* pend;
          long num = strtol(each_word.c_str(), &pend, 10);
          //cout<<num<<endl;
          if (var_record == false) {
            n_variable = num;
            var_record = true;
            continue;
          }
          if (clause_record == false && var_record == true) {
            n_clause = num;
            clause_record = true;
          }
        }
      }

      assert(var_record == true && clause_record == true);
      //cout<<"n_variable: "<<n_variable<<" n_clause: "<<n_clause<<endl;
      continue;
    }

    // Case 4. data line
    // Now each line is the data of the cnf
    std::istringstream iss(oneline);
    std::string val;
    while(iss >> val) {
      if (!isInteger(val)) {
        qlog.speakError("Error: %s in cnf file is not a number!", val.c_str());
      }
      char* pend;
      long num = strtol(val.c_str(), &pend, 10);
      long abs_num = abs(num);
      // Step 1. not 0, push data into clause
      if (num != 0) {
        auto iter = variable_set.find(abs_num);
        if (iter == variable_set.end()) {
          variable_set.insert(abs_num);
        }
        clause.push_back(num);
        // Step 2. end of a clause, put clause into clauses and clear clause
      } else {
        clauses.push_back(clause);
        clause.clear();
        clause_cnt++;
      }
    }
  }

  assert(clause_cnt == n_clause);

  assert(variable_set.size() <= (size_t)n_variable);

  return genBlif(clauses, n_variable, gen_fname);

}



bool SatReaderWriter::genBlif(const std::vector<std::vector<long>>& clauses,
    const long& n_variable, 
    const std::string& gen_fname) {
  // Step 1. open outfile
  std::fstream ofile;
  ofile.open(gen_fname.c_str(), std::ios::out);
  if (!ofile.is_open()) {
    qlog.speakError("Error: cnf->blif file %s does not exits.", gen_fname.c_str());
    return false;
  }

  // Step 2. write PIs and PO
  std::unordered_map<long, std::string> id2name;
  for (int i = 0; i < n_variable; ++i) {
    const std::string pi_name = "in_" + std::to_string(i+1);
    id2name.insert(make_pair(i+1,pi_name));
  }
  std::string model_name;
  std::stringstream ss(gen_fname);
  getline(ss,model_name,'.');

  ofile<<".model "<<model_name<<std::endl;
  ofile<<".inputs";
  for (int i = 0; i < n_variable; ++i) {
    std::string pi_name = id2name[i+1];
    ofile<<" "<<pi_name;
  }
  ofile<<std::endl;
  ofile<<".outputs O"<<std::endl;



  // Step 3. write each clause
  std::vector<std::string> clause_names;
  for (int i = 0; i < (int)clauses.size(); ++i) {
    const std::string clause_name = "m_" + std::to_string(i+1);
    clause_names.push_back(clause_name);
    const std::vector<long>& clause = clauses[i];
    ofile<<".names";
    std::vector<int> polarities;
    for (int j = 0; j < (int)clause.size(); ++j) {
      //cout<<clause[j]<<" ";
      // pay attention tth
      long input = clause[j];
      long abs_input = abs(input);
      auto iter = id2name.find(abs_input);
      assert(iter != id2name.end());

      std::string input_name = id2name[abs_input];
      ofile <<" "<<input_name;
      if (input > 0) {
        polarities.push_back(1);
      } else if (input < 0) {
        polarities.push_back(0);
      } else {
        assert(0);
      }
    }
    ofile<<" "<<clause_name<<std::endl;
    assert(polarities.size() == clause.size());
    int n_polarities = polarities.size();
    for (int j = 0; j < n_polarities; ++j) {
      for (int k = 0; k < n_polarities; ++k) {
        if (j == k) {
          if (polarities[j] == 1) {
            ofile<<"1";
          } else if (polarities[j] == 0) {
            ofile<<"0";
          } else {
            assert(0);
          }
        } else {
          ofile<<"-"; 
        }
      }
      ofile<<" 1"<<std::endl;
    }


  }
  std::queue<std::string> frontier;
  for (size_t i = 0; i < clause_names.size(); ++i) {
    frontier.push(clause_names[i]);
  }
  unsigned int name_counter = clause_names.size() + 1;

  while (frontier.size() > 4) {
    std::string in1 = frontier.front();
    frontier.pop();
    std::string in2 = frontier.front();
    frontier.pop();
    std::string in3 = frontier.front();
    frontier.pop();
    std::string in4 = frontier.front();
    frontier.pop();
    std::string output_name = "m_" + std::to_string(name_counter);
    genSingleAndGate(ofile, in1, in2, in3, in4, output_name);
    ++name_counter;
    frontier.push(output_name);
  }

  if (frontier.size() == 4) {
    std::string in1 = frontier.front();
    frontier.pop();
    std::string in2 = frontier.front();
    frontier.pop();
    std::string in3 = frontier.front();
    frontier.pop();
    std::string in4 = frontier.front();
    frontier.pop();
    genSingleAndGate(ofile, in1, in2, in3, in4, "O");
  } else if (frontier.size() == 3) {
    std::string in1 = frontier.front();
    frontier.pop();
    std::string in2 = frontier.front();
    frontier.pop();
    std::string in3 = frontier.front();
    frontier.pop();
    genSingleAndGate(ofile, in1, in2, in3, "O");
  } else if (frontier.size() == 2) {
    std::string in1 = frontier.front();
    frontier.pop();
    std::string in2 = frontier.front();
    frontier.pop();
    genSingleAndGate(ofile, in1, in2, "O");
  } else {
    assert(0); // impossible case
  }

  assert(frontier.empty());

  ofile<<".end";
  ofile.close();

  return true;
}

void SatReaderWriter::genSingleAndGate(std::fstream& outfile, std::string in1, std::string in2, std::string in3, std::string in4, std::string out) {
  outfile << ".names";
  outfile << " " << in1 << " " << in2 << " " << in3 << " " << in4 << " " << out << std::endl;
  outfile << "1111 1" << std::endl;
}

void SatReaderWriter::genSingleAndGate(std::fstream& outfile, std::string in1, std::string in2, std::string in3,std::string out) {
  outfile << ".names";
  outfile << " " << in1 << " " << in2 << " " << in3 << " " << out << std::endl;
  outfile << "111 1" << std::endl;
}

void SatReaderWriter::genSingleAndGate(std::fstream& outfile, std::string in1, std::string in2, std::string out) {
  outfile << ".names";
  outfile << " " << in1 << " " << in2 << " " << out << std::endl;
  outfile << "11 1" << std::endl;
}

//void SatReaderWriter::checkModel() {
//  qlog.speak("CNF2BLIF","Start checking blif model"); 
//  model_->getNetListFromElem();
//  int n_net = model_->getNumNets();
//  int n_elem = model_->getNumElem();
//  int n_port = model_->getNumPort();
//  qlog.speak("CNF2BLIF","n_net: %d, n_elem: %d, n_port %d",n_net, n_elem, n_port);
//
//  vector<Pin*> all_pin;
//  model_->topoOrder(all_pin);
//
//  for (int i = 0; i < all_pin.size(); ++i) {
//    Pin* pin = all_pin[i];
//    qlog.speak("CNF2BLIF","Is gate pin? %d",pin->isGatePin());
//
//  }
//
//  int i = 1;
//
//  return;
//}


