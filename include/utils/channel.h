#ifndef CHANNEL_H
#define CHANNEL_H
#include "board/board.h"
#include "movegen/move.h"
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

//
// definitions for channel struct and enum
//

typedef enum {
    NullCommand,
    Quit,
    UciNewGame,
    FlipTurn,
    PrintEval,
    PrintBoard,
    Go,
    Position,
} CommandType;

typedef struct {
    int depth;	
    // in milliseconds
    u64 movetime;
} GoArgs;

typedef struct {
    CommandType type;
    union {
	GoArgs go;

	struct {
	    Board board;
	} position;
    };
} Command;

static inline Command NewNullCommand() {
    return (Command){0};
}

typedef struct {
    const char *name;
    Command (*parse)(char **args);
} CommandDef;

typedef struct {
    atomic_bool stop_flag;
    atomic_bool engine_searching;
} AtomicInterface;

typedef struct {
    Command *queue;
    size_t element_size;
    size_t capacity;
    size_t head;
    size_t tail;
    bool full;
    atomic_int lock;
} Channel;

// makes new channel struct
Channel NewChannel(size_t cap);

// returns whether the command passed or not
bool TrySendChannel(Command cmd, Channel *self);

// returns nullcommand if fail
Command TryRecvChannel(Channel *self);

// returns whether channel was cleared succesfully
bool TryClearChannel(Channel *self);

static void FreeChannel(Channel *self) {
    free(self->queue);
}

#endif
