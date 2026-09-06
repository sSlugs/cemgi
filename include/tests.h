#ifndef TESTS_H
#define TESTS_H

#include "globals.h"
#include <stdbool.h>

// NullType, enum for type of null a testcase can be
typedef enum {
    OtherError,
    EndofFile,
    ParseFail,
    // comments, and empty lines should be ignored
    IgnoreLine,
} NullType;

// MovegenTestCase, the struct that holds all information about how to test a position
typedef struct {
    char fen[512];
    int depth;
    u64 expected_leaf_nodes;
    bool is_null;
    NullType null_type;
} MovegenTestCase;

MovegenTestCase get_next_movegen_testcase(int fd);

void test_movegen();

#endif
