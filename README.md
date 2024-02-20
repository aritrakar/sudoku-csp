# Python Sudoku Solver

## Introduction

This project presents a Sudoku solver implemented in Python, modeled as a Constraint Satisfaction Problem (CSP). Leveraging Python's rich ecosystem for computational problems, this solver demonstrates efficient puzzle solving through backtracking, forward checking, and different heuristics.

## Setup and Execution

### Prerequisites

- C++17 OR
- Python 3.8+

### Running the Solver

```bash
# Clone the repository
git clone https://
cd sudoku-csp

# For the C++ solution
# Compile
g++ -std=c++17 -03 sudoku_solver.cpp -o sudoku
./sudoku 2 # Mode 2. See below for details.
```

## Implementation Details

There are 2 modes and 3 approaches to solve the Sudoku puzzle. The mode and approach can be specified as command-line arguments. The approahces are:

- Backtracking (1): `b`
- Backtracking + Forward Checking (2): `btfc`
- Backtracking + Forward Checking + Heuristics (3): `btfch`

The modes are:

- Mode 1: Solve a single puzzle with a specified approach.

  ```bash
      ./sudoku 1 <puzzle> <approach>
  ```

- Mode 2: Solve a single puzzle with all approaches and compare the results. By default, it'll search in the `etc` directory.
  ```bash
      ./sudoku 2 <puzzle>
  ```

### Modeling Sudoku as a CSP

Before diving into each technique, let's first model sudoku as a constraint satisfaction problem (CSP) as follows:

- **Variables (V):** Each cell in the grid is a variable. For a standard 9x9 Sudoku, there are 81 variables. $$V = \{v_1, v_2, \ldots, v_{81}\} \quad \text{or} \quad V = \{v_{ij}: i, j \in [1, 9]\}$$
- **Domain (D):** The domain of each variable is the set of possible values that can be assigned to it. For a standard 9x9 Sudoku, the domain of each variable is {1, 2, 3, 4, 5, 6, 7, 8, 9}.
- **Constraints (C):** The constraints are the rules of Sudoku. No two cells in the same row, column, or 3x3 subgrid can contain the same value.

### Backtracking for sudoku

Backtracking is a brute-force search algorithm that systematically searches for a solution to a problem. It explores all possible paths to find a solution. If a path leads to a dead end, the algorithm backtracks and tries a different path.

Backtracking is not efficient for solving Sudoku, but it is a good starting point to understand the problem and the solution space.

Note: There is a major difference between a standard backtracking approach and the approach used in my solution. A standard backtracking solution would fill cells sequentially, starting from the top-left corner and in the domain order (i.e. from 1 to 9). However, my solution uses **randomizes** the order for both of these. Certain puzzles can be solved by backtracking using the traditional approach due to the ordering of the cells and the domain. However, randomizing the order of the cells and the domain makes the problem more challenging.

<!-- The backtracking function can be found [here](). -->

### Backtracking + Forward Checking for sudoku

Since backtracking is not efficient for solving Sudoku, we can use forward checking to improve the efficiency.

Forward checking is a simple and efficient _constraint propagation_ technique that eliminates values from the domain of variables that have been assigned a value. It is a way to keep track of remaining legal values for unassigned variables. Whenever a variable is assigned a value, the forward checking algorithm removes that value from the domain of all the variables connected to the variable that was just assigned. This helps in reducing the search space by eliminating values that cannot be part of the solution and checking for failure early.

<!-- The backtracking + forward checking function can be found [here](). -->

### Backtracking + Forward Checking + Heuristics for sudoku

Lastly, we can use heuristics to further improve the efficiency of the solver. Heuristics are rules or methods that help in choosing the most promising path. I use the **most constrained variable**, **most constraining variable**, and **least constraining value** heuristics.

- First, the **most constrained variable** heuristic selects the variable that is involved in the largest number of constraints on the remaining variables. This is the variable that is most likely to cause a failure, and it is therefore a good variable to assign a value to early.

- Second, in case of ties in the **most constrained variable** heuristic, the most constraining variable heuristic is used. This heuristic selects the variable that is involved in the largest number of constraints on the remaining variables.

- Lastly, the **least constraining value** heuristic is used to select the value to assign to the variable. This heuristic selects the value that rules out the fewest choices for the neighboring variables in the constraint graph.

## Experiiment results

The solver was tested on 4 different puzzles difficulties (easy, medium, hard, and evil) for 10 iterations each. Statistics about the time taken to solve each puzzle and the number of nodes expanded in the search tree were recorded. The results are shown below for each approach.

Note:

- The results are not deterministic due to the randomization of the order of cells and domain.
- The results are presented as the mean ± standard deviation.
- Initially, I tried running with the Python solution and with a higher number of iterations, but it was taking too long. So, I switched to C++. The C++ solution is much faster than the Python solution.

### Time taken to solve the puzzle in milliseconds

| Difficulty | btfch            | btfc             | b                 |
| ---------- | ---------------- | ---------------- | ----------------- |
| Easy       | 1.8 ± 2.13542    | 7.5 ± 20.5098    | 124.6 ± 106.516   |
| Medium     | 6.7 ± 5.56866    | 9.5 ± 9.46837    | 2417.8 ± 4476.78  |
| Hard       | 192.9 ± 147.634  | 403.5 ± 389.354  | 77080.3 ± 99295.3 |
| Evil       | 2068.6 ± 1253.96 | 5229.3 ± 6034.34 | 142869 ± 284326   |

### Number of nodes expanded in the search tree

| Difficulty | btfch             | btfc                      | b                         |
| ---------- | ----------------- | ------------------------- | ------------------------- |
| Easy       | 815.1 ± 695.224   | 4565.9 ± 11716.6          | 388242 ± 327441           |
| Medium     | 2236.2 ± 1701.42  | 5327.6 ± 4810.94          | 7.7055e+06 ± 1.42765e+07  |
| Hard       | 61352.3 ± 46844.4 | 237808 ± 230248           | 2.44967e+08 ± 3.16293e+08 |
| Evil       | 645198 ± 389603   | 2.98515e+06 ± 3.47978e+06 | 4.47678e+08 ± 8.88712e+08 |

## Conclusion

The results show that the `btfch` approach is the most efficient, as expected, in terms of time taken to solve the puzzle and the number of nodes expanded in the search tree. The `btfc` approach is also efficient, but it is not as efficient as the `btfch` approach. The standard backtracking approach is not efficient for solving Sudoku puzzles (at least with randomized order of cells and domain), as expected.
