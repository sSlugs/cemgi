#ifndef UCI_H
#define UCI_H
#include "utils/channel.h"

typedef struct {
    AtomicInterface atomic_flags;
    Channel channel;
} ThreadInterface;

int UciMainLoop(ThreadInterface *args);

void *UciThread(void *args);

typedef struct {
    u64 ms;
    AtomicInterface *interface;
    atomic_bool end_timing_early;
} TimingThreadArgs;

void *TimingThread(void *args);

bool SendGoCommand(Channel *tx,char *token);
Command ParseGo(char *token);

bool SendPositionCommand(Channel *tx,char *token);
Command ParsePosition(char *token);
    
#endif
