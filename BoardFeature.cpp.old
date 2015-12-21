#include "BoardFeature.h"


/**
	@constructor
	@arg
		board: 2D array of the game board
		score: merge score 
	
	initial field of the object.
	
	according to symmetry and reversal, classify 8 4-tuples into 2 groups 
	for every 4-tuple, choose smaller number as its index
	
	example: [6 2 13 5] and [5 13 2 6] are the same, choose 00101_01101_00010_00110 (5_13_2_6) as their index
	
	
	4 outer 4-tuples 		4 inner 4-tuples
	+-+-+-+-+               +-+-+-+-+
	|*|*|*|*|			    | |o|o| |
	+-+-+-+-+               +-+-+-+-+
	|*| | |*|               |o|o|o|o|
	+-+-+-+-+               +-+-+-+-+
	|*| | |*|               |o|o|o|o|
	+-+-+-+-+               +-+-+-+-+
	|*|*|*|*|               | |o|o| |
	+-+-+-+-+               +-+-+-+-+
	
*/
Fib2584Ai::BoardFeature::BoardFeature(const int board[4][4], int score){
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
            outerFeature[i] += getFibIndex(board[outerIndex[i][j][0]][outerIndex[i][j][1]]) << (15-5*j);
            innerFeature[i] += getFibIndex(board[innerIndex[i][j][0]][innerIndex[i][j][1]]) << (15-5*j);
        }
        int reverse = reverseTuple(outerFeature[i]);
        outerFeature[i] = (outerFeature[i] < reverse)? outerFeature[i] : reverse;
        reverse = reverseTuple(innerFeature[i]);
        innerFeature[i] = (innerFeature[i] < reverse)? innerFeature[i] : reverse;
    }
	
	if (IGNORE)
		extraFeature = 0;
	else
		extraFeature = getExtraFeature(board);
	
	mergeScore = score;
	boardString = toString(board);
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
		
	read the corresponding values in the value table and add them up
	outer features stored in  valueTable[OUTER]
	inner features stored in  valueTable[INNER]
*/
double Fib2584Ai::BoardFeature::getBoardScore(const double *const* valueTable,double const*extraTable){
	int score = 0;
	for (int i = 0 ; i < 4 ; i++){
		score += valueTable[OUTER][outerFeature[i]] + valueTable[INNER][innerFeature[i]];
	}
	
	if (!IGNORE)
		score += extraTable[extraFeature];
	
	return score;
}

int Fib2584Ai::BoardFeature::getExtraFeature(const int board[4][4]){
	
	int maxTile = 0, emptyCount = 0, mergeCount =  0, longestSequence = 0;
	int index[4][4];
	
	for (int i = 0 ; i < 4 ; i++){
		for (int j = 0 ; j < 4 ; j++){
			index[i][j] = getFibIndex(board[i][j]);
			if ( maxTile < board[i][j] ){
				maxTile = board[i][j];
			}
			
			if (board[i][j] == 0){
				emptyCount += 1;
			}
			
			if ( i+1 < 3 && canMerge(board[i][j],board[i+1][j])){
				mergeCount += 1;	
			}
			
			if ( j+1 < 3 && canMerge(board[i][j],board[i][j+1])){
				mergeCount += 1;	
			}
		}
	}
	
	for (int i = 0 ; i < 4 ; i++){
		for (int j = 0 ; j < 4; j ++){
			if (board[i][j] == maxTile){
				int tmp = getSequenceLength(index,i,j);
				if (longestSequence < tmp)
					longestSequence = tmp;
			}
		}
	}
	
	int fibIndex = getFibIndex(maxTile);
	return   fibIndex << 13 | mergeCount << 8 | emptyCount << 4 | longestSequence;
}

int Fib2584Ai::BoardFeature::getSequenceLength(const int index[4][4], int row, int col){
	const int deltaRow[] = {-1,1,0,0};
	const int deltaCol[] = {0,0,-1,1};

	int maxLength = 0;
	
	for (int i = 0 ; i < 4 ; i++){
        int nextRow = row+deltaRow[i];
        int nextCol = col+deltaCol[i];
        if ( index[row][col] > 0 && nextRow >= 0 && nextRow < 4 && nextCol >= 0 && nextCol < 4){
            if ( index[nextRow][nextCol] == 0 && maxLength < 1){
                maxLength = 1;
            }
            else if (index[row][col] == index[nextRow][nextCol]+2){
                int tmp = getSequenceLength(index,nextRow,nextCol);
                if ( tmp > 0 && maxLength < 1+tmp)
                    maxLength = 1+tmp;
            }
        }
	}
	return maxLength;
}