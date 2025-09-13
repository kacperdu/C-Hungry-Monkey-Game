#include <stdlib.h>
#include <string.h>
#include "hungry_monkey.h"

// number of rows from top to bottom
#define MAX_PATH FIELD_HEIGHT

struct monkey_state_s {
    int planned_moves[MAX_PATH];  // path of planned moves
    int step_index;               // which step we’re on in path
    int path_length;              // total path steps
};


int simulate_path(int field[][FIELD_WIDTH], int start_col, int moves[], int *path_len) {
    int score = 0;
    int col = start_col;

    // Simulate from top to bottom
    for (int row = 0; row < FIELD_HEIGHT; row++) {
        if (field[row][col] == TREAT_VAL)
            score++;

        // Try to decide next move for max future score
        int best_move = MOVE_FWD;
        int best_score = 0;

        for (int move = -1; move <= 1; move++) {
            int next_col = col + move;
            if (next_col < 0 || next_col >= FIELD_WIDTH)
                continue;

            if (field[row + 1][next_col] == TREAT_VAL)
                if (1 > best_score) {
                    best_score = 1;
                    best_move = move;
                }
        }

        if (moves) moves[row] = best_move;
        col += best_move;

        // Clip column to bounds
        if (col < 0) col = 0;
        if (col >= FIELD_WIDTH) col = FIELD_WIDTH - 1;
    }

    if (path_len) *path_len = FIELD_HEIGHT;
    return score;
}

struct monkey_action move_monkey(int field[][FIELD_WIDTH], void *monkey_state) {
    struct monkey_state_s *state;

    // Allocate state on first call
    if (monkey_state == NULL) {
        state = malloc(sizeof(struct monkey_state_s));
        state->step_index = 0;
        state->path_length = 0;
    } else {
        state = (struct monkey_state_s *)monkey_state;
    }

    // Find monkey’s current column
    int monkey_col = -1;
    for (int col = 0; col < FIELD_WIDTH; col++) {
        if (field[FIELD_HEIGHT - 1][col] == MONKEY_VAL) {
            monkey_col = col;
            break;
        }
    }

    // If no plan or we've reached the end of the plan, computing  new one
    if (state->step_index >= state->path_length) {
        int best_path[MAX_PATH];
        int best_score = -1;
        int best_path_len = 0;

        // Try all columns as starting point, find the best scoring path
        for (int col = 0; col < FIELD_WIDTH; col++) {
            int temp_path[MAX_PATH];
            int path_len = 0;
            int score = simulate_path(field, col, temp_path, &path_len);

            // Check if we can reach this column in time
            if (abs(col - monkey_col) <= FIELD_HEIGHT - 1) {
                if (score > best_score) {
                    best_score = score;
                    best_path_len = path_len;
                    memcpy(best_path, temp_path, sizeof(int) * path_len);
                }
            }
        }

        // Save the best path
        memcpy(state->planned_moves, best_path, sizeof(int) * best_path_len);
        state->path_length = best_path_len;
        state->step_index = 0;
    }

    // Get the next move in the path
    int move = MOVE_FWD;
    if (state->step_index < state->path_length) {
        move = state->planned_moves[state->step_index];
        state->step_index++;
    }

    // Return the action and updated state
    struct monkey_action action;
    action.move = move;
    action.state = state;
    return action;
}
