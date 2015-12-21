#include "BoardFeature.h"

const short Fib2584Ai::BoardFeature::outer_axe_index[8][6] = {{12,8,4,0,1,5},{0,1,2,3,7,6},{3,7,11,15,14,10},{15,14,13,12,8,9},{0,4,8,12,13,9},{12,13,14,15,11,10},{15,11,7,3,2,6},{3,2,1,0,4,5}};
const short Fib2584Ai::BoardFeature::inner_axe_index[8][6] = {{13,9,5,1,2,6},{4,5,6,7,11,10},{2,6,10,14,13,9},{11,10,9,8,4,5},{1,5,9,13,14,10},{8,9,10,11,7,6},{14,10,6,2,1,5},{7,6,5,4,8,9}};
const short Fib2584Ai::BoardFeature::corner_rect_index[8][6] = {{0,1,2,6,5,4},{0,4,8,9,5,1},{3,2,1,5,6,7},{3,7,11,10,6,2},{12,8,4,5,9,13},{12,13,14,10,9,8},{15,11,7,6,10,14},{15,14,13,9,10,11}};
const short Fib2584Ai::BoardFeature::center_rect_index[8][6] = {{1,5,9,2,6,10},{13,9,5,14,10,6},{4,5,6,8,9,10},{7,6,5,11,10,9}};
const int Fib2584Ai::BoardFeature::tupleOffset[6] = {1,20,400,8000,160000,3200000};

Fib2584Ai::BoardFeature::BoardFeature(const int board[4][4], int score){
    int i,j;
    int lineBoard[16];

    mergeScore = score;
    for (i=0;i<4;i++){
        for(j=0;j<4;j++){
            lineBoard[i*4+j] = getFibIndex(board[i][j]);
        }
    }
    for (i=0;i<8;i++){
        outer_axe_feature[i] = 0;
        inner_axe_feature[i] = 0;
        corner_rect_feature[i] = 0;
        for (j=0;j<6;j++){
            outer_axe_feature[i] += lineBoard[ outer_axe_index[i][j] ]*tupleOffset[j];
            inner_axe_feature[i] += lineBoard[ inner_axe_index[i][j] ]*tupleOffset[j];
            corner_rect_feature[i] += lineBoard[ corner_rect_index[i][j] ]*tupleOffset[j];
        }
    }

    for (i=0;i<4;i++){
        int tmp1 = 0 , tmp2 = 0;
        for (j=0;j<3;j++){
            tmp1 += lineBoard[ center_rect_index[i][j] ]*tupleOffset[j];
            tmp2 += lineBoard[ center_rect_index[i][j+3] ]*tupleOffset[j];
        }
        /*處理對稱性*/
        if (tmp1 <= tmp2){
            center_rect_feature[i] = tmp1*tupleOffset[3] + tmp2;
        }
        else{
            center_rect_feature[i] = tmp2*tupleOffset[3] + tmp1;
        }
    }
	
	if (IGNORE)
		extraFeature = 0;
	else
		extraFeature = getExtraFeature(board);
	
	mergeScore = score;
}

float Fib2584Ai::BoardFeature::getBoardScore(float const *outer_axe_table, float const *inner_axe_table, float const *corner_rect_table, float const *center_rect_table, float const *extraTable){
	
	float score = 0;
	for (int i = 0 ; i < 8 ; i++){
		score += outer_axe_table[outer_axe_feature[i]] + inner_axe_table[inner_axe_feature[i]] + corner_rect_table[corner_rect_feature[i]];
	}

	for (int i = 0 ;  i< 4 ; i++){
		score += center_rect_table[center_rect_feature[i]];
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
