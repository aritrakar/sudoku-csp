# Python Sudoku Solver

## Introduction

This project presents a Sudoku solver implemented in Python, modeled as a Constraint Satisfaction Problem (CSP). Leveraging Python's rich ecosystem for computational problems, this solver demonstrates efficient puzzle solving through backtracking, forward checking, and different heuristics.

## Setup and Execution

### Prerequisites

- Python 3.8+

### Running the Solver

To run the Sudoku solver, simply download the IPYNB notebook. A plain Python file is also provided. To use the file, execute the following command in your terminal:

```bash
python sudoku_solver.py
```

## Implementation Details

There are 3 modes:

- Backtracking: `b`
- Backtracking + Forward Checking: `btfc`
- Backtracking + Forward Checking + Heuristics: `btfch`

### Modeling Sudoku as a CSP

Before diving into each technique, let's first model sudoku as a constraint satisfaction problem (CSP) as follows:

- **Variables (V):** Each cell in the grid is a variable. For a standard 9x9 Sudoku, there are 81 variables. $$V = \{v_1, v_2, \ldots, v_{81}\} \quad \text{or} \quad V = \{v_{ij}: i, j \in [1, 9]\}$$
- **Domain (D):** The domain of each variable is the set of possible values that can be assigned to it. For a standard 9x9 Sudoku, the domain of each variable is {1, 2, 3, 4, 5, 6, 7, 8, 9}.
- **Constraints (C):** The constraints are the rules of Sudoku. No two cells in the same row, column, or 3x3 subgrid can contain the same value.

### Backtracking for sudoku

Backtracking is a brute-force search algorithm that systematically searches for a solution to a problem. It explores all possible paths to find a solution. If a path leads to a dead end, the algorithm backtracks and tries a different path.

Backtracking is not efficient for solving Sudoku, but it is a good starting point to understand the problem and the solution space.

Note: There is a major difference between a standard backtracking approach and the approach used in my solution. A standard backtracking solution would fill cells sequentially, starting from the top-left corner and in the domain order (i.e. from 1 to 9). However, my solution uses **randomizes** the order for both of these. Certain puzzles can be solved by backtracking using the traditional approach due to the ordering of the cells and the domain. However, randomizing the order of the cells and the domain makes the problem more challenging.

The backtracking function can be found [here]().

### Backtracking + Forward Checking for sudoku

Since backtracking is not efficient for solving Sudoku, we can use forward checking to improve the efficiency.

Forward checking is a simple and efficient _constraint propagation_ technique that eliminates values from the domain of variables that have been assigned a value. It is a way to keep track of remaining legal values for unassigned variables. Whenever a variable is assigned a value, the forward checking algorithm removes that value from the domain of all the variables connected to the variable that was just assigned. This helps in reducing the search space by eliminating values that cannot be part of the solution and checking for failure early.

The backtracking + forward checking function can be found [here]().

### Backtracking + Forward Checking + Heuristics for sudoku

Lastly, we can use heuristics to further improve the efficiency of the solver. Heuristics are rules or methods that help in choosing the most promising path. I use the **most constrained variable**, **most constraining variable**, and **least constraining value** heuristics.

- First, the **most constrained variable** heuristic selects the variable that is involved in the largest number of constraints on the remaining variables. This is the variable that is most likely to cause a failure, and it is therefore a good variable to assign a value to early.

- Second, in case of ties in the **most constrained variable** heuristic, the most constraining variable heuristic is used. This heuristic selects the variable that is involved in the largest number of constraints on the remaining variables.

- Lastly, the **least constraining value** heuristic is used to select the value to assign to the variable. This heuristic selects the value that rules out the fewest choices for the neighboring variables in the constraint graph.

## Experiiment results

The solver was tested on 4 different puzzles difficulties (easy, medium, hard, and evil) for 10 iterations each. Statistics about the time taken to solve each puzzle and the number of nodes expanded in the search tree were recorded. The results are shown below for each approach:

### Time taken to solve the puzzle in milliseconds

| Difficulty | btfch                                   | btfc                                   | b   |
| ---------- | --------------------------------------- | -------------------------------------- | --- |
| Easy       | 1.890802400012035 ± 9.190758212574265   | 408.7562686799904 ± 2002.4560833995129 | N/A |
| Medium     | 2.7266593200010902 ± 13.28979370587803  | 2095.844647840003 ± 10267.430175684243 | N/A |
| Hard       | 3.465234879995478 ± 16.92438692419921   | 150718.96512020004 ± 738369.0487443672 | N/A |
| Evil       | 2.6524054399988017 ± 12.939423608551735 | N/A                                    | N/A |

### Number of nodes expanded in the search tree

| Difficulty | btfch                     | btfc                           | b   |
| ---------- | ------------------------- | ------------------------------ | --- |
| Easy       | 2.8 ± 8.818163074019441   | 13447.84 ± 65875.79330569306   | N/A |
| Medium     | 4.88 ± 19.008040403997487 | 87181.8 ± 427096.950735263     | N/A |
| Hard       | 5.52 ± 22.143387274759917 | 6459193.12 ± 31643449.68921189 | N/A |
| Evil       | 5.24 ± 20.771673018801355 | N/A                            | N/A |

## Conclusion

The results show that the `btfch` approach is the most efficient, as expected, in terms of time taken to solve the puzzle and the number of nodes expanded in the search tree. The `btfc` approach is also efficient, but it is not as efficient as the `btfch` approach. The standard backtracking approach is not efficient for solving Sudoku puzzles (at least with randomized order of cells and domain), as expected.
