# C-Hungry-Monkey-Game

A simple C programming project where a monkey navigates a 2D grid to collect treats.  
The project showcases pathfinding, arrays, pointers, and dynamic memory in C.

The Game:
- The playing field is a **30x20 2D grid**.
- Cells may contain Treats (`TREAT_VAL`), Monkey (`MONKEY_VAL`) and Empty space.
- The monkey can move left, right, or forward each step.
- The objective is to **collect as many treats as possible**.

Project involves two logics:
`hungry_monkey_printf.c` – Basic strategy* 
   - Plans a path once and follows it until the end.  
   - Replans only when the path is finished.  
`move_monkey.c` – Adaptive strategy  
   - Replans every few steps or when the chosen path is no longer valuable.  
   - More robust in dynamic environments.
     
Both implementations use:
- Arrays to represent the grid and paths.  
- Pointers and dynamic memory for state tracking.  
- Functions for modular and reusable logic

Author:
Kacper Duda
