#ifndef __BOARDFEATURE_H__
#define __BOARDFEATURE_H__

#include "Fib2584Ai.h"

#define IGNORE false

class Fib2584Ai::BoardFeature{
	public:
		BoardFeature(const int [4][4], int);
		double getBoardScore( const double *const *, double const *);
		
		int outerFeature[4], innerFeature[4];
		int extraFeature;
		int mergeScore;
		string boardString;
		static void printTuple(int);
	
	private:
		int getExtraFeature(const int[4][4]);
		int getSequenceLength(const int [4][4], int, int);
};

#endif // __BOARDFEATURE_H__
