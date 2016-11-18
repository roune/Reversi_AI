
#include "../include/bitboard.h"

size_t board_size;
bool verbose;
int game_mode;

static void usage(int status) {
	if (status == EXIT_SUCCESS){
		printf("Usage: reversi [OPTION] FILE\n"
		"Play a reversi game interactively with humans and AIs\n"
		"\n\t -s, --size SIZE\t board size (min=1, max=4 (default))\n"
		"\t -c, --contest\t enable 'contest mode'\n"
		"\t -b, --black-ai\t set black player as an AI\n"
		"\t -w, --white-ai\t set white player as an AI\n"
		"\t -v, --verbose\t verbose output\n"
		"\t -V, --version\t display version and exit\n"
		"\t -h, --help\t display this help\n");
	} else {
		fprintf(stderr, "Try 'reversi --help' for more information\n");
	}
}

static void version() {
	printf("reversi %d.%d.%d\n", PROG_VERSION, PROG_SUBVERSION, PROG_REVISION);
	printf("This software allows to play to reversi game\n");
}

char* trim_white_spaces(char* input) {
    int i,j;
    char *output = input;
    for (i = 0, j = 0; i < strlen(input); i++,j++) {
        if (input[i]!=' '){
			output[j]=input[i];                     
		} else {
			j--;
		}
    }
    output[j]=0;
    return output;
}

static state_t board_init(size_t width) {
	state_t state;
	state.board = bb_init(board_size);
	state.player = BLACK_STONE;
	return state;
}

static void board_delete(char** board) {
	/*int i;
	
	if (board == NULL) {
		return;
	}
	
	for (i = 0; i < board_size; i++) {
		free(board[i]);
	}
	free(board);*/
}

static score_t board_score(state_t state) {
	return bb_score(state.board);
}

static void board_print(state_t state) {
	bb_print(state.board);
}

static void board_save(state_t state) {
	char filename[50];
	int i, j;
	FILE * f;
	
	printf("Give a filename to save the game (default: 'board.txt'): ");
	fgets(filename, 50, stdin);
	strcpy(filename, trim_white_spaces(filename));
	
	if (strlen(filename) == 1) {
		strcpy(filename, "board.txt");
	}
	
	f = fopen(filename, "w");
	if (f == NULL) {
		printf("Could not save the board, please try later\n");
		exit(EXIT_SUCCESS);
	}
	fprintf(f, "%c\n", state.player);
	for(i = 0; i < state.board.size; i++) {
		for (j = 0; j < state.board.size; j++) {
			if (get_bit(state.board.black, one_dimension(j, i, state.board.size))) {
				fprintf(f, "X ");
			} else if (get_bit(state.board.white, one_dimension(j, i, state.board.size))) {
				fprintf(f, "O ");
			} else {
				fprintf(f, "_ ");
			}
		}
		fprintf(f, "\n");
	}
	fclose(f);
}

static void old_print(char** board) {
	int i, j;
	char letter;
	
	printf("\t");
	for (i = 0; i < board_size; i++) {
		printf("%d ", i+1);
	}
	printf("\n");
	for (i = 0; i < board_size; i++) {
		switch(i) {
			case 0:
				letter = 'A';
				break;
			case 1:
				letter = 'B';
				break;
			case 2:
				letter = 'C';
				break;
			case 3:
				letter = 'D';
				break;
			case 4:
				letter = 'E';
				break;
			case 5:
				letter = 'F';
				break;
			case 6:
				letter = 'G';
				break;
			case 7:
				letter = 'H';
				break;
		}
		printf("%c\t", letter);
		for (j = 0; j < board_size; j++) {
			printf("%c ", board[i][j]);
		}
		printf("\n");
	}
}

