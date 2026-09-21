#include "uci/uci.h"
#include "board/board.h"
#include "globals.h"
#include "movegen/move.h"
#include "utils/channel.h"
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int UciMainLoop(ThreadInterface *interface) {
    char buffer[1024];
    Channel *tx = &interface->channel;

    while (1) {
	fgets(buffer, sizeof(buffer), stdin);
	buffer[strlen(buffer) - 1] = 0;

	//
	// Non multi-word commands
	//
	
	if (strcmp(buffer,"quit") == 0) {
	    Command cmd = NewNullCommand();
	    cmd.type = Quit;
	    if (!TrySendChannel(cmd, tx)) {
		printf("Failed to send 'Quit' command to search thread. Ending proccess.\n");
		break;
	    }
	    break;
	} 

	else if (strcmp(buffer,"help") == 0) {
	    printf("\nCemgi is a weak chess engine for playing but not probably not analyzing.\n\
Cemgi is normally used with a graphical user interface (GUI) and implements\n\
the Universal Chess Interface (UCI) protocol to communicate with a GUI, an API, etc.\n\
For any further information, i am sorry.\n\n");
	} 

	else if (strcmp(buffer,"uci") == 0) {
	    printf("uciok\n");
	} 

	else if (strcmp(buffer,"isready") == 0) {
	    printf("readyok\n");
	} 

	else if (strcmp(buffer,"ucinewgame") == 0) {
	    Command cmd = NewNullCommand();
	    cmd.type = UciNewGame;
	    if (!TrySendChannel(cmd, tx)) {
		printf("Failed to send 'UciNewGame' command to search thread. Ending proccess.\n");
		break;
	    }
	} 

	else if (strcmp(buffer,"stop") == 0) {
	    if (atomic_load(&interface->atomic_flags.engine_searching)) {
		atomic_store(&interface->atomic_flags.stop_flag, true);
	    }
	} 

	else if (strcmp(buffer,"flip") == 0) {
	    Command cmd = NewNullCommand();
	    cmd.type = FlipTurn;
	    if (!TrySendChannel(cmd, tx)) {
		printf("Failed to send 'FlipTurn' command to search thread. Ending proccess.\n");
		break;
	    }
	} 

	else if (strcmp(buffer,"eval") == 0) {
	    Command cmd = NewNullCommand();
	    cmd.type = PrintEval;
	    if (!TrySendChannel(cmd, tx)) {
		printf("Failed to send 'PrintEval' command to search thread. Ending proccess.\n");
		break;
	    }
	} 

	else if (strcmp(buffer,"d") == 0) {
	    Command cmd = NewNullCommand();
	    cmd.type = PrintBoard;
	    if (!TrySendChannel(cmd, tx)) {
		printf("Failed to send 'PrintBoard' command to search thread. Ending proccess.\n");
		break;
	    }
	} 

	//
	// Multi-word commands
	//
	
	else {
	    // tokenize buffer
	    char *token = strtok(buffer, " ");
	    if (token != NULL) {
		// compare token then hand off to function
		if (strcmp(token,"go") == 0) {
		    if (!SendGoCommand(tx,token)) {
			printf("Failed to send 'go' command to search thread. Ending proccess.\n");
			break;
		    }
		}
		
		else if (strcmp(token,"position") == 0) {
		    if (!SendPositionCommand(tx,token)) {
			printf("Failed to send 'position' command to search thread. Ending proccess.\n");
			break;
		    }
		}
		
		else if (strcmp(token,"setoption") == 0) {
		    // Unimplemented
		}

		else {
		    printf("Unknown command: '%s'. Type help for more information.\n",buffer);
		}
	    }
	}
    }

    return 0;
}

void *UciThread(void *interface) {
    int r = UciMainLoop((ThreadInterface *)interface);

    if (r != 0) {
	printf("uci loop did not end properly.\n");
    }

    return NULL;
}

bool SendGoCommand(Channel *tx, char *token) {
    token = strtok(NULL, " ");

    Command cmd = ParseGo(token);

    if (!TrySendChannel(cmd, tx))
	return false;

    return true;
}

Command ParseGo(char *token) {
    Command cmd = NewNullCommand();
    cmd.type = Go;

    while (token != NULL) {

	if (strcmp(token,"depth") == 0) {
	    token = strtok(NULL, " ");
	    cmd.go.depth = StringToNumber(token);
	}

	else if (strcmp(token,"movetime") == 0) {
	    token = strtok(NULL, " ");
	    cmd.go.movetime = StringToNumber(token);
	}

	token = strtok(NULL, " ");
    }

    if (cmd.go.depth == 0)
	cmd.go.depth = MAX_PLY - 1;

    return cmd;
}


bool SendPositionCommand(Channel *tx, char *token) {
    token = strtok(NULL, " ");

    Command cmd = ParsePosition(token);

    if (!TrySendChannel(cmd, tx))
	return false;

    return true;
}

Command ParsePosition(char *token) {
    Command cmd = NewNullCommand();
    Board board = BoardNew();
    cmd.type = Position;

    while (token != NULL) {

	if (strcmp(token,"fen") == 0) {
	    token = strtok(NULL, " ");
	    char fen[512] = {0};
	    int fen_pointer = 0;

	    // loop through tokens to fill up fen string.
	    // stop if we see "moves" or token becomes NULL
	    while (token != NULL) {
		if (strcmp(token,"moves") == 0) {
		    board = BoardFEN(fen);
		    goto moves;
		}

		size_t token_len = strlen(token);
		memcpy(&fen[fen_pointer],token,token_len);
		fen_pointer += token_len;
		fen[fen_pointer++] = ' ';
		
		token = strtok(NULL, " ");
	    }
	    board = BoardFEN(fen);
	    break;
	}

	else if (strcmp(token, "moves") == 0) {
	moves:
	    token = strtok(NULL, " ");
	    
	    while (token != NULL) {
		Move move = UciToMove(&board, token);

		if (move == NULL_MOVE) {
		    printf("Incorrect use of 'position'.");
		    return NewNullCommand();
		}

		MakeMove(&board, move);
		
		token = strtok(NULL, " ");
	    }

	}

	token = strtok(NULL, " ");
    }

    cmd.position.board = board;
    return cmd;
}



void *TimingThread(void *timing_thread_args) {
    const u64 check_iteration = 50;

    TimingThreadArgs *args = timing_thread_args;
    u64 total_ms = args->ms;
    u64 intervals = total_ms / check_iteration;
    u64 remainder_ms = total_ms % check_iteration;

    struct timespec check_ts = {
	.tv_sec = 0,
	.tv_nsec = 1000000 * check_iteration
    };

    struct timespec remainder_ts = {
	.tv_sec = 0,
	.tv_nsec = 1000000 * remainder_ms
    };

    for (u64 i = 0; i < intervals; i++) {
	nanosleep(&check_ts, NULL);

	// check if search ended early and wants us to exit
	if (atomic_load(&args->end_timing_early)) {
	    return NULL;
	}
    }

    nanosleep(&remainder_ts, NULL);

    if (atomic_load(&args->interface->engine_searching)) {
	atomic_store(&args->interface->stop_flag, true);
    }

    return NULL;
}
