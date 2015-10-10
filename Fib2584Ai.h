#ifndef __FIB2584AI_H__

#define __FIB2584AI_H__

#include <cstdlib>
#include <ctime>
#include "Fib2584/MoveDirection.h"
#include "Fib2584/GameBoard.h"
#include <stdio.h>
#include <stdlib.h>

class Fib2584Ai
{
public:
	Fib2584Ai();
	// initialize ai
	void initialize(int argc, char* argv[]);
	// generate one move
	MoveDirection generateMove(int board[4][4]);
	// do some action when game over
	void gameOver(int board[4][4], int iScore);

	/**********************************
	You can implement any additional functions
	or define any variables you may need.
	**********************************/
private:
    static void moveTile(MoveDirection,int [4][4]);
    static void moveVertically(int [4][4], int, int);
    static void moveHorizontally(int [4][4], int, int);
    static void printBoard(int [4][4]);
    static bool isSame(int[4][4], int[4][4]);
    static int getScore(int[4][4]);
    static int getEmptyBlockCount(int[4][4]);
    static int getFibIndex(int);
    static int getLongestSequence(int [4][4]);
    static int getMergeCount(int [4][4]);
    static bool canMerge(int,int);
    static void copyBoard(int [4][4], int[4][4]);
    static int getBorderAverage(int[4][4]);
};

#endif