static state_t board_load(char * filename) {
	char unknown_char_regexp[] = "^[_OX \t]|O{2,}|X{2,}";
	int res, num_char, num_line = 0, rows = 0, i, j;
	char line[200], aux[200], read_char, **board;
	size_t width = -1;
	state_t state;
	row_t row;
	bitboard_t b;
	FILE * f;
	
	state.player = 0;
	
	f = fopen(filename, "r");
	
	if (f == NULL) {
		printf("File not found\n");
		exit(EXIT_FAILURE);
	}
	
	//CHECKING FIRST LINE TO BE A PLAYER'S TURN
	do {
 		num_line++;
		fgets(line, 200, f);
		
		res = is_there_comment(line);
		if (res != -1) {
			strncpy(aux, line, res);
			strcpy(line, aux);
		}
		if ((num_char = is_first_line_correct(line)) >= 0 && num_char < strlen(line)) {
			fprintf(stderr, "reversi: error: wrong character '%c' at line %d\n", line[num_char], num_line);
			return state;
		}
		state.player = read_current_player(line);
	} while(!state.player);
	
	//NORMAL BOARD
	do {
		num_line++;
		if (fgets(line, 200, f) == NULL) {
			break;
		}
		
		res = is_there_comment(line);
		if (res != -1) {
			if (res == 0)
				continue;
			strncpy(aux, line, res);
			strcpy(line, aux);
		}
		if ((num_char = is_line_correct(line)) >= 0 && num_char < strlen(line)) {
			fprintf(stderr, "reversi: error: wrong character '%c' at line %d\n",
			line[num_char], num_line);
			return state;
		}
		row = read_row(line, num_line);
		if (row.width <= 0)
			continue;
		if (width == -1) {
			width = row.width;
			board = malloc(sizeof(char*)*width);
			if (board == NULL) {
				fprintf(stderr, "No memory available\n");
				exit(EXIT_SUCCESS);
			}
		} else if (width != row.width) {
			if (board) {
				for (i = 0; i < rows; i++) {
					free(board[i]);
				}
				if (width > row.width) {
					fprintf(stderr, "reversi: error board is too short at line %d\n", num_line);
				} else if (width < row.width) {
					fprintf(stderr, "reversi: error board is too long at line %d\n", num_line);
				}
				free(board);
			}
			exit(EXIT_FAILURE);
		}
		board[rows] = malloc(sizeof(char)*width);
		for (i = 0; i < width; i++) {
			board[rows][i] = row.row[i];
		}
		rows++;
	} while(rows != width);
	if (rows > width) {
		fprintf(stderr, "reversi: error: board has too few rows\n");
		exit(EXIT_FAILURE);
	} else if (rows < width) {
		fprintf(stderr, "reversi: error: board has too many rows\n");
		exit(EXIT_FAILURE);
	}
	b.size = width;
	b.black = 0;
	b.white = 0;
	for (i = 0; i < width; i++) {
		for (j = 0; j < width; j++) {
			if (board[j][i] == BLACK_STONE) {
				b.black = set_bit(b.black, one_dimension(i, j, width), width);
			} else if (board[i][j] == WHITE_STONE) {
				b.white = set_bit(b.white, one_dimension(i, j, width), width);
			}
		}
	}
	state.board = b;
	board_size = width;
	return state;
}

row_t read_row(char * line, int num_line) {
	row_t row;
	int i, j;
	row.width = -1;
	row.row = malloc(sizeof(char)*8);
	i = 0, j = 0;
	while(line[i] != '\n' && line[i] != 0) {
		if (line[i] != ' ') {
			if (j >= 8) {
				fprintf(stderr, "reversi: error: board width is too long at line %d\n", num_line);
				exit(EXIT_FAILURE);
			}
			row.row[j] = line[i];
			j++;
		}
		i++;
	}
	row.width = j;
	return row;
}

char read_current_player(char * line) {
	char read_column_regexp[] = "[OX]";
	regex_t reg;
	regmatch_t matches[1];
	int res;
	res = regcomp(&reg, read_column_regexp, REG_EXTENDED);
	if (res) {
		fprintf(stderr, "Error defining regexp\n");
		return -1;
	}
	res = regexec(&reg, line, 1, matches, 0);
	if (!res) {
		return line[(int)matches[0].rm_so];
	}
	return 0;
}

int is_there_comment(char * line) {
	char comment_regexp[] = "#";
	regex_t reg;
	regmatch_t matches[1];
	int res;
	res = regcomp(&reg, comment_regexp, REG_EXTENDED);
	if (res) {
		printf("Error defining regexp\n");
		return -1;
	}
	res = regexec(&reg, line, 1, matches, REG_NOTBOL);
	if (!res) {
		return (int)matches[0].rm_so;
	}
	return -1;
}



