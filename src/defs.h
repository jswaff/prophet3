/*
 * defs.h
 *
 *  Created on: Feb 1, 2011
 *      Author: james
 */

#ifndef DEFS_H_
#define DEFS_H_

typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef uint64 move;

const int MAX_CHARS_PER_MOVE = 10;
const int MAX_MOVES_PER_PLY = 300;
const int MAX_MOVES_PER_GAME = 5948; // http://blog.chess.com/kurtgodden/the-longest-possible-chess-game
const int MAX_PLY = 100;
const int MOVE_STACK_SIZE = 5000;
const int UNDO_STACK_SIZE = MAX_MOVES_PER_GAME + MAX_PLY;

const int32 INF = 200000;
const int32 CHECKMATE = 65535;
const int32 DRAWSCORE = 0;
const int32 BADMOVE = 0;

enum square_t { A8,B8,C8,D8,E8,F8,G8,H8,
                A7,B7,C7,D7,E7,F7,G7,H7,
                A6,B6,C6,D6,E6,F6,G6,H6,
                A5,B5,C5,D5,E5,F5,G5,H5,
                A4,B4,C4,D4,E4,F4,G4,H4,
                A3,B3,C3,D3,E3,F3,G3,H3,
                A2,B2,C2,D2,E2,F2,G2,H2,
                A1,B1,C1,D1,E1,F1,G1,H1,
                NO_SQUARE };

enum rank_t { RANK_8,RANK_7,RANK_6,RANK_5,RANK_4,RANK_3,RANK_2,RANK_1 };
enum file_t { FILE_A,FILE_B,FILE_C,FILE_D,FILE_E,FILE_F,FILE_G,FILE_H };
enum dir_t { NORTH,NORTHEAST,EAST,SOUTHEAST,SOUTH,SOUTHWEST,WEST,NORTHWEST,NODIR };

enum piece_t { NO_PIECE,PAWN,KNIGHT,BISHOP,ROOK,QUEEN,KING };

enum color_t { BLACK=0,WHITE=1 };

enum hash_entry_t { MOVE_ONLY,LOWER_BOUND,UPPER_BOUND,EXACT_SCORE };

enum move_order_stage { PVMOVE,HASHMV,GENCAPS,CAPTURES_PROMOS,KILLER1,KILLER2,GENNONCAPS,REMAINING };

enum game_status { INPROGRESS,CHECKMATED,STALEMATED,DRAW_MATERIAL,
				DRAW_BY_50,DRAW_REP,RESIGN };

typedef struct {
	int32 val1;
	int32 val2;
} int32_pair;

typedef struct {
	bool val1;
	bool val2;
	bool val3;
} bool_triple;

typedef struct {
	int32 piece[64];
	uint32 piece_counts[2][7];
	color_t player;
	square_t ep_sq;
	uint32 castling_rights;
	uint32 move_counter;
	uint32 fifty_counter;
	square_t white_king;
	square_t black_king;
	uint64 white_pawns;
	uint64 black_pawns;
	uint64 white_knights;
	uint64 black_knights;
	uint64 white_bishops;
	uint64 black_bishops;
	uint64 white_rooks;
	uint64 black_rooks;
	uint64 white_queens;
	uint64 black_queens;
	uint64 white_pieces;
	uint64 black_pieces;
	uint64 hash_key;
	uint64 pawn_key;
} position;

typedef struct {
	int n; // number of moves
	move mv[MAX_PLY];
} move_line;

typedef struct {

	uint64 fail_highs;
	uint64 fail_lows;
	uint64 hash_exact_scores;

	uint64 nodes;
	uint64 qnodes;
	move_line last_pv;
	move_line first_line_searched;
	uint64 prunes;

} search_stats;

typedef struct {
	move mv;
	uint64 hash_key;
	piece_t captured;
	square_t ep_sq;
	uint32 fifty_counter;
	uint32 castling_rights;
} undo;

typedef struct {
	uint64 pieces[7][2][64];
	uint64 ptm[2];
	uint64 casting_rights[16];
	uint64 ep[65]; // include NO_SQUARE
} zobrist_keys;

typedef struct {
	uint64 key;
	uint64 val;
} hash_entry;

typedef struct {
	hash_entry *tblptr;
	uint32 tblsize;
	uint64 tblmask;
	uint64 probes;
	uint64 hits;
	uint64 collisions;
} hash_table;

typedef struct {
	move_order_stage next_stage;
	bool caps_only;
	move pv_move;
	move hash_move;
	move killer1;
	move killer2;
	move *startp;
	move *endp;
	move *currentp;
} move_order_dto;

typedef struct {
	move mv;
	int32 freq;
} move_freq_pair;

typedef square_t (*dirFuncType)(square_t);

// i/o
void draw_board(position *p);
void draw_bitboard(uint64 bb);
void error(const char* format, ...);
void init_logfile();
void parse_command(const char *cmd);
void print(const char* format, ...);

// apply
void apply_move(position* p,move m,undo *undo_stack);
square_t apply_null_move(position *p);
void undo_last_move(position *p,undo *undo_stack);
void undo_null_move(position *p,square_t ep_sq);

