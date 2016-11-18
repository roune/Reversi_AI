#include "../include/bitboard.h"

uint8_t get_bit(uint64_t bits, int pos) {
   return (bits >> pos) & 0x01;
}

uint64_t set_bit(uint64_t bits, int pos, int value) {
   uint64_t mask = 1LL << pos;
   if (value)
       bits |= mask;
   else
       bits &= ~mask;
   return bits;
}

bitboard_t bb_new(size_t size) {
	bitboard_t board;
	
	board.size = size;
	board.black = 0;
	board.white = 0;
	return board;
}

bitboard_t bb_set(move_t move, state_t state) {
	int pos;
	bitboard_t board;
	board.size = state.board.size;
	pos = move.row * state.board.size + move.column;
	
	if (state.player == BLACK_STONE) {
		board.black = set_bit(state.board.black, pos, 1);
	} else {
		board.white = set_bit(state.board.white, pos, 1);
	}
	
	return board;
}

bitboard_t bb_init(size_t size) {
	bitboard_t board;
	
	board = bb_new(size);
	
	int first_row, first_col, first_empty, second_empty;
	
	first_row = size/2 - 1;
	first_col = size/2 - 1;
	first_empty = first_row * size + first_col;
	second_empty = (first_row + 1) * size + first_col;

	board.size = size;
	board.black = board.white = 0;
	board.white = set_bit(board.white, first_empty, 1);
	board.black = set_bit(board.black, first_empty + 1, 1);
	board.black = set_bit(board.black, second_empty, 1);
	board.white = set_bit(board.white, second_empty + 1, 1);
	
	return board;
}

void bb_print(bitboard_t board) {
	uint64_t full_board = 0;
	int i;
	char letter;
	printf("\t");
	for (i = 0; i < board.size; i++) {
		printf("%d ", i + 1);
	}
	
	full_board = board.white | board.black;
	
	for (i = 0; i < board.size * board.size; i++) {
		if (i % board.size == 0) {
			printf("\n");
			
			letter = 'A' + i/board.size;
			printf("%c\t", letter);
		}
		if (get_bit(full_board, i)) {
			if (get_bit(board.white, i)) {
				printf("O ");
			} else {
				printf("X ");
			}
		} else {
			printf("_ ");
		}
	}
	printf("\n");
}

score_t bb_score(bitboard_t board) {
	score_t score;
	size_t black, white;
	int i;
	black = white = 0;
	
	for (i = 0; i < 64; i++) {
		if (get_bit(board.black, i))
			black++;
		else if (get_bit(board.white, i))
			white++;
	}
	score.black = black;
	score.white = white;
	return score;
}

int one_dimension(int x, int y, size_t size) {
	return y*size + x;
}

/**
 * @brief 
 * @param player
 * @param black
 * @param white
 * @param x
 * @param y
 * @param size
 * @return If false, there move could not be done. Returns true
 */
bitboard_t change_t(char player, uint64_t black, uint64_t white, int x, int y, size_t size) {
	if (player == BLACK_STONE) {
		white = set_bit(white, one_dimension(x, y, size), 0);
		black = set_bit(black, one_dimension(x, y, size), 1);
	} else {
		black = set_bit(black, one_dimension(x, y, size), 0);
		white = set_bit(white, one_dimension(x, y, size), 1);
	}
	bitboard_t res;
	res.black = black;
	res.white = white;
	res.size = size;
	return res;
}

