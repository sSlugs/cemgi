#ifndef CHANNEL_H
#define CHANNEL_H
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

//
// definitions for channel struct and enum
//

typedef enum {
    NullCommand
} Command;

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

// returns whether channel was cleared
bool TryClearChannel(Channel *self);

void FreeChannel(Channel *self) {
    free(self->queue);
}

#endif
