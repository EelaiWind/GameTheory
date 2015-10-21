#include "Fib2584Ai.h"

Fib2584Ai::Fib2584Ai()
{
	totalScore = 0;
	alpha = 0.002;
	
	valueTable = (double **)malloc(2*sizeof(double *)+2*TABLE_SIZE*sizeof(double));
	int i ;
	double *pData;
	for (i = 0, pData = (double *)(valueTable+2); i < 2; i++, pData += TABLE_SIZE)
		valueTable[i]=pData;
	
	FILE *file = fopen("valueTable.txt","r");
	if (file == NULL){
		file = fopen("valueTable.txt","w");
		for (int i = 0 ; i < TABLE_SIZE ; i++){
			if ( i <= reverseTuple(i))
				fprintf(file,"%d 0 0\n",i);
		}
		fclose(file);
	}
	else{
		int index,count = 0;
		double outter,inner;
		while(true){
			count += 1;
			if ( fscanf(file,"%d %lf %lf\n",&index, &outter, &inner) == EOF)
				break;
		}
		fclose(file);
	}
}

void Fib2584Ai::initialize(int argc, char* argv[])
{
	srand(time(NULL));
	return;
}


MoveDirection Fib2584Ai::generateMove(int board[4][4])
{
	/** choose next direction*/
    MoveDirection dir = static_cast<MoveDirection>(rand() % 4);
	
	int score = moveTile(dir,board);
	totalScore += score;
	BoardFeature feature(board, score);
	recordfeature.push_back(feature);
	
	return dir;
}

void Fib2584Ai::gameOver(int board[4][4], int iScore)
{	
	/** add dead board to the list*/
	BoardFeature feature(board, 0);
	recordfeature.push_back(feature);
	
	/*std::list<BoardFeature>::iterator iterFeature;
	for (iterFeature = recordfeature.begin(); iterFeature != recordfeature.end() ; iterFeature++){
		cout << iterFeature->score << endl;
		cout << iterFeature->boardString << endl;
		cout << "outer features" << endl;
		for (int i = 0 ; i < 4 ; i++){
			BoardFeature::printTuple(iterFeature->outerFeature[i]);
		}
		cout << "inner features" << endl;
		for (int i = 0 ; i < 4 ; i++){
			BoardFeature::printTuple(iterFeature->innerFeature[i]);
		}
	}*/
	cout << "avg delta = " << runBackwardPropagation() << endl;
	return;
}

/**********************************
You can implement any additional functions
you may need.
**********************************/

/**
	return movement score
*/
int Fib2584Ai::moveTile(MoveDirection moveDirection,int board[4][4]){

    if (moveDirection == MOVE_UP){
        return moveVertically(board,0,1);
    }
    else if (moveDirection == MOVE_DOWN){
        return moveVertically(board,3,-1);
    }
    else if (moveDirection == MOVE_LEFT){
        return moveHorizontally(board, 0, 1);
    }
    else if (moveDirection == MOVE_RIGHT){
        return moveHorizontally(board, 3, -1);
    }
	return 0;
}

int Fib2584Ai::moveVertically(int board[4][4], int start, int offset){
    int score = 0;
	for (int col = 0 ; col < 4 ; col++){
        int preTile = 0;
        int preRow = start;
        bool preIsMerged = false;
        for (int i = 0 ; i < 4 ; i++){
            int row = start + i*offset;
            if (board[row][col] == 0)
                continue;

            if (preTile == 0){
                preTile = board[row][col];
                board[row][col] = 0;
                board[preRow][col] = preTile;
            }
            else{
                if ( canMerge(preTile,board[row][col]) && !preIsMerged){
                    preTile = preTile+board[row][col];
					score += preTile;
                    board[row][col] = 0;
                    board[preRow][col] = preTile;
                    preIsMerged = true;
                }
                else{
                    preTile = board[row][col];
                    board[row][col] = 0;
                    preRow += offset;
                    board[preRow][col] = preTile;
                    preIsMerged = false;
                }
            }
        }
    }
	return score;
}