// attack
bool attacked(position *p,square_t sq,color_t player);
bool attacked_by_knight(position *p,square_t sq,color_t player);
bool attacked_by_king(position *p,square_t sq,color_t player);
bool attacked_by_rook(position *p,square_t sq,color_t player);
bool attacked_by_bishop(position *p,square_t sq,color_t player);
bool attacked_by_queen(position *p,square_t sq,color_t player);
bool attacked_by_pawn(position *p,square_t sq,color_t player);
uint64 get_attackers(position *p,square_t sq,color_t player);

// bitboards
uint32 get_num_bits(uint64 val);
uint32 get_lsb(uint64 val);
uint32 get_msb(uint64 val);
uint64 isolate_lsb_bit(uint64 mask,uint32 bit_pos);
void init_bitboards();

// book
int32 get_book_moves(move_freq_pair *mfp,position *p);
int32 open_book_db(char *path);
int64 build_book_key(position *p);
move probe_book(position *p);

// draw
bool is_lack_of_mating_material(position *pos);
bool is_draw_by_rep(position *pos,undo *undo_stack);
bool is_draw_by_50(position *pos);


// eval
int32 piece_val(int32 pc);
int32 eval(position *p,bool mat_only);
int32 eval_knight(position *p,square_t sq,bool wtm);
int32 eval_bishop(position *p,square_t sq,bool wtm);
int32 eval_rook(position *p,square_t sq,bool wtm);
int32 eval_queen(position *p,square_t sq,bool wtm);
int32_pair eval_nonpawn_material(position *p);
int32_pair eval_pawn_material(position *p);
int32 eval_scale(int32 score,int32 material);

// evalks
int32 eval_king_safety(position *p,bool wtm);

// evalp
int32 eval_pawn(position *p,square_t sq,bool wtm);
bool is_doubled(position *pos,square_t pawn_sq,bool wtm);
bool is_isolated(const position *pos,const square_t pawn_sq,const bool wtm);
bool is_passed(position *pos,square_t pawn_sq,bool wtm);
void init_evalp_bitboards();

// extend
int32 extend(position *pos,move last_move,bool incheck);

// hash
uint64 build_hash_key(position *pos);
uint64 build_pawn_key(position *pos);
uint64 build_hash_val(hash_entry_t entry_type,int32 depth,int32 score,move mv);
uint64 build_pawn_hash_val(int32 score);
hash_entry_t get_hash_entry_type(uint64 val);
int32 get_hash_entry_depth(uint64 val);
int32 get_hash_entry_score(uint64 val);
int32 get_pawn_hash_entry_score(uint64 val);
move get_hash_entry_move(uint64 val);
void init_hash_table(hash_table *tbl,uint32 tblsize);
void clear_hash_table(hash_table *tbl);
uint64 get_hash_entry(hash_table *tbl,uint64 key);
void store_hash_entry(hash_table *tbl,uint64 key,uint64 val);

// init
void init();

// magic
uint64 get_bishop_moves(position* p,square_t from,uint64 targets);
uint64 get_queen_moves(position* p,square_t from,uint64 targets);
uint64 get_rook_moves(position* p,square_t from,uint64 targets);
void init_magics();

// move
move to_move(piece_t piece,square_t from,square_t to);
void to_move(move *mv,piece_t piece,square_t from,square_t to);
move to_capture(piece_t piece,square_t from,square_t to,piece_t captured_piece);
void to_capture(move* mv,piece_t piece,square_t from,square_t to,piece_t captured_piece);
void set_promopiece(move *mv,piece_t promo);
void set_capture(move *mv,piece_t captured_piece);
void set_epcapture(move *mv);
void set_castle(move *mv);
square_t get_from_sq(move mv);
square_t get_to_sq(move mv);
piece_t get_piece(move mv);
piece_t get_captured_piece(move mv);
piece_t get_promopiece(move mv);
bool is_capture(move mv);
bool is_epcapture(move mv);
bool is_castle(move mv);
int32 get_move_score(move *m);
void set_move_score(move *m,int32 score);
move clear_score(move mv);

// movegen
move* gen_knight_moves(move* m,position* p,bool caps,bool noncaps);
move* gen_knight_moves(move* m,position* p,square_t from,bool caps,bool noncaps);
move* gen_bishop_moves(move* m,position* p,bool caps,bool noncaps);
move* gen_bishop_moves(move* m,position* p,square_t from,bool caps,bool noncaps);
move* gen_rook_moves(move* m,position* p,bool caps,bool noncaps);
move* gen_rook_moves(move* m,position* p,square_t from,bool caps,bool noncaps);
move* gen_queen_moves(move* m,position* p,bool caps,bool noncaps);
move* gen_queen_moves(move* m,position* p,square_t from,bool caps,bool noncaps);
move* gen_king_moves(move* m,position* p,bool caps,bool noncaps);
move* gen_pawn_moves(move* m,position* p,bool caps,bool noncaps);
move* gen_moves(move *m,position *p,bool caps,bool noncaps);
move* gen_legal_moves(move *m,position *p,bool caps,bool noncaps);


// mvvlva
int32 mvvlva(position *pos,move mv);

