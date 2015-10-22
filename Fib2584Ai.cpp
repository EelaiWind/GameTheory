#include "Fib2584Ai.h"

Fib2584Ai::Fib2584Ai()
{
	alpha = 0.0001;
	isTraining= false;
	countGame = 0;
	
	valueTable = new double*[2];
	
	for(int i = 0; i < 2; ++i)
		valueTable[i] = new double[TABLE_SIZE];
	
	FILE *file = fopen("valueTable.txt","r");
	if (file == NULL){
		for (int i = 0 ; i < TABLE_SIZE ; i++){
			if ( i <= reverseTuple(i)){
				valueTable[OUTER][i] = 0;
				valueTable[INNER][i] = 0;
			}
		}
	}
	else{
		int index,count = 0;
		double outerWeight,innerWeight;
		while(true){
			count += 1;
			if ( fscanf(file,"%d %lf %lf\n",&index, &outerWeight, &innerWeight) == EOF)
				break;
			valueTable[OUTER][index] = outerWeight;
			valueTable[INNER][index] = innerWeight;
		}
		fclose(file);
	}
}

Fib2584Ai::~Fib2584Ai(){
	if (isTraining)
		saveValueTable();
}

void Fib2584Ai::initialize(int argc, char* argv[])
{	
	double tmp;
	if (argc >= 3 && sscanf(argv[2],"%lf",&tmp) == 1){
		printf("Enable Training mode...\n");
		isTraining = true;
		printf("training rate = %lf\n",alpha);
		alpha = tmp;
	}
	srand(time(NULL));
	return;
}


MoveDirection Fib2584Ai::generateMove(int board[4][4])
{
	/** choose next direction*/
    MoveDirection dir = chooseNextDirection(board);
	
	int score = moveTile(dir,board);
	if (isTraining){
		BoardFeature feature(board, score);
		recordfeature.push_back(feature);
	}
	return dir;
}

void Fib2584Ai::gameOver(int board[4][4], int iScore)
{	
	if (isTraining){
		/** add dead board to the list*/
		BoardFeature feature(board, 0);
		recordfeature.push_back(feature);
		cout << "count = "<< ++countGame <<", avg delta = " << runBackwardPropagation() << endl;
	}
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
			double delta = (preScore - thisFeature.getBoardScore(valueTable));
			sum += delta;
			delta *= alpha;
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

/**
	choose the next move direction based on the current board, this function also avoid
	unchanged moves.
	DON'T modified "board[4][4]" in this function. Instead, manipulate the copy of "board[4][4]"
*/
MoveDirection Fib2584Ai::chooseNextDirection(int board[4][4]){
	const MoveDirection dir[] = {MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT};
	int maxScore = -2147483648, maxIndex = 0;
	
	for (int i = 0 ; i < 4 ; i++){
		int nextBoard[4][4];
		copyBoard(board,nextBoard);
		int mergeScore = moveTile(dir[i],nextBoard);
		if (isSame(nextBoard,board)){
			continue;
		}
		BoardFeature nextFeature = BoardFeature(nextBoard,mergeScore);
		int nextScore = nextFeature.mergeScore + nextFeature.getBoardScore(valueTable);
		if (maxScore < nextScore){
			maxScore = nextScore;
			maxIndex = i;
		}
	}
	
	return dir[maxIndex];
}
/**
	save feature weight to file;
*/
void Fib2584Ai::saveValueTable(){
	
	FILE *file = fopen("valueTable.txt","w");
	for (int i = 0 ; i < TABLE_SIZE ; i++){
		if ( i <= reverseTuple(i))
			fprintf(file,"%d %lf %lf\n",i, valueTable[OUTER][i], valueTable[INNER][i]);
	}
	fclose(file);
}
