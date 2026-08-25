#include "movegen/move.h"
#include "board/board.h"
#include "utils/bitboard.h"
#include "utils/types.h"

static char COLS_TO_LETTERS[8] = {'a','b','c','d','e','f','g','h'};
static char ROWS_TO_NUMS[8] = {'1','2','3','4','5','6','7','8'};

//
// Move stuff
//

State MakeMove(Board *board, Move move) {
    // get useful data
    Colour turn = board->turn;
    Square source = move & SOURCE;
    Square dest = (move & DEST) >> 6;
    Piece source_piece = board->mailbox[source];
    u16 flag = move & MOVE_FLAGS;

    // remove piece on source square
    BoardClearSq(board, source, source_piece);

    // setup state
    State state = NULL_PIECE << 24;
    StateWriteCastleRights(&state, board->castle_rights);
    StateWriteHalfMoveClock(&state, board->halfmove_clock);
    StateWriteEpTargetSquare(&state, board->enpassant_tsq);

    // reset and set some values
    board->enpassant_tsq = NullSquare;
    board->halfmove_clock++;

    // modify board depending on what type of move it is
    switch (flag) {
	case QUIET_MOVE:
	    BoardSetSq(board, dest, source_piece);
	    break;

	case SINGLE_PAWN_PUSH:
	    BoardSetSq(board, dest, source_piece);
	    board->halfmove_clock = 0;
	    break;

	case DOUBLE_PAWN_PUSH:
	    BoardSetSq(board, dest, source_piece);
	    board->halfmove_clock = 0;
	    board->enpassant_tsq = dest - (8 - (16 * turn));
	    break;
	
	case KING_CASTLE:
	    {
		Piece rook = ROOK | (BLACK_PIECE * turn);
		BoardSetSq(board, dest, source_piece);
		BoardClearSq(board, dest + 1, rook);
		BoardSetSq(board, dest - 1, rook);
		break;
	    }

	case QUEEN_CASTLE:
	    {
		Piece rook = ROOK | (BLACK_PIECE * turn);
		BoardSetSq(board, dest, source_piece);
		BoardClearSq(board, dest - 2, rook);
		BoardSetSq(board, dest + 1, rook);
		break;
	    }

	case CAPTURE:
	    {
		Piece dest_piece = board->mailbox[dest];
		BoardClearSq(board, dest, dest_piece);
		BoardSetSq(board, dest, source_piece);
		StateWriteCapturedPiece(&state, dest_piece);
		board->halfmove_clock = 0;
		break;
	    }
	
	case EP_CAPTURE:
	    BoardSetSq(board, dest, source_piece);
	    BoardClearSq(board, dest - (8 - (16 * turn)), PAWN | (BLACK_PIECE * InverseColour(turn)));
	    StateWriteCapturedPiece(&state, PAWN | (BLACK_PIECE * InverseColour(turn)));
	    board->halfmove_clock = 0;
	    break;

	case KNIGHT_PROMO_CAPTURE:
	    {
		Piece dest_piece = board->mailbox[dest];
		BoardClearSq(board, dest, dest_piece);
		StateWriteCapturedPiece(&state, dest_piece);
	    }
	case KNIGHT_PROMO:
	    BoardSetSq(board, dest, KNIGHT | (BLACK_PIECE * turn));
	    board->halfmove_clock = 0;
	    break;

	case BISHOP_PROMO_CAPTURE:
	    {
		Piece dest_piece = board->mailbox[dest];
		BoardClearSq(board, dest, dest_piece);
		StateWriteCapturedPiece(&state, dest_piece);
	    }
	case BISHOP_PROMO:
	    BoardSetSq(board, dest, BISHOP | (BLACK_PIECE * turn));
	    board->halfmove_clock = 0;
	    break;

	case ROOK_PROMO_CAPTURE:
	    {
		Piece dest_piece = board->mailbox[dest];
		BoardClearSq(board, dest, dest_piece);
		StateWriteCapturedPiece(&state, dest_piece);
	    }
	case ROOK_PROMO:
	    BoardSetSq(board, dest, ROOK | (BLACK_PIECE * turn));
	    board->halfmove_clock = 0;
	    break;

	case QUEEN_PROMO_CAPTURE:
	    {
		Piece dest_piece = board->mailbox[dest];
		BoardClearSq(board, dest, dest_piece);
		StateWriteCapturedPiece(&state, dest_piece);
	    }
	case QUEEN_PROMO:
	    BoardSetSq(board, dest, QUEEN | (BLACK_PIECE * turn));
	    board->halfmove_clock = 0;
	    break;

    }

    // castle rights check
    u8 cr = board->castle_rights;

    // loop through active castle rights and run code accordingly
    while (cr) {
	int right = LsbIndexU8(cr);

	switch (right) {
	    // if you have WQC right attempt to prove you lost rights this move
	    case 0:
		if (board->mailbox[4] != KING) {
		    board->castle_rights &= ~(WQC | WKC);
		} else if (board->mailbox[0] != ROOK)
		    board->castle_rights &= ~WQC;
		break;

	    // WKC
	    case 1:
		if (board->mailbox[4] != KING) {
		    board->castle_rights &= ~(WQC | WKC);
		    break;
		} else if (board->mailbox[7] != ROOK)
		    board->castle_rights &= ~WKC;
		break;

	    // BQC
	    case 2:
		if (board->mailbox[60] != (KING | BLACK_PIECE)) {
		    board->castle_rights &= ~(BQC | BKC);
		    break;
		} else if (board->mailbox[56] != (ROOK | BLACK_PIECE))
		    board->castle_rights &= ~BQC;
		break;

	    // BQC
	    case 3:
		if (board->mailbox[60] != (KING | BLACK_PIECE)) {
		    board->castle_rights &= ~(BQC | BKC);
		    break;
		} else if (board->mailbox[63] != (ROOK | BLACK_PIECE))
		    board->castle_rights &= ~BKC;
		break;
	}

	PopLsbU8(&cr);
    }

    // final data modifications at end
    board->turn = InverseColour(turn);
    return state;
}