bitboard_t bb_up(move_t move, state_t state, bool change) {
	int x = move.row, y = move.column;
	char player = state.player;
	size_t size = state.board.size;
	bitboard_t board = state.board, error;
	error.size = 0;
	
	if (y <= 1) {
		return error;
	}
	
	if (player == BLACK_STONE) {
		if (get_bit(state.board.black, one_dimension(x, y-1, size)) == 1) {
			return error;
		} 
	} else {
		if (get_bit(state.board.white, one_dimension(x, y-1, size)) == 1) {
			return error;
		} 
	}
	
	for(y = y-1; y >= 0; y--) {
		if (player == BLACK_STONE) {
			if (get_bit(state.board.black, one_dimension(x, y, size)) == 1) {
				return state.board; // ENDS
			} else if (get_bit(state.board.white, one_dimension(x, y, size)) == 0) {					
				return error; // ERROR
			}
			if (change) {
				state.board = change_t(player, state.board.black, state.board.white, x, y, size);
			}
		} else {
			if (get_bit(state.board.white, one_dimension(x, y, size)) == 1) {
				return state.board; // ENDS
			} else if (get_bit(state.board.black, one_dimension(x, y, size)) != 1) {
				return error; // ERROR
			}
			if (change) {
				state.board = change_t(player, state.board.black, state.board.white, x, y, size);
			}
		}
	}
	return error;
}

bitboard_t bb_down(move_t move, state_t state, bool change) {
	int x = move.row, y = move.column;
	char player = state.player;
	size_t size = state.board.size;
	bitboard_t board = state.board, error;
	error.size = 0;
	
	if (y >= size - 2) {
		return error;
	}
	
	if (player == BLACK_STONE) {
		if (get_bit(state.board.black, one_dimension(x, y+1, size)) == 1) {
			return error;
		} 
	} else {
		if (get_bit(state.board.white, one_dimension(x, y+1, size)) == 1) {
			return error;
		} 
	}
	
	for(y = y+1; y <= size - 1; y++) {
		if (player == BLACK_STONE) {
			if (get_bit(state.board.black, one_dimension(x, y, size)) == 1) {
				return state.board; // ENDS
			} else if (get_bit(state.board.white, one_dimension(x, y, size)) == 0) {					
				return error; // ERROR
			}
			if (change) {
				state.board = change_t(player, state.board.black, state.board.white, x, y, size);
			}
		} else {
			if (get_bit(state.board.white, one_dimension(x, y, size)) == 1) {
				return state.board; // ENDS
			} else if (get_bit(state.board.black, one_dimension(x, y, size)) != 1) {
				return error; // ERROR
			}
			if (change) {
				state.board = change_t(player, state.board.black, state.board.white, x, y, size);
			}
		}
	}
	return error;
}

bitboard_t bb_right(move_t move, state_t state, bool change) {
	int x = move.row, y = move.column;
	char player = state.player;
	size_t size = state.board.size;
	bitboard_t board = state.board, error;
	error.size = 0;
	
	if (x >= size - 2) {
		return error;
	}
	
	if (player == BLACK_STONE) {
		if (get_bit(state.board.black, one_dimension(x+1, y, size)) == 1) {
			return error;
		} 
	} else {
		if (get_bit(state.board.white, one_dimension(x+1, y, size)) == 1) {
			return error;
		} 
	}
	
	for(x = x+1; x <= size - 1; x++) {
		if (player == BLACK_STONE) {
			if (get_bit(state.board.black, one_dimension(x, y, size)) == 1) {
				return state.board; // ENDS
			} else if (get_bit(state.board.white, one_dimension(x, y, size)) == 0) {					
				return error; // ERROR
			}
			if (change) {
				state.board = change_t(player, state.board.black, state.board.white, x, y, size);
			}
		} else {
			if (get_bit(state.board.white, one_dimension(x, y, size)) == 1) {
				return state.board; // ENDS
			} else if (get_bit(state.board.black, one_dimension(x, y, size)) != 1) {
				return error; // ERROR
			}
			if (change) {
				state.board = change_t(player, state.board.black, state.board.white, x, y, size);
			}
		}
	}
	return error;
}

