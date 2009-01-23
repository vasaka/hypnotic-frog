#ifndef INDEX_H
#define INDEX_H

#include "service_defs.h"

template<class T>
class Index
{
  int xStep_m;
  int yStep_m;
  DISALLOW_COPY_AND_ASSIGN(Index);
  public:
    Index(T);
    Index(int xStep, int yStep): xStep_m(xStep),yStep_m(yStep) {}
    int operator()(int x, int y)
    {
      return x*xStep_m + y*yStep_m;
    }
};

#endif // INDEX_H


