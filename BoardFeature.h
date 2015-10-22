#ifndef __BOARDFEATURE_H__
#define __BOARDFEATURE_H__

#include "Fib2584Ai.h"

class Fib2584Ai::BoardFeature{
	public:
		BoardFeature(int [4][4], int);
		double getBoardScore(double **);
		
		int outerFeature[4], innerFeature[4];
		double mergeScore;
		string boardString;
		static void printTuple(int);
};

#endif // __BOARDFEATURE_H__
