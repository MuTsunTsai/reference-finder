
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <limits>

/**********
Globals
**********/

const double EPS = 1.0e-8; // used for equality of XYPts, parallelness of XYLines

typedef unsigned short rank_t;
typedef int key_t;

/*****
Utilities
*****/
template <class T>
inline T min_val(T t1, T t2) {return (t1 < t2) ? t1 : t2;};
template <class T>
inline T max_val(T t1, T t2) {return (t1 > t2) ? t1 : t2;};

#endif