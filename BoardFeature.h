#ifndef __BOARDFEATURE_H__
#define __BOARDFEATURE_H__

#include "Fib2584Ai.h"

#define IGNORE false //ignore extra feature

class Fib2584Ai::BoardFeature{
	public:
		BoardFeature(const int [4][4], int);
		float getBoardScore( float const *, float const *, float const *, float const *, float const *);
		
		int outer_axe_feature[8],inner_axe_feature[8],corner_rect_feature[8],center_rect_feature[4];
		int extraFeature;
		int mergeScore;
	private:
		int getExtraFeature(const int[4][4]);
		int getSequenceLength(const int [4][4], int, int);
		static const short outer_axe_index[8][6];
	    static const short inner_axe_index[8][6];
	    static const short corner_rect_index[8][6];
	    static const short center_rect_index[8][6];
	    static const int tupleOffset[6];
};

#endif // __BOARDFEATURE_H__