int is_line_correct(char * line) {
	char correct_line_regexp[] = "^[OX_ ]|O{2,}|X{2,}|_{2,}";
	regex_t reg;
	regmatch_t matches[1];
	int res;
	res = regcomp(&reg, correct_line_regexp, REG_EXTENDED);
	if (res) {
		printf("Error defining regexp\n");
		return -1;
	}
	res = regexec(&reg, line, 1, matches, REG_NOTBOL);
	if (!res) {
		return (int)matches[0].rm_so;
	}
	return -1;
}

int is_first_line_correct(char * line) {
	int i;
	i = 0;
	while(line[i] != '\n') {
		if (line[i] != ' ' && line[i] != BLACK_STONE && line[i] != WHITE_STONE && line[i] != EMPTY_STONE) {
			return i;
		}
		i++;
	}
	/*row.row = cur_row;
	row.width = j;*/
	return -1;
}

int count_valid_moves(state_t state) {
	bitboard_t a = bb_moves(state);
	int i, count = 0;
	uint64_t table;
	if (state.player == BLACK_STONE) {
		table = a.black;
	} else {
		table = a.white;
	}
	for (i = 0; i < a.size * a.size; i++) {
		if (get_bit(table, i))
			count++;
	}
	return count;
}

bitboard_t board_play(state_t state, move_t move) {
	return bb_move(move, state);
}

static int is_game_over(char ** board) {
	/*int i, j, count = 0;
	for (i = 0; i < board_size; i++) {
		for (j = 0; j < board_size; j++) {
			if (board[i][j] != EMPTY_STONE)
				count++;
		}
	}
	if (count == board_size*board_size)
		return 1;
	else 
		return 0;*/
}

void quit_program(state_t state) {
	char string[200];
	int i = 0;
	printf("Quitting, do you want to save the game (y/N)? ");
	fgets(string, 200, stdin);
	while(string[i] == ' ' && string[i] != 0) {
		i++;			
	}
	if (string[i] != 0 && string[i] == 'y' || string[i] == 'Y') {
		board_save(state);
	}
	/*board_delete(state.board);*/
	exit(EXIT_SUCCESS);
}

static state_t human_player(state_t state) {
	char string[200];
	move_t move;
	//board_print(state);
	score_t score = bb_score(state.board);
	printf("Score:\n'O': %d, 'X': %d\n", score.white, score.black);
	
	printf("'%c' player's turn.\n Give your move (e.g. 'A5' or 'a5'), press 'q' or 'Q' to quit: ", state.player);
	memset(string,0,sizeof(string));
	fgets(string, 199, stdin);
	if (strstr(string, "q") || strstr(string, "Q")) {
		quit_program(state);
	}
	move = read_move(string);

	bitboard_t res = board_play(state, move);
	while(!res.size) {
		printf("Move not valid. Try again: ");
		fgets(string, 199, stdin);
		move = read_move(string);
		if (strstr(string, "q") || strstr(string, "Q")) {
			quit_program(state);
		}
		res = board_play(state, move);
	}
	state.board = res;
	return state;
}

void contest(char * line) {
	state_t state = board_load(line);
	move_t move = ai_player(state);
	printf("%c%d\n", move.column + 'a', move.row + 1);
	exit(0);
}

