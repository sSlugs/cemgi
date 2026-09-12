#include "tests.h"
#include "board/board.h"
#include "perft.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

static int char_to_digit(char ch) {
    switch (ch) {
	case '0':
	    return 0;

	case '1':
	    return 1;

	case '2':
	    return 2;

	case '3':
	    return 3;

	case '4':
	    return 4;

	case '5':
	    return 5;

	case '6':
	    return 6;

	case '7':
	    return 7;

	case '8':
	    return 8;

	default:
	    return 9;
    }
}

MovegenTestCase get_next_movegen_testcase(int fd) {
    MovegenTestCase movegen_case = {0};
    movegen_case.is_null = true;

    if (fd == -1)
	return movegen_case;

    // put newline into line
    char line[512] = {0};

    ssize_t bytes_read;
    int char_pointer = 0;

    while ((bytes_read = read(fd, line + char_pointer, 1)) > 0) { 
	if ((line + char_pointer)[0] == '\n') {
	    // remove the \n from the string
	    (line + char_pointer)[0] = 0;
	    break;
	}

	char_pointer += 1;
    }

    if (bytes_read < 1) {
	movegen_case.null_type = bytes_read + 1;
	return movegen_case;
    }

    // if all passes parse line into a new movegen test_case
    
    // verify we have fen string at start

    // checks if its comment "//" or empty
    if (((line[0] == '/') && (line[1] == '/')) || (line[0] == '\0')) {
	movegen_case.null_type = IgnoreLine;
	return movegen_case;
    }

    char *first_ap = strchr(line, '"');
    if (first_ap == NULL) {
	movegen_case.null_type = ParseFail;
	return movegen_case;
    }

    char *fen_pointer = &first_ap[1];

    char *second_ap = strchr(&first_ap[1], '"');
    if (second_ap == NULL) {
	movegen_case.null_type = ParseFail;
	return movegen_case;
    }

    // turn the fen spaces into * so strtok doesnt fuck things up
    for (char *temp_fen_pointer = fen_pointer; temp_fen_pointer < second_ap; temp_fen_pointer++) {
	if (*temp_fen_pointer == ' ') {
	    *temp_fen_pointer = '*';
	}
    }

    char *token = strtok(line, " ");
    int section = 0;

    // go through all lines
    while (token != NULL) {
	switch (section) {
	    // fen string
	    case 0:

		// turn the *s back into spaces
		for (char *temp_fen_pointer = fen_pointer; temp_fen_pointer < second_ap; temp_fen_pointer++) {
		    if (*temp_fen_pointer == '*') {
			*temp_fen_pointer = ' ';
		    }
		}

		memcpy(movegen_case.fen,&token[1],strlen(token) - 2);

		break;

	    // depth
	    case 1:
		{
		    int depth = 0;
		    size_t len = strlen(token);
		    for (size_t i = 0; i < len; i++) {
			depth += char_to_digit(token[i]);
			if (i != len -1)
			    depth *= 10;
		    }
		    movegen_case.depth = depth;
		}
		break;

	    // expected leaf nodes
	    case 2:
		{
		    u64 nodes = 0;
		    size_t len = strlen(token);
		    for (size_t i = 0; i < len; i++) {
			nodes += char_to_digit(token[i]);
			if (i != len -1)
			    nodes *= 10;
		    }
		    movegen_case.expected_leaf_nodes = nodes;
		}
		break;

	    default:
		break;
	}

	section++;
	token = strtok(NULL, " ");
    }

    // section will be 0 if theres no tokens meaning empty line
    if (section == 0) {
	movegen_case.null_type = IgnoreLine;
	return movegen_case;
    }

    movegen_case.is_null = false;
    return movegen_case;
}

// tests our movegen to see if theres any errors using tests/movegen.tests
void test_movegen() {
    MovegenTestCase testcase;

    int fd = open("tests/movegen.tests", O_RDONLY);

    if (fd == -1){
	printf("test failed to start. couldnt open tests/movegen.tests\n");
	return;
    }

    int i = 0;
    while (1) {
	testcase = get_next_movegen_testcase(fd);
	if (testcase.is_null) {
	   if (testcase.null_type == EndofFile)  {
	       break;
	   } else if (testcase.null_type == IgnoreLine) {
		continue;
	   } else {
	       printf("tests/movegen.tests exited unsafely. do not trust tests\n");
	       return;
	   }
	}

	Board board = BoardFEN(testcase.fen);

	u64 nodes = Perft(&board, testcase.depth);

	if (nodes == testcase.expected_leaf_nodes) {
	    printf("test %d passed...   ",i);
	    printf("nodes: %llu == expected nodes: %llu\n",nodes,testcase.expected_leaf_nodes);
	} else {
	    printf("test %d failed...   ",i);
	    printf("nodes: %llu == expected nodes: %llu\n",nodes,testcase.expected_leaf_nodes);
	    return;
	}
	i++;
    }

    printf("reached end of tests file.\n");

    return;
}