// next
bool next(position *pos,move **m,move_order_dto *mo);
bool best_at_top(move *startp,move *endp);
bool place_move_on_top(move *start,move *end,move top_move);
bool good_move(position *p,move m);

// perft
uint64 perft_iterate(int32 max_depth);
void perft_test_fast();
void perft_test_slow(int32 depth);


// pos
bool equal_pos(position* p1,position* p2,bool strict);
void add_piece(position *p,int32 piece,square_t sq);
piece_t remove_piece(position* p,square_t sq);
bool can_castle_wk(position *pos);
bool can_castle_wq(position *pos);
bool can_castle_bk(position *pos);
bool can_castle_bq(position *pos);
void remove_rook_castling_availability(position *p,square_t sq);
void remove_castling_availability(position *p,move mv);
void reset_pos(position *p);
bool set_pos(position *p,const char *fen);
bool is_empty_sq(position *pos,square_t sq);
bool is_enemy_occupied(position *pos,square_t sq,color_t player);
bool is_friendly_occupied(position *pos,square_t sq,color_t player);
bool in_check(position *pos,color_t player);
void flip_board(position *pos);
bool is_checkmate(position *pos);
bool is_stalemate(position *pos);
bool is_zugzwang(position *pos);

// prune
bool prune(position *pos,move last_move,bool incheck,bool gives_check,
		int32 extensions,int32 alpha,int32 beta,int32 depth);

// search
bool is_abort_search();
void set_abort_search(bool abort_search);
bool is_analysis_mode();
void set_analysis_mode(bool analysis_mode);
int32 get_start_time();
void set_start_time(int32 start_time);
void set_stop_time(int32 stop_time);
void set_abort_iterator(bool abort_iterator);

int32 search(position *p,move_line *pv,int32 alpha,int32 beta,int32 ply,
		search_stats *stats,move *move_stack,undo *undo_stack,bool show_thinking);
int32 qsearch(position *pos,int32 alpha,int32 beta,int32 ply,int32 qply,bool incheck,
		search_stats *stats,move *startp,undo *undo_stack);

// see
int32 see(position *pos,move mv);

// square
dir_t get_dir(square_t from,square_t to);
rank_t get_rank(square_t sq);
file_t get_file(square_t sq);
square_t get_square(rank_t rank,file_t file);
int32 rank_distance(square_t sq1,square_t sq2);
int32 file_distance(square_t sq1,square_t sq2);
int32 distance(square_t sq1,square_t sq2);
square_t north(square_t sq);
square_t south(square_t sq);
square_t east(square_t sq);
square_t west(square_t sq);
square_t northeast(square_t sq);
square_t southeast(square_t sq);
square_t southwest(square_t sq);
square_t northwest(square_t sq);
bool is_light_sq(square_t sq);
bool is_dark_sq(square_t sq);
bool is_diagonal(square_t sq1,square_t sq2);
void init_directions();

// suites
void test_suite(char *tsfile,int32 tstime);

// think
void set_time_remaining(int32 time_remaining);
void set_increment(int32 increment);
void set_max_depth(int32 max_depth);
void set_max_time(int32 max_time);
void set_pondering_enabled(bool pondering_enabled);
void set_post(bool post);
bool is_pondering();
void stop_pondering();
move get_ponder_move();
void calculate_search_times();
void think();
move_line iterate(position *pos,bool test_suite_mode);

// time
int32 milli_timer();
void get_time(char* buffer);
int32 get_search_time(int32 remaining_time,int32 increment);

// utils
color_t opposite_player(color_t player);
square_t str_to_sq(const char *str_sq);
void sq_to_str(square_t sq,char *buffer);
void move_to_str(move mv,char *buffer);
move str_to_move(const char* str_mv,position *pos);
bool is_line_valid(move_line *line,position *pos);
bool is_legal_move(move mv,position *pos);
bool is_in_move_list(move mv,move *start,move *end);
uint32 num_moves(position *pos,bool caps,bool noncaps);
uint32 num_moves(move *startp,move *endp,bool caps,bool noncaps);
bool is_white_piece(int32 piece);
bool is_black_piece(int32 piece);
bool is_not_white_piece(int32 piece);
bool is_not_black_piece(int32 piece);
game_status get_game_status();
void line_to_str(move_line *line,char *line_buffer);
void print_move(move mv);
uint64 random64();
bool undo_stacks_equal(undo* stack1, undo* stack2,int32 num_elements);

// xboard
void print_thinking_output(move_line* line,int32 depth,int32 score,int32 start_time,uint64 nodes);
void print_result(game_status gs);

//// tests
void test_attack();
void test_apply();
void test_bitboards();
bool test_book();
void test_defs();
void test_draw();
void test_eval();
bool test_eval_symmetry(int32 score,position *pos);
void test_evalp();
void test_extend();
void test_hash();
void test_init();
void test_magic();
void test_move();
void test_movegen();
void test_next();
void test_pos();
bool test_pos_consistency(position *pos);
void test_prune();
void test_mvvlva();
void test_search();
void test_see();
void test_square();
void test_time();
void test_utils();

#endif /* DEFS_H_ */