void game(char * line) {
	char string[200];
	char temporary_player;
	move_t move;
	state_t state;
	if(line) {
		state = board_load(line);
	} else {
		state = board_init(board_size);
	}
	score_t score_tmp = bb_score(state.board);
	if (score_tmp.black < 2 && score_tmp.white < 2) {
		fprintf(stderr, "Incorrect board\n");
		exit(EXIT_FAILURE);
	}
	
	printf("Welcome to this reversi game!\n");
	printf("Black player(X) is human and white player (O) is human.\n");
	printf("Black player start\n");
	int i = 0;
	while(1) {
		i++;
		if (i==13) {
			printf("n\n");
		}
		score_t score = bb_score(state.board);
		if (count_valid_moves(state)== 0) {
			temporary_player = state.player;
			if (state.player == WHITE_STONE) {
				state.player = BLACK_STONE;
			} else {
				state.player = WHITE_STONE;
			}
			if (count_valid_moves(state) == 0) {
				printf("Game over\n");
				if (score.white > score.black) {
					printf("Player 'O' win the game\n");
				} else if (score.white < score.black) {
					printf("Player 'X' win the game\n");
				} else {
					printf("Draw game, no winner\n");
				}
				board_print(state);
				printf("Score:\n'O': %d, 'X': %d\n", score.white, score.black);
				printf("Thanks for playing, see you soon!\n");
				//board_delete(state.board);
				return;
			}
			printf("'%c' Does not have moves, turn changes\n", temporary_player);
		}
		board_print(state);
		if (state.player == BLACK_STONE && (game_mode == 1 || game_mode == 3)) {
			printf("\n");
			move_t move = ai_player(state);
			bitboard_t res = board_play(state, move);
			
			state.board = res;
			if (res.size == 0) {
				bb_moves(state);
			}
		} else if (state.player == WHITE_STONE && (game_mode == 2 || game_mode == 3)) {
			printf("\n");
			move_t move = ai_player(state);
			bitboard_t res = board_play(state, move);
			state.board = res;
			if (res.size == 0) {
				bb_moves(state);
			}
		} else {
			state = human_player(state);
		}
		
		if (state.player == WHITE_STONE) {
			state.player = BLACK_STONE;
		} else {
			state.player = WHITE_STONE;
		}
	}
}

move_t read_move(char * string) {
	move_t move;
	char * letter;
	int y = 0, x;
	letter = string;
	while(*letter == ' ')
		letter++;
	
	y = tolower(letter[0]) - 'a';
	if (y > 7 || y < 0) {
		move.column = -1;
		move.row = -1;
		return move;
	}
	letter++;
	
	while(*letter == ' ')
		letter++;
	
	if (isdigit(*letter)) {
		x = atoi(letter);
		if (x > 8) {
			move.column = -1;
			move.row = -1;
			return move;
		}
	} else {
		move.column = -1;
		move.row = -1;
		return move;
	}
	
	move.row = x-1;
	move.column = y;
	return move;
}



int main(int argc, char *argv[]) {
	int optc;
	bool other_prev_options = false, end = false;
	char *filename = NULL;
	struct option long_opts[] = {
		{"size", required_argument, NULL, 's'},
		{"black-ai", no_argument, NULL, 'b'},
		{"white-ai", no_argument, NULL, 'w'},
		{"all-ai", no_argument, NULL, 'a'},
		{"verbose", no_argument, NULL, 'v'},
		{"Version", no_argument, NULL, 'V'},
		{"contest", required_argument, NULL, 'c'},
		{"h", no_argument, NULL, 'h'},
		{NULL, no_argument, NULL, 0}
	};
	board_size = 8;
	verbose = false;
	game_mode = 0;
	while(((optc = getopt_long (argc, argv, "s:bwavVc:h", long_opts, NULL)) != 1) && !end) {
		switch(optc) {
			case 's':
				other_prev_options = 1;
				if (!optarg)
					board_size = 2*atoi(argv[optind]);
				else
					board_size = 2*atoi(optarg);
				if (board_size < 2) {
					fprintf(stderr, "Board size too small (1-4)\n");
					return EXIT_FAILURE;
				} else if (board_size > 8) {
					fprintf(stderr, "Board size too big (1-4)\n");
					return EXIT_FAILURE;
				}
				optind += 1;
				break;
			case 'V':
				other_prev_options = 1;
				version();
				exit(0);
				break;
			case 'v':
				other_prev_options = 1;
				verbose = true;
				break;
			case 'a':
				other_prev_options = 1;
				game_mode = 3;
				break;
			case 'b':
				other_prev_options = 1;
				game_mode = 1;
				break;
			case 'w':
				other_prev_options = 1;
				game_mode = 2;
				break;
			case '?':
				usage(EXIT_FAILURE);
				return EXIT_FAILURE;
			case 'c':
				filename = malloc(sizeof(char)*(strlen(argv[optind]) + 1));
				strcpy(filename, argv[optind]);
				contest(filename);
			default:
				if (argv[optind]) {
					filename = malloc(sizeof(char)*(strlen(argv[optind]) + 1));
					strcpy(filename, argv[optind]);
				}
				end = true;
				break;
		}
	}
	game(filename);
	return 0;
}