int Fib2584Ai::moveHorizontally(int board[4][4], int start, int offset){
    int score = 0;
	for (int row = 0 ; row < 4 ; row++){
        int preTile = 0;
        int preCol = start;
        bool preIsMerged = false;
        for (int i = 0 ; i < 4 ; i++){
            int col = start+ i*offset;
            if (board[row][col] == 0)
                continue;

            if (preTile == 0){
                preTile = board[row][col];
                board[row][col] = 0;
                board[row][preCol] = preTile;
            }
            else{
                if (canMerge(preTile,board[row][col]) && !preIsMerged){
                    preTile = preTile+board[row][col];
					score += preTile;
                    board[row][col] = 0;
                    board[row][preCol] = preTile;
                    preIsMerged = true;
                }
                else{
                    preTile = board[row][col];
                    board[row][col] = 0;
                    preCol += offset;
                    board[row][preCol] = preTile;
                    preIsMerged = false;
                }
            }
        }
    }
	return score;
}

void Fib2584Ai::copyBoard(int oldBoard[4][4], int newBoard[4][4]){
    for (int i = 0 ; i <4 ; i++){
        for (int j = 0 ; j < 4 ; j++){
            newBoard[i][j] = oldBoard[i][j];
        }
    }
}

string Fib2584Ai::printBoard(int board[4][4]){
    int i,j;
	string str;
	char tmp[100];
    for (i = 0 ; i < 4 ; i++){
        for (j = 0 ; j < 4 ; j++){
            sprintf(tmp,"%5d",board[i][j]);
			str += tmp;
        }
        str += '\n';
    }
	return str;
}

bool Fib2584Ai::canMerge(int num1,int num2){
    if (num1 == 0 || num2 == 0)
        return false;
    int fib = num1 + num2;
    for (int i = 1 ; i < 32 ; i++){
        if (fib == GameBoard::fibonacci_[i])
            return true;
    }
    return false;
}

bool Fib2584Ai::isSame(int board1[4][4], int board2[4][4]){
    for (int i = 0 ; i < 4 ; i++){
        for (int j = 0 ; j < 4 ; j++){
            if (board1[i][j] != board2[i][j])
                return false;
        }
    }
    return true;
}

int Fib2584Ai::getFibIndex(int number){
    for (int i = 0 ; i < 32 ; i++){
        if (number == GameBoard::fibonacci_[i])
            return i;
    }
    printf("Invalid Fib number : %d\n",number);
    exit(1);
    return 0;
}

int Fib2584Ai::reverseTuple(int tuple){
    int mask = 0xf8000, offset = -15;
    int i;
    int result = 0;
    for (i = 0 ; i < 4 ; i++){
        if (offset < 0)
            result |= (tuple&(mask)) >> -offset ;
        else
           result |= (tuple&(mask)) << offset ;
        mask = mask >> 5;
        offset += 10;
    }
    return result;
}
/**
	tuple [1,5,6,3] and [3,6,5,1] are the same, and they are both mapped to the SMALLER one
*/
int Fib2584Ai::getTupleIndex(int tuple){
	int reverse = reverseTuple(tuple);
	return (tuple < reverse) ? tuple : reverse;
}

/**
	update "valueTable"
*/
double Fib2584Ai::runBackwardPropagation(){

	double sum = 0;
	int count =0;
	bool isLast = true;
	double preScore = 0;
	
	while (recordfeature.size() > 0){
		BoardFeature thisFeature = recordfeature.back();
		recordfeature.pop_back();
		if (isLast){
			isLast = false;
			preScore = 0;
		}
		else{
			double delta = alpha*(preScore - thisFeature.getBoardScore(valueTable));
			sum += delta;
			count += 1;
			for (int i = 0 ; i < 4 ; i++){
				valueTable[OUTER][thisFeature.outerFeature[i]] += delta;
				valueTable[INNER][thisFeature.innerFeature[i]] += delta;
			}
			
			preScore = thisFeature.mergeScore + thisFeature.getBoardScore(valueTable);
		}
	}
	
	return (count==0) ? 0 : sum/count;
}

Fib2584Ai::BoardFeature::BoardFeature(int board[4][4], double score){
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

double Fib2584Ai::BoardFeature::getBoardScore(double **valueTable){
	int score = 0;
	for (int i = 0 ; i < 4 ; i++){
		score += valueTable[OUTER][outerFeature[i]] + valueTable[INNER][innerFeature[i]];
	}
	return score;
}