Sudoku Solver
=============

Graph coloring based sudoku solver. Yes, I could just bruteforce it in a simpler way, but this is cooler.

Accepts input from stdin, puzzles should be one line with 0s to mark blank cells, like this:

    530070000600195000098000060800060003400803001700020006060000280000419005000080079 

You can run the examples by building the program with make, then something like:

    cat sample_easy | ./solver

It will return the solved puzzle in the same format, or error out saying it cannot color the graph. Currently, this solver only supports 9x9 puzzles and will expect 81 characters to be read from stdin.



