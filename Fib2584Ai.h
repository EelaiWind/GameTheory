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

#define EXTRA_TABLE_SIZE 262144 //2^18
#define EXTRA_TABLE_FILE "_extra_table"

#define WEIGHT_TABLE_SIZE 64000000 //20^6
#define OUTER_AXE_TABLE_FILE "_inner_axe_table"
#define INNER_AXE_TABLE_FILE "_outer_axe_table"
#define CORNER_RECT_TABLE_FILE "_corner_rect_table"
#define CENTER_RECT_TABLE_FILE "_center_rect_table"

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
    static int generateNextBoard(int [4][4], int, int, bool);
    static string toString(const int [4][4]);
    static bool isSame(const int[4][4], const int[4][4]);
    static int getFibIndex(int);
    static bool canMerge(int,int);
    static void copyBoard(const int [4][4], int[4][4]);
	static int reverseTuple(int);
	static int getTupleIndex(int);
	MoveDirection chooseNextDirection(const int [4][4]);
	void saveweightTable();
	float updateWeightTable();
	
	int countGame;
	float alpha;
	float *extra_table, *outer_axe_table, *inner_axe_table, *corner_rect_table, *center_rect_table;
	bool isTraining;
	std::list<Fib2584Ai::BoardFeature> recordfeature;
};
#include "BoardFeature.h"

#endif
