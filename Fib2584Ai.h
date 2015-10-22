#ifndef __FIB2584AI_H__

#define __FIB2584AI_H__

#include <cstdlib>
#include <ctime>
#include "Fib2584/MoveDirection.h"
#include "Fib2584/GameBoard.h"
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <string>
#include <iostream>

#define INNER 0
#define OUTER 1
#define TABLE_SIZE 1048576 //2^20

class Fib2584Ai
{
public:
	Fib2584Ai();
	~Fib2584Ai();
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
	class BoardFeature;
    static int moveTile(MoveDirection,int [4][4]);
    static int moveVertically(int [4][4], int, int);
    static int moveHorizontally(int [4][4], int, int);
    static string printBoard(int [4][4]);
    static bool isSame(int[4][4], int[4][4]);
    static int getFibIndex(int);
    static bool canMerge(int,int);
    static void copyBoard(int [4][4], int[4][4]);
	static int reverseTuple(int);
	static int getTupleIndex(int);
	MoveDirection chooseNextDirection(int [4][4]);
	void saveValueTable();
	double runBackwardPropagation();
	
	int countGame;
	double alpha;
	double **valueTable;
	bool isTraining;
	std::list<Fib2584Ai::BoardFeature> recordfeature;
};
#include "BoardFeature.h"

#endif
