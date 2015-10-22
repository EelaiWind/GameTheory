#include "Fib2584Ai.h"

Fib2584Ai::Fib2584Ai()
{
	alpha = 0.0001;
	isTraining= false;
	countGame = 0;
	
	weightTable = new double*[2];
	
	for(int i = 0; i < 2; ++i)
		weightTable[i] = new double[TABLE_SIZE];
	
	FILE *file = fopen(WEIGHT_TABLE_FILE,"r");
	if (file == NULL){
		for (int i = 0 ; i < TABLE_SIZE ; i++){
			if ( i <= reverseTuple(i)){
				weightTable[OUTER][i] = 0;
				weightTable[INNER][i] = 0;
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
			weightTable[OUTER][index] = outerWeight;
			weightTable[INNER][index] = innerWeight;
		}
		fclose(file);
	}
}

Fib2584Ai::~Fib2584Ai(){
	if (isTraining)
		saveweightTable();
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
		cout << "count = "<< ++countGame <<", avg delta = " << updateWeightTable() << endl;
	}
	return;
}

/**********************************
You can implement any additional functions
you may need.
**********************************/

/**
	@arguments
		moveDirection: next move direction
		board: game board
	@return 
		merge score earned by this move
		
	this function will change the content of "board[4][4]"
*/
int Fib2584Ai::moveTile(MoveDirection moveDirection,int board[4][4]){

    if (moveDirection == MOVE_UP){
        return generateNextBoard(board,0,1,false);
    }
    else if (moveDirection == MOVE_DOWN){
        return generateNextBoard(board,3,-1,false);
    }
    else if (moveDirection == MOVE_LEFT){
        return generateNextBoard(board, 0, 1,true);
    }
    else if (moveDirection == MOVE_RIGHT){
        return generateNextBoard(board, 3, -1,true);
    }
	return 0;
}

/**
	@arguments
		board: game board
		start: start index
		offset: increase or decrease index
		isTransport: reverse game board by its diagonal
	
	@return
		merge score earned by this move
	
	
	this function will change the content of "board[4][4]"
	
	for UP and LEFT index should be 0,1,2,3 	(start = 0, offset = 1)
	for DOWN and RIGHT index should be 3,2,1,0 	(start = 3, offset = -1)
	
	**NOTE**
	this function only do column wise manipulation,
	so be sure to transport the board if direction is LEFT or RIGHT
*/
int Fib2584Ai::generateNextBoard(int board[4][4], int start, int offset, bool isTransport){
    int score = 0;
	int *tilePointer[4][4];
	
	if (isTransport){
		for (int i = 0 ; i < 4;i++){
			for ( int j = 0 ; j < 4 ; j++){
				tilePointer[i][j] = &board[j][i];
			}
		}
	}
	else{
		for (int i = 0 ; i < 4;i++){
			for ( int j = 0 ; j < 4 ; j++){
				tilePointer[i][j] = &board[i][j];
			}
		}
	}
	
	for (int col = 0 ; col < 4 ; col++){
        int preTile = 0;
        int preRow = start;
        bool preIsMerged = false;
        for (int i = 0 ; i < 4 ; i++){
            int row = start + i*offset;
            if (*tilePointer[row][col] == 0)
                continue;

            if (preTile == 0){
                preTile = *tilePointer[row][col];
                *tilePointer[row][col] = 0;
                *tilePointer[preRow][col] = preTile;
            }
            else{
                if ( canMerge(preTile,*tilePointer[row][col]) && !preIsMerged){
                    preTile = preTile + *tilePointer[row][col];
					score += preTile;
                    *tilePointer[row][col] = 0;
                    *tilePointer[preRow][col] = preTile;
                    preIsMerged = true;
                }
                else{
                    preTile = *tilePointer[row][col];
                    *tilePointer[row][col] = 0;
                    preRow += offset;
                    *tilePointer[preRow][col] = preTile;
                    preIsMerged = false;
                }
            }
        }
    }
	return score;
}

/**
	@arguments
		oldBoard: board to be copied
		newBoard: board where "oldBoard[4][4]" will be copied to 
	
	this function will modify the content of "newBoard[4][4]"
*/
void Fib2584Ai::copyBoard(int oldBoard[4][4], int newBoard[4][4]){
    for (int i = 0 ; i <4 ; i++){
        for (int j = 0 ; j < 4 ; j++){
            newBoard[i][j] = oldBoard[i][j];
        }
    }
}

