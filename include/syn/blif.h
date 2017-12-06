/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set expandtab tabstop=2 softtabstop=2 shiftwidth=2: */

#ifndef _BLIF_H_
#define _BLIF_H_


#include <cstdio>
//#include <iostream>
//#include <fstream>

namespace SYN {
#define BUF_SIZE 51200

  class BlifReaderWriter {
  public:
    BlifReaderWriter() {}
    ~BlifReaderWriter() {}

    bool isSep(const char c) {
      return c == ' ' || c == '\0' || c == '\n';
    }
    char* getToken(char* buf, char*& rest);
    char* readLine(FILE*);
    void readDesign(FILE*);
    void writeDesign(FILE*);
  private:
    char _buf[BUF_SIZE];
  };

}

#endif //_BLIF_H_

