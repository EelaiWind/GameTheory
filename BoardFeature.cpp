#include "BoardFeature.h"

Fib2584Ai::BoardFeature::BoardFeature(int board[4][4], int score){
    const int outerIndex[4][4][2] ={
        {{0,0},{0,1},{0,2},{0,3}},
        {{3,0},{3,1},{3,2},{3,3}},
        {{0,0},{1,0},{2,0},{3,0}},
        {{0,3},{1,3},{2,3},{3,3}}
    };

    const int innerIndex[4][4][2] = {
        {{1,0},{1,1},{1,2},{1,3}},
        {{2,0},{2,1},{2,2},{2,3}},
        {{0,1},{1,1},{2,1},{3,1}},
        {{0,2},{1,2},{2,2},{3,2}}
    };

    for(int i = 0 ; i < 4 ; i++){
        outerFeature[i] = 0;
        innerFeature[i] = 0;
        for (int j = 0 ; j < 4; j++){
            outerFeature[i] += board[outerIndex[i][j][0]][outerIndex[i][j][1]] << (15-5*j);
            innerFeature[i] += board[innerIndex[i][j][0]][innerIndex[i][j][1]] << (15-5*j);
        }
        int reverse = reverseTuple(outerFeature[i]);
        outerFeature[i] = (outerFeature[i] < reverse)? outerFeature[i] : reverse;
        reverse = reverseTuple(innerFeature[i]);
        innerFeature[i] = (innerFeature[i] < reverse)? innerFeature[i] : reverse;
    }
	
	mergeScore = score;
	boardString = printBoard(board);
}

void Fib2584Ai::BoardFeature::printTuple(int tuple){
    int i = 0;

    for (i=0;i<4;i++){
        printf("%d ",(tuple&0xf8000)>>15);
        tuple = (tuple&0x07fff) << 5;
    }
    putchar('\n');
}

/**
	@arg
		valueTable: 2D array of feature weight
		
	@return
		score of the board
		
	
*/
double Fib2584Ai::BoardFeature::getBoardScore(double **valueTable){
	int score = 0;
	for (int i = 0 ; i < 4 ; i++){
		score += valueTable[OUTER][outerFeature[i]] + valueTable[INNER][innerFeature[i]];
	}
	return score;
}