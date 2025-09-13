#include <stdlib.h>
#include <string.h>
#include "hungry_monkey.h"

#define MAX_PATH FIELD_HEIGHT
#define REPLAN_INTERVAL 5  // Number of steps after which to force a replan


struct monkey_state_s {
    int planned_moves[MAX_PATH];   // The sequence of planned moves (left/right/forward)
    int step_index;                // The current index in the planned move sequence
    int path_length;               // Total number of steps in the current path
    int steps_since_replan;       // How long it's been since we last recomputed a path
};

int simulate_path(int field[][FIELD_WIDTH], int start_col, int moves[], int *path_len) {
    int col = start_col;
    int score = 0;

    for (int row = 0; row < FIELD_HEIGHT; row++) {
        // Score current cell if there's a treat
        if (field[row][col] == TREAT_VAL) score++;

        // Decide next move: stay forward, or move left/right if there's a treat ahead
        int best_move = MOVE_FWD;
        int best_score = 0;

        // Look at the row below to see if treat is reachable next
        for (int move = -1; move <= 1; move++) {
            int next_col = col + move;
            if (next_col < 0 || next_col >= FIELD_WIDTH)
                continue;

            if (field[row + 1][next_col] == TREAT_VAL) {
                // Prioritize direction with a visible treat
                best_score = 1;
                best_move = move;
            }
        }

        // Store the move if caller wants to save the path
        if (moves) moves[row] = best_move;

        // Update column based on chosen move
        col += best_move;

        // Prevent out-of-bounds
        if (col < 0) col = 0;
        if (col >= FIELD_WIDTH) col = FIELD_WIDTH - 1;
    }

    // Return path length
    if (path_len) *path_len = FIELD_HEIGHT;

    return score;
}

struct monkey_action move_monkey(int field[][FIELD_WIDTH], void *monkey_state) {
    struct monkey_state_s *state;

    // Allocate and initialize state on first run
    if (monkey_state == NULL) {
        state = malloc(sizeof(struct monkey_state_s));
        state->step_index = 0;
        state->path_length = 0;
        state->steps_since_replan = 0;
    } else {
        state = (struct monkey_state_s *)monkey_state;
    }

    // Find the current column of the monkey by looking at the bottom row
    int monkey_col = -1;
    for (int col = 0; col < FIELD_WIDTH; col++) {
        if (field[FIELD_HEIGHT - 1][col] == MONKEY_VAL) {
            monkey_col = col;
            break;
        }
    }
    // Determine if we should replan the monkey's path
    int need_replan = 0;
    // If we're out of steps, we need a new plan
    if (state->step_index >= state->path_length) {
        need_replan = 1;
    }
    // Force a replan after REPLAN_INTERVAL steps to adapt to changing field
    if (state->steps_since_replan >= REPLAN_INTERVAL) {
        need_replan = 1;
    }
    // If the next step in the plan goes to a dead column, replan
    if (!need_replan && state->step_index < state->path_length) {
        int next_move = state->planned_moves[state->step_index];
        int future_col = monkey_col + next_move;
        // Clamp column safely
        if (future_col < 0) future_col = 0;
        if (future_col >= FIELD_WIDTH) future_col = FIELD_WIDTH - 1;
        // Check if that column still contains any treats
        int valuable = 0;
        for (int r = 0; r < FIELD_HEIGHT - 1; r++) {
            if (field[r][future_col] == TREAT_VAL) {
                valuable = 1;
                break;
            }
        }
        if (!valuable) need_replan = 1;  // No value in continuing path
    }
    // Compute the best possible path from current position
    if (need_replan) {
        int best_score = -1;
        int best_path[MAX_PATH];
        int best_len = 0;
        // Try simulating paths from all columns
        for (int col = 0; col < FIELD_WIDTH; col++) {
            // Skip unreachable starting columns
            if (abs(col - monkey_col) > FIELD_HEIGHT - 1) continue;

            int temp_path[MAX_PATH];
            int temp_len;
            int score = simulate_path(field, col, temp_path, &temp_len);

            if (score > best_score) {
                best_score = score;
                best_len = temp_len;
                memcpy(best_path, temp_path, sizeof(int) * temp_len);
            }
        }
        // Save the best found path to state
        memcpy(state->planned_moves, best_path, sizeof(int) * best_len);
        state->path_length = best_len;
        state->step_index = 0;
        state->steps_since_replan = 0;
    }
    // Execute the next move in the planned path
    int move = MOVE_FWD;
    if (state->step_index < state->path_length) {
        move = state->planned_moves[state->step_index];
        state->step_index++;
    }
    // Track how long we've been following this path
    state->steps_since_replan++;
    // Return the next move and updated state
    struct monkey_action action;
    action.move = move;
    action.state = state;
    return action;
}
