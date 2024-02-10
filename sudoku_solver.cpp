#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <utility>
#include <fstream>
#include <functional>

using namespace std;

typedef vector<vector<int>> Board;
typedef vector<vector<char>> State;

// Get random item from vector
template <class T>
T get_random_item(vector<T> &v)
{
    // unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    // shuffle(v.begin(), v.end(), default_random_engine(seed));
    // return v[0];

    sample(v.begin(), v.end(), v.begin(), 1, mt19937{random_device{}()});
    return v[0];
}

void printBoard(Board &board)
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
            std::cout << board[i][j] << " ";
        std::cout << endl;
    }
}

/*
    Three approaches:
    1. Backtracking with randomized cell and domain ordering
    2. Backtracking + Forward checking with randomized cell and domain ordering
    3. Backtracking + Forward checking + Heuristics with randomized cell and domain ordering
*/

class SudokuSolver
{
public:
    SudokuSolver(Board board, int approach) : board(board), approach(approach) {}

    vector<long> solve()
    {
        // Make a copy of the board
        Board boardCopy = board;

        // if (approach == 1)
        // {
        //     return solve_generic(boardCopy, [&](Board &emptyCells, long &nodesExpanded)
        //                          { return solve_backtrack_helper(board, emptyCells, 0, nodesExpanded); });
        // }

        if (approach == 1)
        {
            return solve_backtrack(boardCopy);
        }
        else if (approach == 2)
        {
            return solve_btfc(boardCopy);
        }
        // else if (approach == 3)
        // {
        //     solve_btfch(board);
        // }

        return {-1};
    }

private:
    Board board;
    int approach;

    // vector<long> solve_generic(Board &board, function<bool(Board &, long &)> solver)
    // {
    //     Board emptyCells = getEmptyCells(board);

    //     // Randomize empty cells
    //     mt19937 rng(random_device{}());
    //     shuffle(emptyCells.begin(), emptyCells.end(), rng);

    //     long nodesExpanded = 0;

    //     // Start timer
    //     auto start = chrono::high_resolution_clock::now();
    //     solver(emptyCells, nodesExpanded);

    //     // End timer
    //     auto end = chrono::high_resolution_clock::now();

    //     // Calculate duration
    //     auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    //     return {duration.count(), nodesExpanded};
    // }

    vector<long> solve_backtrack(Board &board)
    {
        Board emptyCells = getEmptyCells(board);

        // Randomize empty cells
        mt19937 rng(random_device{}());
        shuffle(emptyCells.begin(), emptyCells.end(), rng);

        long nodesExpanded = 0;

        // Start timer
        auto start = chrono::high_resolution_clock::now();

        if (solve_backtrack_helper(board, emptyCells, 0, nodesExpanded))
        {
            // End timer
            auto end = chrono::high_resolution_clock::now();

            // Calculate duration
            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

            return {duration.count(), nodesExpanded};
        }

        return {};
    }

    bool solve_backtrack_helper(Board &board, Board &emptyCells, int index, long &nodesExpanded)
    {
        // If no empty cells, then the board is solved
        if (index == emptyCells.size())
            return true;

        // Otherwise, choose a random empty cell
        vector<int> cell = emptyCells[index];

        // Domain
        vector<int> domain = {1, 2, 3, 4, 5, 6, 7, 8, 9};

        // Randomize domain
        random_shuffle(domain.begin(), domain.end());

        for (int i = 0; i < domain.size(); i++)
        {
            if (isValid(board, cell[0], cell[1], domain[i]))
            {
                // Choose
                board[cell[0]][cell[1]] = domain[i];

                ++nodesExpanded;

                // Explore
                if (solve_backtrack_helper(board, emptyCells, index + 1, nodesExpanded))
                    return true;

                // Unchoose
                board[cell[0]][cell[1]] = 0;
            }
        }

        return false;
    }

    // TODO: Can probably make this function generic and accept a function pointer or lambda

    // Backtracking + Forward checking
    vector<long> solve_btfc(Board &board)
    {
        auto emptyCells = getEmptyCells(board);
        mt19937 rng(random_device{}());
        shuffle(emptyCells.begin(), emptyCells.end(), rng);

        long nodesExpanded = 0;
        auto start = chrono::high_resolution_clock::now();

        if (solve_btfc_helper(board, emptyCells, 0, nodesExpanded))
        {
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
            return {duration.count(), nodesExpanded};
        }

        return {}; // Return an empty vector or appropriate error indication if no solution was found
    }