bitboard_t bb_left(move_t move, state_t state, bool change) {
	int x = move.row, y = move.column;
	char player = state.player;
	size_t size = state.board.size;
	bitboard_t board = state.board, error;
	error.size = 0;
	
	if (x <= 1) {
		return error;
	}
	
	if (player == BLACK_STONE) {
		if (get_bit(state.board.black, one_dimension(x-1, y, size)) == 1) {
			return error;
		} 
	} else {
		if (get_bit(state.board.white, one_dimension(x-1, y, size)) == 1) {
			return error;
		} 
	}
	
	for(x = x-1; x >= 0; x--) {
		if (player == BLACK_STONE) {
			if (get_bit(state.board.black, one_dimension(x, y, size)) == 1) {
				return state.board; // ENDS
			} else if (get_bit(state.board.white, one_dimension(x, y, size)) == 0) {					
				return error; // ERROR
			}
			if (change) {
				state.board = change_t(player, state.board.black, state.board.white, x, y, size);
			}
		} else {
			if (get_bit(state.board.white, one_dimension(x, y, size)) == 1) {
				return state.board; // ENDS
			} else if (get_bit(state.board.black, one_dimension(x, y, size)) != 1) {
				return error; // ERROR
			}
			if (change) {
				state.board = change_t(player, state.board.black, state.board.white, x, y, size);
			}
		}
	}
	return error;
}

bitboard_t bb_up_right(move_t move, state_t state, bool change) {
	int x = move.row, y = move.column;
	char player = state.player;
	size_t size = state.board.size;
	bitboard_t board = state.board, error;
	error.size = 0;
	
	if (x >= size - 2 || y <= 1) {
		return error;
	}

	if (player == BLACK_STONE) {
		if (get_bit(state.board.black, one_dimension(x+1, y-1, size)) == 1) {
			return error;
		} 
	} else {
		if (get_bit(state.board.white, one_dimension(x+1, y-1, size)) == 1) {
			return error;
		} 
	}

	for(x = x+1, y = y-1; x <= size - 1 && y >= 0; x++, y--) {
		if (player == BLACK_STONE) {
			if (get_bit(state.board.black, one_dimension(x, y, size)) == 1) {
				return state.board; // ENDS
			} else if (get_bit(state.board.white, one_dimension(x, y, size)) == 0) {					
				return error; // ERROR
			}
			if (change) {
				state.board = change_t(player, state.board.black, state.board.white, x, y, size);
			}
		} else {
			if (get_bit(state.board.white, one_dimension(x, y, size)) == 1) {
				return state.board; // ENDS
			} else if (get_bit(state.board.black, one_dimension(x, y, size)) != 1) {
				return error; // ERROR
			}
			if (change) {
				state.board = change_t(player, state.board.black, state.board.white, x, y, size);
			}
		}
	}
	return error;
}

bitboard_t bb_up_left(move_t move, state_t state, bool change) {
	int x = move.row, y = move.column;
	char player = state.player;
	size_t size = state.board.size;
	bitboard_t board = state.board, error;
	error.size = 0;
	
	if (x <= 1 || y <= 1) {
		return error;
	}
	
	if (player == BLACK_STONE) {
		if (get_bit(state.board.black, one_dimension(x-1, y-1, size)) == 1) {
			return error;
		} 
	} else {
		if (get_bit(state.board.white, one_dimension(x-1, y-1, size)) == 1) {
			return error;
		} 
	}
	
	for(x = x-1, y = y-1; x >= 0 && y >= 0; x--, y--) {
		if (player == BLACK_STONE) {
			if (get_bit(state.board.black, one_dimension(x, y, size)) == 1) {
				return state.board; // ENDS
			} else if (get_bit(state.board.white, one_dimension(x, y, size)) == 0) {					
				return error; // ERROR
			}
			if (change) {
				state.board = change_t(player, state.board.black, state.board.white, x, y, size);
			}
		} else {
			if (get_bit(state.board.white, one_dimension(x, y, size)) == 1) {
				return state.board; // ENDS
			} else if (get_bit(state.board.black, one_dimension(x, y, size)) != 1) {
				return error; // ERROR
			}
			if (change) {
				state.board = change_t(player, state.board.black, state.board.white, x, y, size);
			}
		}
	}
	return error;
}

