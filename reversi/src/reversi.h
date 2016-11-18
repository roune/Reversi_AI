#ifndef REVERSI_H
#define REVERSI_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <regex.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>

#define PROG_VERSION 2
#define PROG_SUBVERSION 0
#define PROG_REVISION 0

#define MIN_BOARD_SIZE 2
#define MAX_BOARD_SIZE 8

#define BLACK_STONE 'X'
#define WHITE_STONE 'O'
#define EMPTY_STONE '_'

typedef enum { false, true } bool;

typedef struct {
	size_t black;
	size_t white;
} score_t;

typedef struct {
	size_t row;
	size_t column;
} move_t;

typedef struct {
	size_t size;
	uint64_t black;
	uint64_t white;
} bitboard_t;

typedef struct {
	char player;
	bitboard_t board;
} state_t;

typedef struct {
	char* row;
	size_t width;
} row_t;


char read_column(char * line);

char* trim_white_spaces(char* input);

row_t read_row(char * line, int num_line);

char read_current_player(char * line);

static state_t board_init(size_t width);

static void board_delete(char** board);

static score_t board_score(state_t state);

static void board_print(state_t state);

static void board_save(state_t state);

static state_t board_load(char * filename);

int is_there_comment(char * line);

int is_first_line_correct(char * line);

int is_line_correct(char * line);

char read_column(char * line);

int is_move_valid(state_t state, move_t move);

int is_up_valid(state_t state,int x,int y,char player);

int up_valid(state_t state,int x,int y,char player);

int is_down_valid(state_t state,int x,int y,char player);

int down_valid(state_t state,int x,int y,char player);

int is_right_valid(state_t state,int x,int y,char player);

int right_valid(state_t state,int x,int y,char player);

int is_left_valid(state_t state,int x,int y,char player);

int left_valid(state_t state,int x,int y,char player);

int is_up_left_valid(state_t state,int x,int y,char player);

int up_left_valid(state_t state,int x,int y,char player);

int is_up_right_valid(state_t state,int x,int y,char player);

int up_right_valid(state_t state,int x,int y,char player);

int is_down_left_valid(state_t state,int x,int y,char player);

int down_left_valid(state_t state,int x,int y,char player);

int is_down_right_valid(state_t state,int x,int y,char player);

int down_right_valid(state_t state,int x,int y,char player);

int count_valid_moves(state_t state);

int do_the_move(state_t state, move_t move);

move_t read_move(char * string);

#endif