    bool solve_btfc_helper(Board &board, Board &emptyCells, int index, long &nodesExpanded)
    {
        // If no empty cells, then the board is solved
        if (index == emptyCells.size())
            return true;

        // Otherwise, choose a random empty cell
        vector<int> cell = emptyCells[index];

        // Domain
        vector<int> domain = getDomain(board, cell[0], cell[1]);

        // Randomize domain
        random_shuffle(domain.begin(), domain.end());

        for (int value : domain)
        {
            if (isValid(board, cell[0], cell[1], value))
            {
                // Choose
                board[cell[0]][cell[1]] = value;

                ++nodesExpanded;

                // Explore
                if (solve_btfc_helper(board, emptyCells, index + 1, nodesExpanded))
                    return true;

                // Unchoose
                board[cell[0]][cell[1]] = 0;
            }
        }

        return false;
    }

    bool isValid(const Board &board, int row, int col, int c)
    {
        for (int i = 0; i < 9; i++)
        {
            if (board[i][col] == c)
                return false;
            if (board[row][i] == c)
                return false;
            if (board[3 * (row / 3) + i / 3][3 * (col / 3) + i % 3] == c)
                return false;
        }
        return true;
    }

    Board getEmptyCells(Board &board)
    {
        Board emptyCells;
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if (board[i][j] == 0)
                    emptyCells.push_back({i, j});
            }
        }
        return emptyCells;
    }

    /*
        Calculate and return the domain (possible values) for the cell at [row][col]
        This typically involves checking which values are not already in the same row, column, and 3x3 grid
    */
    vector<int> getDomain(const Board &board, int row, int col)
    {
        vector<int> domain;
        for (int i = 1; i <= 9; ++i)
        {
            if (isValid(board, row, col, i))
                domain.push_back(i);
        }
        return domain;
    }
};

Board processInput(string input)
{
    Board board;
    for (int i = 0; i < 9; i++)
    {
        vector<int> row;
        for (int j = 0; j < 9; j++)
            row.push_back(input[i * 9 + j] - '0');
        board.push_back(row);
    }
    return board;
}

// Solve the sudoku puzzle `iter` number of times and print statistics
void solveSudoku(Board &board, int approach, int iter)
{
    SudokuSolver solver(board, approach);
    vector<vector<long>> stats;
    for (int i = 0; i < iter; i++)
    {
        stats.emplace_back(solver.solve());
    }

    // Print stats
    for (int i = 0; i < iter; i++)
        cout << stats[i][0] << " ";

    std::cout << endl;

    for (int i = 0; i < iter; i++)
        cout << stats[i][1] << " ";

    // Calculate average time and nodes expanded
    float avgTime = 0;
    float avgNodesExpanded = 0;
    for (int i = 0; i < iter; i++)
    {
        avgTime += stats[i][0];
        avgNodesExpanded += stats[i][1];
    }

    avgTime /= iter;
    avgNodesExpanded /= iter;

    // Calculate standard deviation of time and nodes expanded
    float stdDevTime = 0;
    float stdDevNodesExpanded = 0;
    for (int i = 0; i < iter; i++)
    {
        stdDevTime += pow(stats[i][0] - avgTime, 2);
        stdDevNodesExpanded += pow(stats[i][1] - avgNodesExpanded, 2);
    }

    cout << "Average time taken: " << avgTime << " +- " << sqrt(stdDevTime / iter) << " milliseconds" << endl;
    cout << "Average nodes expanded: " << avgNodesExpanded << " +- " << sqrt(stdDevNodesExpanded / iter) << endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Usage: ./sudoku_solver <input_file> <approach>" << endl;
        return 1;
    }

    // Read input from file
    ifstream file(argv[1]);
    string input = "";
    for (string line; getline(file, line);)
        input += line;

    Board board = processInput(input);

    cout << "Initial board: \n";
    printBoard(board);

    int approach = *argv[2] - '0';

    solveSudoku(board, approach, 10);

    return 0;
}