bitboard_t bb_down_right(move_t move, state_t state, bool change) {
	int x = move.row, y = move.column;
	char player = state.player;
	size_t size = state.board.size;
	bitboard_t board = state.board, error;
	error.size = 0;
	
	if (x >= size - 2 || y >= size - 2) {
		return error;
	}
	
	if (player == BLACK_STONE) {
		if (get_bit(state.board.black, one_dimension(x+1, y+1, size)) == 1) {
			return error;
		} 
	} else {
		if (get_bit(state.board.white, one_dimension(x+1, y+1, size)) == 1) {
			return error;
		} 
	}
	
	for(x = x+1, y = y+1; x <= size - 1 && y <= size - 1; x++, y++) {
		if (player == BLACK_STONE) {
			if (get_bit(state.board.black, one_dimension(x, y, size)) == 1) {
				return state.board; // ENDS
			} else if (get_bit(state.board.white, one_dimension(x, y, size)) == 0) {					
				return error; // ERROR
			}
			if (change) {
				state.board = change_t(player, state.board.black, state.board.white, x, y, size);
			}
		} else {
			if (get_bit(state.board.white, one_dimension(x, y, size)) == 1) {
				return state.board; // ENDS
			} else if (get_bit(state.board.black, one_dimension(x, y, size)) != 1) {
				return error; // ERROR
			}
			if (change) {
				state.board = change_t(player, state.board.black, state.board.white, x, y, size);
			}
		}
	}
	return error;
}

bitboard_t bb_down_left(move_t move, state_t state, bool change) {
	int x = move.row, y = move.column;
	char player = state.player;
	size_t size = state.board.size;
	bitboard_t board = state.board, error;
	error.size = 0;
	
	if (x <= 1 || y >= size - 2) {
		return error;
	}
	
	if (player == BLACK_STONE) {
		if (get_bit(state.board.black, one_dimension(x-1, y+1, size)) == 1) {
			return error;
		} 
	} else {
		if (get_bit(state.board.white, one_dimension(x-1, y+1, size)) == 1) {
			return error;
		} 
	}
	
	for(x = x-1, y = y+1; x >= 0 && y <= size - 1; x--, y++) {
		if (player == BLACK_STONE) {
			if (get_bit(state.board.black, one_dimension(x, y, size)) == 1) {
				return state.board; // ENDS
			} else if (get_bit(state.board.white, one_dimension(x, y, size)) == 0) {					
				return error; // ERROR
			}
			if (change) {
				state.board = change_t(player, state.board.black, state.board.white, x, y, size);
			}
		} else {
			if (get_bit(state.board.white, one_dimension(x, y, size)) == 1) {
				return state.board; // ENDS
			} else if (get_bit(state.board.black, one_dimension(x, y, size)) != 1) {
				return error; // ERROR
			}
			if (change) {
				state.board = change_t(player, state.board.black, state.board.white, x, y, size);
			}
		}
	}
	return error;
}

bitboard_t bb_move(move_t move, state_t state) {
	bool moved = false;
	size_t size = state.board.size;
	bitboard_t res;
	
	res.size = 0;
	
	if (move.column >= size || move.row >= size) {
		return res;
	}
	if (move.column < 0 || move.row < 0) {
		return res;
	}
	
	if (get_bit(state.board.black | state.board.white, one_dimension(move.row, move.column, state.board.size))) {
		return res;
	}
	
	if (bb_up(move, state, false).size) {
		state.board = bb_up(move, state, true);
		moved = true;
	}
	if (bb_down(move, state, false).size) {
		state.board = bb_down(move, state, true);
		moved = true;
	}
	if (bb_right(move, state, false).size) {
		state.board = bb_right(move, state, true);
		moved = true;
	}
	if (bb_left(move, state, false).size) {
		state.board = bb_left(move, state, true);
		moved = true;
	}
	if (bb_up_right(move, state, false).size) {
		state.board = bb_up_right(move, state, true);
		moved = true;
	}
	if (bb_up_left(move, state, false).size) {
		state.board = bb_up_left(move, state, true);
		moved = true;
	}
	if (bb_down_right(move, state, false).size) {
		state.board = bb_down_right(move, state, true);
		moved = true;
	}
	if (bb_down_left(move, state, false).size) {
		state.board = bb_down_left(move, state, true);
		moved = true;
	}
	
	if (!moved)
		return res;
	
	if (state.player == BLACK_STONE) {
		state.board.black = set_bit(state.board.black, one_dimension(move.row, move.column, state.board.size), 1);
	} else {
		state.board.white = set_bit(state.board.white, one_dimension(move.row, move.column, state.board.size), 1);
	}
	
	return state.board;
}