void UndoMove(Board *board, Move move, State state) {
    // undo move by reversing order, reading state, and moving pieces back
    board->turn = InverseColour(board->turn);
    // useful data
    Colour turn = board->turn;
    Square source = move & SOURCE;
    Square dest = (move & DEST) >> 6;
    Piece dest_piece = board->mailbox[dest];
    u16 flag = move & MOVE_FLAGS;

    // write the state to our board
    board->castle_rights = StateReadCastleRights(state);
    board->enpassant_tsq = StateReadEpTargetSquare(state);
    board->halfmove_clock = StateReadHalfMoveClock(state);

    BoardClearSq(board, dest, dest_piece);

    // move our piece back and clear where we landed
    switch (flag) {
	case QUIET_MOVE:
	case SINGLE_PAWN_PUSH:
	case DOUBLE_PAWN_PUSH:
	    BoardSetSq(board, source, dest_piece);
	    break;

	case KING_CASTLE:
	    {
		Piece rook = ROOK | (BLACK_PIECE*turn);
		BoardClearSq(board, dest - 1, rook);
		BoardSetSq(board, source, dest_piece);
		BoardSetSq(board, 7 + 56*turn, rook);
		break;
	    }

	case QUEEN_CASTLE:
	    {
		Piece rook = ROOK | (BLACK_PIECE*turn);
		BoardClearSq(board, dest + 1, rook);
		BoardSetSq(board, source, dest_piece);
		BoardSetSq(board, 56*turn, rook);
		break;
	    }

	case CAPTURE:
	    BoardSetSq(board, source, dest_piece);
	    BoardSetSq(board, dest, StateReadCapturedPiece(state));
	    break;

	case EP_CAPTURE:
	    BoardSetSq(board, source, dest_piece);
	    BoardSetSq(board, dest - (8 - 16*turn), StateReadCapturedPiece(state));
	    break;
	
	case KNIGHT_PROMO_CAPTURE:
	    BoardSetSq(board, dest, StateReadCapturedPiece(state));
	case KNIGHT_PROMO:
	    BoardSetSq(board, source, PAWN | (BLACK_PIECE*turn));
	    break;

	case BISHOP_PROMO_CAPTURE:
	    BoardSetSq(board, dest, StateReadCapturedPiece(state));
	case BISHOP_PROMO:
	    BoardSetSq(board, source, PAWN | (BLACK_PIECE*turn));
	    break;

	case ROOK_PROMO_CAPTURE:
	    BoardSetSq(board, dest, StateReadCapturedPiece(state));
	case ROOK_PROMO:
	    BoardSetSq(board, source, PAWN | (BLACK_PIECE*turn));
	    break;

	case QUEEN_PROMO_CAPTURE:
	    BoardSetSq(board, dest, StateReadCapturedPiece(state));
	case QUEEN_PROMO:
	    BoardSetSq(board, source, PAWN | (BLACK_PIECE*turn));
    }

}

void MoveToUci(Move move, char str[6]) {
    // add source square first  
    int source = (move & SOURCE);
    int dest = ((move & DEST) >> 6);

    str[0] = COLS_TO_LETTERS[source % 8];
    str[1] = ROWS_TO_NUMS[source / 8];
    
    str[2] = COLS_TO_LETTERS[dest % 8];
    str[3] = ROWS_TO_NUMS[dest / 8];

    // avoids checking promotions rn 
    u16 flag = move & MOVE_FLAGS;
    int index = 4;

    switch (flag) {
	case KNIGHT_PROMO_CAPTURE:
	case KNIGHT_PROMO:
	    str[4] = 'n';
	    index = 5;
	    break;

	case BISHOP_PROMO_CAPTURE:
	case BISHOP_PROMO:
	    str[4] = 'b';
	    index = 5;
	    break;

	case ROOK_PROMO_CAPTURE:
	case ROOK_PROMO:
	    str[4] = 'r';
	    index = 5;
	    break;

	case QUEEN_PROMO_CAPTURE:
	case QUEEN_PROMO:
	    str[4] = 'q';
	    index = 5;
	    break;
    }

    str[index] = '\0';
}

//
// Movelist stuff
//

MoveList MoveListNew(void) {
    MoveList list;
    list.len = 0;
    return list;
}