/**
	@arguments
		board: game board
		
	@return
		a string representing the board
*/
string Fib2584Ai::toString(int board[4][4]){
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

/**
	@arguments
		index1: tile index
		index2: tile index
	
	@return
		whether 2 tiles can merge into a bigger one
*/
bool Fib2584Ai::canMerge(int index1,int index2){
    if (index1 == 0 || index2 == 0)
        return false;
    int fib = index1 + index2;
    for (int i = 1 ; i < 32 ; i++){
        if (fib == GameBoard::fibonacci_[i])
            return true;
    }
    return false;
}

/**
	@arguments
		board1: game board1
		board2: game board2
	
	@return 
		whether 2 game board is exactly the same
*/
bool Fib2584Ai::isSame(int board1[4][4], int board2[4][4]){
    for (int i = 0 ; i < 4 ; i++){
        for (int j = 0 ; j < 4 ; j++){
            if (board1[i][j] != board2[i][j])
                return false;
        }
    }
    return true;
}

/**
	@arguments
		number : a fib number
	
	@return
		the index of that fib number
	
	@error
		if "number" is not a fib number, program will halt
*/
int Fib2584Ai::getFibIndex(int number){
    for (int i = 0 ; i < 32 ; i++){
        if (number == GameBoard::fibonacci_[i])
            return i;
    }
    printf("Invalid Fib number : %d\n",number);
    exit(1);
    return 0;
}

/**
	@arguments
		tuple: a number representing a 4-tuple
	
	@return
		a number representing the reverse of "tuple"
		
	<example> 	[2,14,7,1] 				=> [1,7,14,2]
				00010_01110_00111_00001 => 00001_00111_01110_00010
*/
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
	@arguments
		tuple: a number representing a 4-tuple
	
	@return
		the index of the weight table corresponding to that 4-tuple
	
	for 2 symmetry 4-tuple, always map the smaller index 
	
	<example> 
	tuple [1,5,6,3] and [3,6,5,1] are the same, and they are both mapped to the 00001_00101_00110_00011
*/
int Fib2584Ai::getTupleIndex(int tuple){
	int reverse = reverseTuple(tuple);
	return (tuple < reverse) ? tuple : reverse;
}

/**
	@return
		average delta during updating
	
	
	update the weight according to the merge score from the back to the front
	the "recordfeature" list will become empty after updating
	
	V(ST) = 0
	delta = alpha * [ r' + V(S') - V(S)]
*/
double Fib2584Ai::updateWeightTable(){

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
			double delta = (preScore - thisFeature.getBoardScore(weightTable));
			sum += delta;
			delta *= alpha;
			count += 1;
			for (int i = 0 ; i < 4 ; i++){
				weightTable[OUTER][thisFeature.outerFeature[i]] += delta;
				weightTable[INNER][thisFeature.innerFeature[i]] += delta;
			}
			preScore = thisFeature.mergeScore + thisFeature.getBoardScore(weightTable);
		}
	}
	
	return (count==0) ? 0 : sum/count;
}

/**
	@arguments
		board: game board
	
	@return
		a direction which maximize the next board score
		
	this function will avoid unchanged moves
	
	**NOTE**
	DON'T modified "board[4][4]" in this function. Instead, manipulate the copy of "board[4][4]"
*/
MoveDirection Fib2584Ai::chooseNextDirection(int board[4][4]){
	const MoveDirection dir[] = {MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT};
	//const string dirString[] = {"MOVE_UP", "MOVE_DOWN", "MOVE_LEFT", "MOVE_RIGHT"};
	int maxScore = -2147483648, maxIndex = 0;
	
	for (int i = 0 ; i < 4 ; i++){
		int nextBoard[4][4];
		copyBoard(board,nextBoard);
		int mergeScore = moveTile(dir[i],nextBoard);
		if (isSame(nextBoard,board)){
			continue;
		}
		BoardFeature nextFeature = BoardFeature(nextBoard,mergeScore);
		int nextScore = nextFeature.mergeScore + nextFeature.getBoardScore(weightTable);
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
void Fib2584Ai::saveweightTable(){
	
	FILE *file = fopen(WEIGHT_TABLE_FILE,"w");
	for (int i = 0 ; i < TABLE_SIZE ; i++){
		if ( i <= reverseTuple(i))
			fprintf(file,"%d %lf %lf\n",i, weightTable[OUTER][i], weightTable[INNER][i]);
	}
	fclose(file);
}