bitboard_t bb_moves(state_t state) {
	bitboard_t moves, res;
	move_t move;
	int x, y;
	//moves = state.board;
	moves.black = 0;
	moves.white = 0;
	moves.size = state.board.size;
	for (x = 0; x < state.board.size; x++) {
		move.row = (size_t) x;
		for (y = 0; y < state.board.size; y++) {
			move.column = (size_t) y;
			res = bb_move(move, state);
			if (res.size != 0) {
				if (state.player == BLACK_STONE) {
					moves.black = set_bit(moves.black, one_dimension(x, y, state.board.size), 1);
				} else {
					moves.white = set_bit(moves.white, one_dimension(x, y, state.board.size), 1);
				}
			}
		}
	}
	return moves;
}

int score_heuristic (state_t state) {
	score_t score;
	
	score = bb_score(state.board);
	
	if (state.player == BLACK_STONE) {
		return score.black;
	}
	return score.white;
}

int coin_parity_heuristic (state_t state) {
	score_t score;
	
	score = bb_score(state.board);
	int max, min;
	if (state.player == BLACK_STONE) {
		max = score.black;
		min = score.white;
	} else {
		max = score.white;
		min = score.black;
	}
	return 100 * (float) (max - min ) / (float) (max + min);
}

int max(int a, int b) {
	if (a >= b) {
		return a;
	} else {
		return b;
	}
}

int min(int a,int b) {
	if (a <= b) {
		return a;
	} else {
		return b;
	}
}

algo_t minimax_aux(bitboard_t board, char player, int depth, int (*heuristic) (state_t state), int maximizing) {
	int best_value;
	int num_moves = 0, x, y;
	bitboard_t aux_board, moved_board;
	move_t move;
	state_t state;
	score_t score;
	algo_t res, v;
	state.board = board;
	state.player = player;
	aux_board = bb_moves(state);
	score = bb_score(aux_board);
	num_moves = score.black + score.white;
	if (depth == 0 || num_moves == 0) {
		state.board = board;
		state.player = player;
		res.v = (*heuristic)(state);
		return res;
	}

	if (maximizing) {
		best_value = INT_MIN;
		for (x = 0; x < state.board.size; x++) {
			for (y = 0; y < state.board.size; y++) {
				if (state.player == BLACK_STONE) {
					if (get_bit(aux_board.black, one_dimension(x, y, state.board.size))) {
						move.column = (size_t) y;
						move.row = (size_t) x;
						moved_board = bb_move(move, state);
						v = minimax_aux(moved_board, WHITE_STONE, depth - 1, heuristic, 0);
						v.move.column = move.column;
						v.move.row = move.row;
						best_value = max(v.v, best_value);
						if (v.v == best_value) {
							res.move.column = v.move.column;
							res.move.row = v.move.row;
							res.v = v.v;
						}
					}
				} else {
					if (get_bit(aux_board.white, one_dimension(x, y, state.board.size))) {
						move.column = (size_t) y;
						move.row = (size_t) x;
						moved_board = bb_move(move, state);
						v = minimax_aux(moved_board, BLACK_STONE, depth - 1, heuristic, 0);
						v.move.column = move.column;
						v.move.row = move.row;
						best_value = max(v.v, best_value);
						if (v.v == best_value) {
							res.move.column = v.move.column;
							res.move.row = v.move.row;
							res.v = v.v;
						}
					}
				}
			}
		}
		return res;
	} else {
		best_value = INT_MAX;
		for (x = 0; x < state.board.size; x++) {
			for (y = 0; y < state.board.size; y++) {
				if (state.player == BLACK_STONE) {
					if (get_bit(aux_board.black, one_dimension(x, y, state.board.size))) {
						move.column = (size_t) y;
						move.row = (size_t) x;
						moved_board = bb_move(move, state);
						v = minimax_aux(moved_board, WHITE_STONE, depth - 1, heuristic, 1);
						v.move.column = move.column;
						v.move.row = move.row;
						best_value = min(v.v, best_value);
						if (v.v == best_value) {
							res.move.column = v.move.column;
							res.move.row = v.move.row;
							res.v = v.v;
						}
					}
				} else {
					if (get_bit(aux_board.white, one_dimension(x, y, state.board.size))) {
						move.column = (size_t) y;
						move.row = (size_t) x;
						moved_board = bb_move(move, state);
						v = minimax_aux(moved_board, BLACK_STONE, depth - 1, heuristic, 1);
						v.move.column = move.column;
						v.move.row = move.row;
						best_value = max(v.v, best_value);
						if (v.v == best_value) {
							res.move.column = v.move.column;
							res.move.row = v.move.row;
							res.v = v.v;
						}
					}
				}
			}
		}
		return res;
	}
}

