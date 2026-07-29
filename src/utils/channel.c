#include "utils/channel.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//
// Code for channel struct
// 

void sleep_ms(long milliseconds) {
    struct timespec ts;

    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;

    nanosleep(&ts, NULL);
}

Channel NewChannel(size_t cap) {
    if (cap < 1) {
	cap = 1;
    }
    int element_size = sizeof(Command);

    Command *queue_ptr = calloc(cap,element_size);

    if (queue_ptr == NULL) {
	printf("Null malloc when initlizing queue for channel");
    }

    Channel chnl = {
	.element_size = element_size,
	.capacity = cap,	
	.head = 0,
	.tail = 0,
	.queue = queue_ptr,
	.lock = 0,
    };

    return chnl;
}

bool TrySendChannel(Command cmd, Channel *self) {
    // attempt to secure the lock
    bool own_lock = false;
    for (int i = 0; i < 10; i++) {
	int old_lock = atomic_exchange(&self->lock, 1);
	if (old_lock == 0) {
	    own_lock = true;
	    break;
	}
	sleep_ms(1);
    }

    // fail if we coulnt secure the lock
    if (own_lock == false)
	return false;

    if (self->queue == NULL || self->full) {
	atomic_store(&self->lock, 0);
	return false;
    }

    // write command to ring buffer
    size_t write_index = self->tail;
    self->queue[write_index] = cmd;
    size_t new_write_index = (write_index + 1) % self->capacity;

    // if we are full
    if (new_write_index == self->head) {
	self->full = true;

	// drop lock
	atomic_store(&self->lock, 0);
	
	return false;
    } else {
	self->tail = new_write_index;

	// drop lock
	atomic_store(&self->lock, 0);
	
	return true;
    }
}

Command TryRecvChannel(Channel *self) {
    Command retcmd = NullCommand;

    // attempt to secure the lock
    bool own_lock = false;
    for (int i = 0; i < 10; i++) {
	int old_lock = atomic_exchange(&self->lock, 1);
	if (old_lock == 0) {
	    own_lock = true;
	    break;
	}
	sleep_ms(1);
    }

    // fail if we coulnt secure the lock
    if (own_lock == false)
	return NullCommand;

    if (self->queue == NULL) {
	atomic_store(&self->lock, 0);
	return NullCommand;
    }

    // read from buffer
    Command read = self->queue[self->head];
    
    // if we got null command then simply drop lock and dont update channel
    if (read != NullCommand) {
	retcmd = read;
	self->queue[self->head] = NullCommand;
	size_t new_head = (self->head + 1) % self->capacity;
	self->head = new_head;
	self->full = false;
    }

    // drop lock
    atomic_store(&self->lock, 0);
    
    return retcmd;
}

bool TryClearChannel(Channel *self) {
    // attempt to secure the lock
    bool own_lock = false;
    for (int i = 0; i < 10; i++) {
	int old_lock = atomic_exchange(&self->lock, 1);
	if (old_lock == 0) {
	    own_lock = true;
	    break;
	}
	sleep_ms(1);
    }

    // fail if we coulnt secure the lock
    if (own_lock == false)
	return false;

    if (self->queue == NULL) {
	atomic_store(&self->lock, 0);
	return false;
    }

    // clear buffer
    for (int i = 0; i < self->capacity; i++) {
	self->queue[i] = NullCommand;
    }
    self->head = 0;
    self->tail = 0;

    // drop lock
    atomic_store(&self->lock, 0);

    return true;
}
