#include "Fib2584Ai.h"


Fib2584Ai::Fib2584Ai()
{
}

void Fib2584Ai::initialize(int argc, char* argv[])
{
	srand(time(NULL));
	return;
}


MoveDirection Fib2584Ai::generateMove(int board[4][4])
{
    int nextBoard[4][4][4];
    MoveDirection direction[] = {MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT};
    char dir[][10] = {"UP", "DOWN", "LEFT", "RIGHT"};
    int maxScore = -1;
    int maxIndex = 0;

    //printBoard(board);
    for (int i = 0 ; i < 4 ; i++){
        copyBoard(board,nextBoard[i]);
        moveTile(direction[i],nextBoard[i]);

        if ( !isSame(board,nextBoard[i])){
            //printBoard(nextBoard[i]);
            //printf("%s score = %d\n",dir[i],getScore(nextBoard[i]));
            if ( maxScore < getScore(nextBoard[i]) ){
                maxScore = getScore(nextBoard[i]);
                maxIndex = i;
            }
        }
    }

	//MoveDirection randomMove = static_cast<MoveDirection>(rand() % 4);
	//printf(">>%s\n",dir[maxIndex]);
	return direction[maxIndex];
}

void Fib2584Ai::gameOver(int board[4][4], int iScore)
{
	return;
}

/**********************************
You can implement any additional functions
you may need.
**********************************/

void Fib2584Ai::moveTile(MoveDirection moveDirection,int board[4][4]){

    if (moveDirection == MOVE_UP){
        moveVertically(board,0,1);
    }
    else if (moveDirection == MOVE_DOWN){
        moveVertically(board,3,-1);
    }
    else if (moveDirection == MOVE_LEFT){
        moveHorizontally(board, 0, 1);
    }
    else if (moveDirection == MOVE_RIGHT){
        moveHorizontally(board, 3, -1);
    }
}

void Fib2584Ai::moveVertically(int board[4][4], int start, int offset){
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
}

void Fib2584Ai::moveHorizontally(int board[4][4], int start, int offset){
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
}

void Fib2584Ai::copyBoard(int oldBoard[4][4], int newBoard[4][4]){
    for (int i = 0 ; i <4 ; i++){
        for (int j = 0 ; j < 4 ; j++){
            newBoard[i][j] = oldBoard[i][j];
        }
    }
}

void Fib2584Ai::printBoard(int board[4][4]){
    int i,j;

    for (i = 0 ; i < 4 ; i++){
        for (j = 0 ; j < 4 ; j++){
            printf("%5d",board[i][j]);
        }
        putchar('\n');
    }
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

int Fib2584Ai:: getScore(int board[4][4]){
    return getEmptyBlockCount(board)*10000 + getMergeCount(board)*100 + (16-getBorderAverage(board));
}

int Fib2584Ai::getEmptyBlockCount(int board[4][4]){
    int emptyCount = 0;
    for (int i = 0 ; i < 4 ; i++){
        for (int j = 0 ;  j < 4 ; j++){
            if (board[i][j] == 0)
                emptyCount += 1;
        }
    }
    return emptyCount;
}

int Fib2584Ai::getLongestSequence(int board[4][4]){

    int countUp[4][4];
    int countDown[4][4];
    int index[4][4];
    int longestSequence = 1;

    for (int i = 0 ; i < 4 ; i++){
        for (int j = 0 ; j < 4 ; j++){
            index[i][j] = getFibIndex(board[i][j]);
            countUp[i][j] = 1;
            countDown[i][j] = 1;
        }
    }

    for (int i = 0 ; i < 4 ; i++){
        for (int j = 0 ; j < 4 ; j++){
            if (board[i][j] ==0)
                continue;
            /* top less than 1*/
            if ( i>0 && index[i-1][j] != 0 && index[i-1][j] == index[i][j] - 2 ){
                if (countDown[i][j] < countDown[i-1][j] +1)
                    countDown[i][j] = countDown[i-1][j] +1;
            }
            /* left less than 1*/
            if ( j>0 && index[i][j-1] != 0 && index[i][j-1] == index[i][j] - 2){
                if (countDown[i][j] < countDown[i][j-1] +1)
                    countDown[i][j] = countDown[i][j-1] +1;
            }

            /* top more than 1*/
            if ( i>0 && index[i-1][j] != 0 && index[i-1][j] == index[i][j] + 2){
                if (countUp[i][j] < countUp[i-1][j] +1)
                    countUp[i][j] = countUp[i-1][j] +1;
            }
            /* left more than 1*/
            if ( j>0 && index[i][j-1] != 0 && index[i][j-1] == index[i][j] + 2){
                if (countUp[i][j] < countUp[i][j-1] +1)
                    countUp[i][j] = countUp[i][j-1] +1;
            }
        }
    }

    for (int i = 0 ; i < 4 ; i++){
        for (int j = 0 ; j < 4 ; j++){
            if (longestSequence < countDown[i][j])
                longestSequence = countDown[i][j];
            if(longestSequence < countUp[i][j])
                longestSequence = countUp[i][j];
        }
    }

    return longestSequence;
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

int Fib2584Ai::getMergeCount(int board[4][4]){
    int countMerge = 0;
    for (int i = 0 ; i < 4 ; i++){
        for (int j = 0 ; j < 4 ; j++){
            if ( i > 0 && canMerge(board[i][j],board[i-1][j]) )
                countMerge += 1;
            if ( j > 0 && canMerge(board[i][j], board[i][j-1]) )
                countMerge += 1;
        }
    }
    return countMerge;
}

int Fib2584Ai::getBorderAverage(int board[4][4]){
    int x[]={-1,1,0,0};
    int y[]={0,0,-1,1};

    int sum = 0, countTile = 0;

    for (int i = 0 ; i < 4 ; i++){
        for (int j = 0 ; j < 4 ; j++){
            if (board[i][j] == 0)
                continue;
            for (int k = 0 ; k < 4 ; k++){
                int nextX = i + x[k];
                int nextY = j + y[k];
                if ( nextX >= 0 && nextX < 4 && nextY >= 0 && nextY < 4){
                    if (board[nextX][nextY] == 0){
                        countTile += 1;
                        sum += board[i][j];
                        break;
                    }
                }
            }
        }
    }

    if (countTile == 0)
        return 0;
    else
        return sum/countTile;
}