move_t minimax(state_t state, int depth, int (*heuristic) (state_t state)) {
	return minimax_aux(state.board, state.player, depth, heuristic, 1).move;
}

algo_t negamax_aux(bitboard_t board, char player, int depth, int (*heuristic) (state_t state), int color) {
	state_t state;
	algo_t res, v;
	int x, y;
	move_t move;
	state.board = board;
	state.player = player;
	bitboard_t aux_board = bb_moves(state), moved_board;
	score_t score = bb_score(aux_board);
	int num_moves = score.black + score.white;
	if (depth == 0 || num_moves == 0) {
		state.board = board;
		state.player = player;
		res.v = (*heuristic)(state) * color;
		return res;
	}
	
	int best_value = INT_MIN;
	for (x = 0; x < state.board.size; x++) {
		for (y = 0; y < state.board.size; y++) {
			if (state.player == BLACK_STONE) {
				if (get_bit(aux_board.black, one_dimension(x, y, state.board.size))) {
					move.column = (size_t) y;
					move.row = (size_t) x;
					moved_board = bb_move(move, state);
					v = negamax_aux(moved_board, WHITE_STONE, depth - 1, heuristic, -color);
					v.move.column = move.column;
					v.move.row = move.row;
					v.v *= -1;
					best_value = max(v.v, best_value);
					if (v.v == best_value) {
						res.move.column = v.move.column;
						res.move.row = v.move.row;
						res.v = v.v;
					}
				}
			} else {
				if (get_bit(aux_board.white, one_dimension(x, y, state.board.size))) {
					move.column = (size_t) y;
					move.row = (size_t) x;
					moved_board = bb_move(move, state);
					v = negamax_aux(moved_board, BLACK_STONE, depth - 1, heuristic, -color);
					v.move.column = move.column;
					v.move.row = move.row;
					v.v *= -1;
					best_value = max(v.v, best_value);
					if (v.v == best_value) {
						res.move.column = v.move.column;
						res.move.row = v.move.row;
						res.v = v.v;
					}
				}
			}
		}
	}
	return res;
}

move_t negamax(state_t state, int depth, int (*heuristic) (state_t state)) {
	return negamax_aux(state.board, state.player, depth, heuristic, 1).move;
}

