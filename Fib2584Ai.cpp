#include "Fib2584Ai.h"

Fib2584Ai::Fib2584Ai()
{
	alpha = 0.0001;
	isTraining= false;
	countGame = 0;
	
	outer_axe_table = new float[WEIGHT_TABLE_SIZE];
	inner_axe_table = new float[WEIGHT_TABLE_SIZE];
	corner_rect_table = new float[WEIGHT_TABLE_SIZE];
	center_rect_table = new float[WEIGHT_TABLE_SIZE];
	extra_table = new float[EXTRA_TABLE_SIZE];
	
	printf("loading weight table...");
	FILE *file = fopen(OUTER_AXE_TABLE_FILE,"rb");
	if (file == NULL){
		for (int i = 0 ; i < WEIGHT_TABLE_SIZE ; i++){
			outer_axe_table[i] = 0;
		}
	}
	else{
		fread(outer_axe_table, sizeof(float), WEIGHT_TABLE_SIZE, file);
	}
	fclose(file);

	file = fopen(INNER_AXE_TABLE_FILE,"rb");
	if (file == NULL){
		for (int i = 0 ; i < WEIGHT_TABLE_SIZE ; i++){
			inner_axe_table[i] = 0;
		}
	}
	else{
		fread(inner_axe_table, sizeof(float), WEIGHT_TABLE_SIZE, file);
	}
	fclose(file);

	file = fopen(CORNER_RECT_TABLE_FILE,"rb");
	if (file == NULL){
		for (int i = 0 ; i < WEIGHT_TABLE_SIZE ; i++){
			corner_rect_table[i] = 0;
		}
	}
	else{
		fread(corner_rect_table, sizeof(float), WEIGHT_TABLE_SIZE, file);
	}
	fclose(file);

	file = fopen(CENTER_RECT_TABLE_FILE,"rb");
	if (file == NULL){
		for (int i = 0 ; i < WEIGHT_TABLE_SIZE ; i++){
			center_rect_table[i] = 0;
		}
	}
	else{
		fread(center_rect_table, sizeof(float), WEIGHT_TABLE_SIZE, file);
	}
	fclose(file);

	printf("[finish]\n");

	printf("loading extra table...");
	file = fopen(EXTRA_TABLE_FILE,"rb");
	if (file == NULL){
		for (int i = 0 ; i < EXTRA_TABLE_SIZE ; i++){
			extra_table[i] = 0;
		}
	}
	else{
		fread(extra_table, sizeof(float), EXTRA_TABLE_SIZE, file);
	}
	fclose(file);
	printf("[finish]\n");
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
		try {
			BoardFeature feature(board, score);
			recordfeature.push_back(feature);
		}
		catch (std::exception &e){
			std::cerr << "error:" << e.what()<<endl;
			exit(1);
		}
	}
	return dir;
}

void Fib2584Ai::gameOver(int board[4][4], int iScore)
{	
	if (isTraining){
		// add dead board to the list
		BoardFeature feature(board, 0);
		recordfeature.push_back(feature);

		updateWeightTable();
		countGame+=1;
		if (countGame % 100 == 0){
			printf("%d\n",countGame);
		}
		
		if (countGame%100000 == 0){	
			saveweightTable();
		}
	}
	if (IGNORE)
			printf("### IGNORE is true !###\n");
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
void Fib2584Ai::copyBoard(const int oldBoard[4][4], int newBoard[4][4]){
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
string Fib2584Ai::toString(const int board[4][4]){
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
bool Fib2584Ai::isSame(const int board1[4][4],const int board2[4][4]){
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
    for (int i = 0 ; i < 20 ; i++){
        if (number == GameBoard::fibonacci_[i])
            return i;
    }
    //printf("Invalid Fib number : %d\n",number);
    //exit(1);
    return 19;
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
float Fib2584Ai::updateWeightTable(){

	float sum = 0;
	int count =0;
	bool isLast = true;
	float preScore = 0;
	
	while (recordfeature.size() > 0){
		BoardFeature thisFeature = recordfeature.back();
		recordfeature.pop_back();
		if (isLast){
			isLast = false;
			preScore = 0;
		}
		else{
			float delta = preScore - thisFeature.getBoardScore(outer_axe_table, inner_axe_table, corner_rect_table, center_rect_table, extra_table);
			sum += delta;
			delta *= alpha;
			count += 1;
			for (int i = 0 ; i < 8 ; i++){
				outer_axe_table[thisFeature.outer_axe_feature[i]] += delta;
				inner_axe_table[thisFeature.inner_axe_feature[i]] += delta;
				corner_rect_table[thisFeature.corner_rect_feature[i]] += delta;
			}
			for (int i=0; i < 4 ; i++){
				center_rect_table[thisFeature.center_rect_feature[i]] += delta;
			}
			if (!IGNORE){
				extra_table[thisFeature.extraFeature] += delta;
			}
			preScore = thisFeature.mergeScore + thisFeature.getBoardScore(outer_axe_table, inner_axe_table, corner_rect_table, center_rect_table, extra_table);
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
MoveDirection Fib2584Ai::chooseNextDirection(const int board[4][4]){
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
		int nextScore = nextFeature.getBoardScore(outer_axe_table, inner_axe_table, corner_rect_table, center_rect_table, extra_table);
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
	
	printf("saving weight tables...");
	FILE *file = fopen(OUTER_AXE_TABLE_FILE,"wb");
	fwrite(outer_axe_table, sizeof(float), WEIGHT_TABLE_SIZE, file);
	fclose(file);
	
	file = fopen(INNER_AXE_TABLE_FILE,"wb");
	fwrite(inner_axe_table, sizeof(float), WEIGHT_TABLE_SIZE, file);
	fclose(file);

	file = fopen(CORNER_RECT_TABLE_FILE,"wb");
	fwrite(corner_rect_table, sizeof(float), WEIGHT_TABLE_SIZE, file);
	fclose(file);

	file = fopen(CENTER_RECT_TABLE_FILE,"wb");
	fwrite(center_rect_table, sizeof(float), WEIGHT_TABLE_SIZE, file);
	fclose(file);

	file = fopen(EXTRA_TABLE_FILE,"wb");
	fwrite(extra_table, sizeof(float), EXTRA_TABLE_SIZE, file);
	fclose(file);
	printf("[done]\n");
}