algo_t minimax_alphabeta_aux(bitboard_t board, char player, int depth, int alpha, int beta, int (*heuristic) (state_t state), int maximizing) {
	int best_value;
	int num_moves = 0, x, y;
	bitboard_t aux_board, moved_board;
	move_t move;
	state_t state;
	score_t score;
	algo_t res, v;
	state.board = board;
	state.player = player;
	aux_board = bb_moves(state);
	score = bb_score(aux_board);
	num_moves = score.black + score.white;
	if (depth == 0 || num_moves == 0) {
		state.board = board;
		state.player = player;
		res.v = (*heuristic)(state);
		return res;
	}

	if (maximizing) {
		best_value = INT_MIN;
		for (x = 0; x < state.board.size; x++) {
			for (y = 0; y < state.board.size; y++) {
				if (state.player == BLACK_STONE) {
					if (get_bit(aux_board.black, one_dimension(x, y, state.board.size))) {
						move.column = (size_t) y;
						move.row = (size_t) x;
						moved_board = bb_move(move, state);
						v = minimax_alphabeta_aux(moved_board, WHITE_STONE, depth - 1, alpha, beta, heuristic, 0);
						v.move.column = move.column;
						v.move.row = move.row;
						best_value = max(v.v, best_value);
						alpha = max(alpha, v.v);
						if (beta <= alpha)
							break;
						if (v.v == best_value) {
							res.move.column = v.move.column;
							res.move.row = v.move.row;
							res.v = v.v;
						}
					}
				} else {
					if (get_bit(aux_board.white, one_dimension(x, y, state.board.size))) {
						move.column = (size_t) y;
						move.row = (size_t) x;
						moved_board = bb_move(move, state);
						v = minimax_alphabeta_aux(moved_board, BLACK_STONE, depth - 1, alpha, beta, heuristic, 0);
						v.move.column = move.column;
						v.move.row = move.row;
						best_value = max(v.v, best_value);
						alpha = max(alpha, v.v);
						if (beta <= alpha)
							break;
						if (v.v == best_value) {
							res.move.column = v.move.column;
							res.move.row = v.move.row;
							res.v = v.v;
						}
					}
				}
			}
		}
		return res;
	} else {
		best_value = INT_MAX;
		for (x = 0; x < state.board.size; x++) {
			for (y = 0; y < state.board.size; y++) {
				if (state.player == BLACK_STONE) {
					if (get_bit(aux_board.black, one_dimension(x, y, state.board.size))) {
						move.column = (size_t) y;
						move.row = (size_t) x;
						moved_board = bb_move(move, state);
						v = minimax_alphabeta_aux(moved_board, WHITE_STONE, alpha, beta, depth - 1, heuristic, 1);
						v.move.column = move.column;
						v.move.row = move.row;
						best_value = min(v.v, best_value);
						beta = min(beta, v.v);
						if (beta <= alpha)
							break;
						if (v.v == best_value) {
							res.move.column = v.move.column;
							res.move.row = v.move.row;
							res.v = v.v;
						}
					}
				} else {
					if (get_bit(aux_board.white, one_dimension(x, y, state.board.size))) {
						move.column = (size_t) y;
						move.row = (size_t) x;
						moved_board = bb_move(move, state);
						v = minimax_alphabeta_aux(moved_board, BLACK_STONE, alpha, beta, depth - 1, heuristic, 1);
						v.move.column = move.column;
						v.move.row = move.row;
						best_value = max(v.v, best_value);
						if (v.v == best_value) {
							res.move.column = v.move.column;
							res.move.row = v.move.row;
							res.v = v.v;
						}
					}
				}
			}
		}
		return res;
	}
}


move_t minimax_alphabeta(state_t state, int depth, int (*heuristic) (state_t state)) {
	/*(* Initial call *)
	alphabeta(origin, depth, -∞, +∞, TRUE)*/
	return minimax_alphabeta_aux(state.board, state.player, depth, INT_MIN, INT_MAX, heuristic, 1).move;
	
}

algo_t negamax_alphabeta_aux(bitboard_t board, char player, int depth, int alpha, int beta, int (*heuristic) (state_t state), int color) {
	state_t state;
	algo_t res, v;
	int x, y;
	move_t move;
	state.board = board;
	state.player = player;
	bitboard_t aux_board = bb_moves(state), moved_board;
	score_t score = bb_score(aux_board);
	int num_moves = score.black + score.white;
	if (depth == 0 || num_moves == 0) {
		state.board = board;
		state.player = player;
		res.v = (*heuristic)(state) * color;
		return res;
	}
	
	int best_value = INT_MIN;
	for (x = 0; x < state.board.size; x++) {
		for (y = 0; y < state.board.size; y++) {
			if (state.player == BLACK_STONE) {
				if (get_bit(aux_board.black, one_dimension(x, y, state.board.size))) {
					move.column = (size_t) y;
					move.row = (size_t) x;
					moved_board = bb_move(move, state);
					v = negamax_aux(moved_board, WHITE_STONE, depth - 1, heuristic, -color);
					v.move.column = move.column;
					v.move.row = move.row;
					v.v *= -1;
					best_value = max(v.v, best_value);
					alpha = max(alpha, v.v);
					if (alpha >= beta)
						break;
					if (v.v == best_value) {
						res.move.column = v.move.column;
						res.move.row = v.move.row;
						res.v = v.v;
					}
				}
			} else {
				if (get_bit(aux_board.white, one_dimension(x, y, state.board.size))) {
					move.column = (size_t) y;
					move.row = (size_t) x;
					moved_board = bb_move(move, state);
					v = negamax_aux(moved_board, BLACK_STONE, depth - 1, heuristic, -color);
					v.move.column = move.column;
					v.move.row = move.row;
					v.v *= -1;
					best_value = max(v.v, best_value);
					alpha = max(alpha, v.v);
					if (alpha >= beta)
						break;
					if (v.v == best_value) {
						res.move.column = v.move.column;
						res.move.row = v.move.row;
						res.v = v.v;
					}
				}
			}
		}
	}
	return res;
}

move_t negamax_alphabeta(state_t state, int depth, int (*heuristic) (state_t state)) {
	return negamax_alphabeta_aux(state.board, state.player, depth, INT_MIN, INT_MAX, heuristic, 1).move;
}

algo_t negascout_aux(bitboard_t board, char player, int depth, int alpha, int beta, int (*heuristic) (state_t state)) {
	int num_moves = 0, x, y, b, a;
	bitboard_t aux_board, moved_board;
	move_t move;
	state_t state;
	score_t score;
	algo_t res, v;
	state.board = board;
	state.player = player;
	aux_board = bb_moves(state);
	score = bb_score(aux_board);
	num_moves = score.black + score.white;
	if (depth == 0 || num_moves == 0) {
		state.board = board;
		state.player = player;
		res.v = (*heuristic)(state);
		return res;
	}

	b = beta;
	for (x = 0; x < state.board.size; x++) {
		for (y = 0; y < state.board.size; y++) {
			if (state.player == BLACK_STONE) {
				if (get_bit(aux_board.black, one_dimension(x, y, state.board.size))) {
					move.column = (size_t) y;
					move.row = (size_t) x;
					moved_board = bb_move(move, state);
					v = negascout_aux(moved_board, WHITE_STONE, depth - 1, -b, -alpha, heuristic);
					v.v *= -1;
					v.move.column = move.column;
					v.move.row = move.row;
					a = v.v;
					if (a > alpha)
						alpha = a;
					if (alpha >= beta)
						return v;
					if (alpha >= b) {
						v = negascout_aux(moved_board, WHITE_STONE, depth - 1, -b, -alpha, heuristic);
						v.v *= -1;
						alpha = v.v;
						if (alpha >= beta)
							return v;
					}
					b = alpha + 1;
					
				}
			} else {
				if (get_bit(aux_board.white, one_dimension(x, y, state.board.size))) {
					move.column = (size_t) y;
					move.row = (size_t) x;
					moved_board = bb_move(move, state);
					v = negascout_aux(moved_board, BLACK_STONE, depth - 1, -b, -alpha, heuristic);
					v.move.column = move.column;
					v.move.row = move.row;
					v.v *= -1;
					a = v.v;
					if (a > alpha)
						alpha = a;
					if (alpha >= beta)
						return v;
					if (alpha >= b) {
						v = negascout_aux(moved_board, BLACK_STONE, depth - 1, -b, -alpha, heuristic);
						v.move.column = move.column;
						v.move.row = move.row;
						v.v *= -1;
						alpha = v.v;
						if (alpha >= beta)
							return v;
					}
					b = alpha + 1;
				}
			}
		}
	}
	return v;
}

move_t negascout(state_t state, int depth, int (*heuristic) (state_t state)) {
	return negascout_aux(state.board, state.player, depth, INT_MIN, INT_MAX, heuristic).move;
}

move_t ai_player(state_t state) {
	return minimax_alphabeta(state, 2, coin_parity_heuristic);
}
