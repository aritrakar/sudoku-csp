#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <utility>
#include <fstream>
#include <dirent.h>
// #include <functional>

using namespace std;

// Having to randomize the domain as well prevents me from using a bitmap efficiently
// typedef vector<vector<pair<int, unsigned short>>> Board;
typedef vector<vector<int>> Board;

// Macro for shuffling a container with mt19937 and random_device
#define SHUFFLE_CONTAINER_WITH_MT(container)                     \
    do                                                           \
    {                                                            \
        std::random_device rd;                                   \
        std::mt19937 g(rd());                                    \
        std::shuffle((container).begin(), (container).end(), g); \
    } while (0)

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
        vector<long> value;

        // if (approach == 1)
        // {
        //     return solve_generic(boardCopy, [&](Board &emptyCells, long &nodesExpanded)
        //                          { return solve_backtrack_helper(board, emptyCells, 0, nodesExpanded); });
        // }

        if (approach == 1)
        {
            value = solve_backtrack(boardCopy);
        }
        else if (approach == 2)
        {
            value = solve_btfc(boardCopy);
        }
        else if (approach == 3)
        {
            value = solve_btfch(boardCopy);
        }

        if (value.size() > 0)
            return value;

        throw "No solution found";
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
        vector<pair<int, int>> emptyCells = getEmptyCells(board);

        // Randomize empty cells
        SHUFFLE_CONTAINER_WITH_MT(emptyCells);

        long nodesExpanded = 0;
        auto start = chrono::high_resolution_clock::now();

        if (solve_backtrack_helper(board, emptyCells, 0, nodesExpanded))
        {
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
            return {duration.count(), nodesExpanded};
        }

        return {};
    }

    bool solve_backtrack_helper(Board &board, vector<pair<int, int>> &emptyCells, int index, long &nodesExpanded)
    {
        // If no empty cells, then the board is solved
        if (index == emptyCells.size())
            return true;

        // Otherwise, choose a random empty cell
        pair<int, int> cell = emptyCells[index];
        int row = cell.first;
        int col = cell.second;

        // Domain
        vector<int> domain = {1, 2, 3, 4, 5, 6, 7, 8, 9};

        // Randomize domain
        // Note: Using SHUFFLE_CONTAINER_WITH_MT(domain) here makes
        // the program MUCH slower, up to 20x slower.
        random_shuffle(domain.begin(), domain.end());

        for (int i = 0; i < domain.size(); i++)
        {
            if (isValid(board, row, col, domain[i]))
            {
                // Choose
                board[row][col] = domain[i];

                ++nodesExpanded;

                // Explore
                if (solve_backtrack_helper(board, emptyCells, index + 1, nodesExpanded))
                    return true;

                // Unchoose
                board[row][col] = 0;
            }
        }

        return false;
    }

    // TODO: Can probably make this function generic and accept a function pointer or lambda

    // Backtracking + Forward checking
    vector<long> solve_btfc(Board &board)
    {
        vector<pair<int, int>> emptyCells = getEmptyCells(board);

        // Randomize empty cells
        SHUFFLE_CONTAINER_WITH_MT(emptyCells);

        long nodesExpanded = 0;
        auto start = chrono::high_resolution_clock::now();

        vector<vector<vector<int>>> remainingValues(9, vector<vector<int>>(9, vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9}));

        // Initialize remaining values for each cell
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if (board[i][j] != 0)
                {
                    remainingValues[i][j].clear();
                }
                else
                {
                    for (int k = 0; k < 9; k++)
                    {
                        remainingValues[i][j].erase(remove(remainingValues[i][j].begin(), remainingValues[i][j].end(), board[i][k]), remainingValues[i][j].end());
                        remainingValues[i][j].erase(remove(remainingValues[i][j].begin(), remainingValues[i][j].end(), board[k][j]), remainingValues[i][j].end());
                        remainingValues[i][j].erase(remove(remainingValues[i][j].begin(), remainingValues[i][j].end(), board[3 * (i / 3) + k / 3][3 * (j / 3) + k % 3]), remainingValues[i][j].end());
                    }
                }
            }
        }

        if (solve_btfc_helper(board, emptyCells, 0, nodesExpanded, remainingValues))
        {
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
            return {duration.count(), nodesExpanded};
        }

        return {}; // Return an empty vector or appropriate error indication if no solution was found
    }

    bool solve_btfc_helper(Board &board, vector<pair<int, int>> &emptyCells, int index,
                           long &nodesExpanded, vector<vector<vector<int>>> &legalValues)
    {
        // If no empty cells, then the board is solved
        if (index == emptyCells.size())
            return true;

        // Otherwise, choose a random empty cell
        pair<int, int> cell = emptyCells[index];

        int row = cell.first;
        int col = cell.second;

        // Domain
        vector<int> domain = legalValues[row][col];

        // Randomize domain set
        random_shuffle(domain.begin(), domain.end());

        int startRow = 3 * (row / 3);
        int startCol = 3 * (col / 3);

        for (int value : domain)
        {
            if (isValid(board, row, col, value))
            {
                // Choose
                board[row][col] = value;

                bool isConsistent = true;
                // Board boardCopy = board;

                // Copy the legal values
                vector<vector<vector<int>>> legalValuesCopy = legalValues;

                // Forward checking: Update remaining values for cells in the same row and column and grid
                for (int i = 0; i < 9; i++)
                {
                    // Forward checking for rows and columns
                    // We check if a cell is empty because then its domain should be non-empty,
                    // which means we need to update it.
                    if (board[row][i] == 0)
                    {
                        auto &remainingValues = legalValuesCopy[row][i];
                        remainingValues.erase(remove(remainingValues.begin(), remainingValues.end(), value), remainingValues.end());
                        if (remainingValues.empty())
                        {
                            isConsistent = false;
                            break;
                        }
                    }
                    if (board[i][col] == 0)
                    {
                        auto &remainingValues = legalValuesCopy[i][col];
                        remainingValues.erase(remove(remainingValues.begin(), remainingValues.end(), value), remainingValues.end());
                        if (remainingValues.empty())
                        {
                            isConsistent = false;
                            break;
                        }
                    }
                }

                // Correct iteration over the 3x3 box
                if (isConsistent)
                { // Only proceed if still consistent
                    for (int boxRow = 0; boxRow < 3; boxRow++)
                    {
                        for (int boxCol = 0; boxCol < 3; boxCol++)
                        {
                            int actualRow = startRow + boxRow;
                            int actualCol = startCol + boxCol;
                            if (board[actualRow][actualCol] == 0)
                            {
                                auto &remainingValues = legalValuesCopy[actualRow][actualCol];
                                remainingValues.erase(remove(remainingValues.begin(), remainingValues.end(), value), remainingValues.end());
                                if (remainingValues.empty())
                                {
                                    isConsistent = false;
                                    break; // Exit the inner loop
                                }
                            }
                        }
                        if (!isConsistent)
                            break; // Exit the outer loop if inconsistent
                    }
                }

                ++nodesExpanded;

                // Explore
                if (isConsistent && solve_btfc_helper(board, emptyCells, index + 1, nodesExpanded, legalValuesCopy))
                    return true;

                // Unchoose
                board[row][col] = 0;
            }
        }

        return false;
    }

    // Backtracking + Forward checking + Heuristics
    vector<long> solve_btfch(Board &board)
    {
        // vector<pair<int, int>> emptyCells = getEmptyCells(board);
        // SHUFFLE_CONTAINER_WITH_MT(emptyCells);

        long nodesExpanded = 0;
        auto start = chrono::high_resolution_clock::now();

        vector<vector<vector<int>>> remainingValues(9, vector<vector<int>>(9, vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9}));

        // Initialize remaining values for each cell
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if (board[i][j] != 0)
                {
                    remainingValues[i][j].clear();
                }
                else
                {
                    for (int k = 0; k < 9; k++)
                    {
                        remainingValues[i][j].erase(remove(remainingValues[i][j].begin(), remainingValues[i][j].end(), board[i][k]), remainingValues[i][j].end());
                        remainingValues[i][j].erase(remove(remainingValues[i][j].begin(), remainingValues[i][j].end(), board[k][j]), remainingValues[i][j].end());
                        remainingValues[i][j].erase(remove(remainingValues[i][j].begin(), remainingValues[i][j].end(), board[3 * (i / 3) + k / 3][3 * (j / 3) + k % 3]), remainingValues[i][j].end());
                    }
                }
            }
        }

        if (solve_btfch_helper(board, nodesExpanded, remainingValues))
        {
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
            return {duration.count(), nodesExpanded};
        }

        cout << "No solution found" << endl;
        printBoard(board);

        return {}; // Return an empty vector or appropriate error indication if no solution was found
    }

    bool solve_btfch_helper(Board &board, // vector<pair<int, int>> &emptyCells, // int index,
                            long &nodesExpanded, vector<vector<vector<int>>> &legalValues)
    {
        auto emptyCells = getEmptyCells(board);
        // SHUFFLE_CONTAINER_WITH_MT(emptyCells);
        random_shuffle(emptyCells.begin(), emptyCells.end()); // This is faster?!

        // If no empty cells, then the board is solved
        if (emptyCells.empty())
            // if (index == emptyCells.size())
            return true;

        // Find the most constrained cell
        // vector<vector<int>> mostConstrainedCells; // row, col, empty cell index
        vector<pair<int, int>> mostConstrainedCells;
        int minDomainSize = 10;
        for (auto cell : emptyCells)
        {
            int row = cell.first;
            int col = cell.second;
            int domainSize = legalValues[row][col].size();
            if (domainSize < minDomainSize)
            {
                minDomainSize = domainSize;
                mostConstrainedCells.emplace_back(cell);
            }
        }

        // First, find the minimum domain size
        // for (auto cell : emptyCells)
        // {
        //     minDomainSize = min(minDomainSize, (int)legalValues[cell.first][cell.second].size());
        // }

        // Then, find all cells with the minimum domain size
        // for (int i = 0; i < emptyCells.size(); i++)
        // {
        //     if (legalValues[emptyCells[i].first][emptyCells[i].second].size() == minDomainSize)
        //         mostConstrainedCells.emplace_back(vector<int>{emptyCells[i].first, emptyCells[i].second, i});
        // }

        pair<int, int> chosenCell;
        // int emptyCellIndex = 0;

        // Check for tie
        if (mostConstrainedCells.size() == 1)
        {
            // No tie because there's only one cell
            chosenCell = mostConstrainedCells[0];
            // chosenCell = make_pair(mostConstrainedCells[0][0], mostConstrainedCells[0][1]);
            // emptyCellIndex = mostConstrainedCells[0][2];
        }
        else
        {
            // Find the most constraining cell
            int maxConstraints = -1;
            for (auto cell : mostConstrainedCells)
            {
                int row = cell.first;
                int col = cell.second;
                int constraints = 0;
                for (int i = 0; i < 9; i++)
                {
                    if (board[row][i] == 0)
                        constraints++;
                    if (board[i][col] == 0)
                        constraints++;
                    if (board[3 * (row / 3) + i / 3][3 * (col / 3) + i % 3] == 0)
                        constraints++;
                }
                if (constraints > maxConstraints)
                {
                    maxConstraints = constraints;
                    chosenCell = cell;
                }
            }
        }

        int row = chosenCell.first;
        int col = chosenCell.second;

        // Now, to find the least constraining value
        vector<int> domain = legalValues[row][col];
        vector<pair<int, int>> leastConstrainingValues;

        for (int value : domain)
        {
            // Count the number of constraints (occurrences) for each value
            int constraints = 0;
            for (int i = 0; i < 9; i++)
            {
                if (board[row][i] == 0)
                {
                    auto &remainingValues = legalValues[row][i];
                    if (find(remainingValues.begin(), remainingValues.end(), value) != remainingValues.end())
                        constraints++;
                }
                if (board[i][col] == 0)
                {
                    auto &remainingValues = legalValues[i][col];
                    if (find(remainingValues.begin(), remainingValues.end(), value) != remainingValues.end())
                        constraints++;
                }
                if (board[3 * (row / 3) + i / 3][3 * (col / 3) + i % 3] == 0)
                {
                    auto &remainingValues = legalValues[3 * (row / 3) + i / 3][3 * (col / 3) + i % 3];
                    if (find(remainingValues.begin(), remainingValues.end(), value) != remainingValues.end())
                        constraints++;
                }
            }
            leastConstrainingValues.emplace_back(make_pair(value, constraints));
        }

        // Sort the least constraining values (by the second element of the pair,
        // which is the number of constraints)
        sort(leastConstrainingValues.begin(), leastConstrainingValues.end(),
             [](const pair<int, int> &a, const pair<int, int> &b)
             {
                 return a.second < b.second;
             });

        for (auto value : leastConstrainingValues)
        {
            if (isValid(board, row, col, value.first))
            {
                // Choose
                board[row][col] = value.first;

                // Update empty cells
                // emptyCells.erase(remove(emptyCells.begin(), emptyCells.end(), chosenCell), emptyCells.end());

                bool isConsistent = true;

                // Copy the legal values
                vector<vector<vector<int>>> legalValuesCopy = legalValues;

                // Forward checking: Update remaining values for cells in the same row and column and grid
                for (int i = 0; i < 9; i++)
                {
                    // Forward checking for rows and columns
                    // We check if a cell is empty because then its domain should be non-empty,
                    // which means we need to update it.
                    if (board[row][i] == 0)
                    {
                        auto &remainingValues = legalValuesCopy[row][i];
                        remainingValues.erase(remove(remainingValues.begin(), remainingValues.end(), value.first), remainingValues.end());
                        if (remainingValues.empty())
                        {
                            isConsistent = false;
                            break;
                        }
                    }
                    if (board[i][col] == 0)
                    {
                        auto &remainingValues = legalValuesCopy[i][col];
                        remainingValues.erase(remove(remainingValues.begin(), remainingValues.end(), value.first), remainingValues.end());
                        if (remainingValues.empty())
                        {
                            isConsistent = false;
                            break;
                        }
                    }
                }

                // Correct iteration over the 3x3 box
                if (isConsistent)
                { // Only proceed if still consistent
                    for (int boxRow = 0; boxRow < 3; boxRow++)
                    {
                        for (int boxCol = 0; boxCol < 3; boxCol++)
                        {
                            int actualRow = 3 * (row / 3) + boxRow;
                            int actualCol = 3 * (col / 3) + boxCol;
                            if (board[actualRow][actualCol] == 0)
                            {
                                auto &remainingValues = legalValuesCopy[actualRow][actualCol];
                                remainingValues.erase(remove(remainingValues.begin(), remainingValues.end(), value.first), remainingValues.end());
                                if (remainingValues.empty())
                                {
                                    isConsistent = false;
                                    break; // Exit the inner loop
                                }
                            }
                        }
                        if (!isConsistent)
                            break; // Exit the outer loop if inconsistent
                    }
                }

                ++nodesExpanded;

                // Explore
                if (isConsistent && solve_btfch_helper(board, nodesExpanded, legalValuesCopy))
                    // if (isConsistent && solve_btfch_helper(board, emptyCells, index + 1, nodesExpanded, legalValuesCopy))
                    return true;

                // Unchoose
                board[row][col] = 0;
                // emptyCells.push_back(chosenCell);
            }
        }

        return false;
    }

    bool isValid(const Board &board, int row, int col, int c)
    {
        int startRow = 3 * (row / 3);
        int startCol = 3 * (col / 3);
        for (int i = 0; i < 9; i++)
        {
            if (board[i][col] == c)
                return false;
            if (board[row][i] == c)
                return false;
            if (board[startRow + i / 3][startCol + i % 3] == c)
                // if (board[3 * (row / 3) + i / 3][3 * (col / 3) + i % 3] == c)
                return false;
        }
        return true;
    }

    vector<pair<int, int>> getEmptyCells(Board &board)
    {
        vector<pair<int, int>> emptyCells;
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if (board[i][j] == 0)
                    emptyCells.push_back({i, j});
            }
        }
        // SHUFFLE_CONTAINER_WITH_MT(emptyCells);
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
    int value = 0;
    for (int i = 0; i < 9; i++)
    {
        vector<int> row;
        for (int j = 0; j < 9; j++)
        {
            value = input[i * 9 + j] - '0';
            row.push_back(value);
        }
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
    // for (int i = 0; i < iter; i++)
    //     cout << stats[i][0] << " ";

    // std::cout << endl;

    // for (int i = 0; i < iter; i++)
    //     cout << stats[i][1] << " ";

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

vector<string> readPuzzleFiles(const string &directory)
{
    vector<string> files;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(directory.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            string file = ent->d_name;
            // Skip . and .. directories
            if (file != "." && file != "..")
            {
                // cout << "File: " << file << endl;
                files.push_back(directory + "/" + file);
            }
        }
        closedir(dir);
    }
    else
    {
        cerr << "Could not open directory: " << directory << endl;
    }

    // Sort the filenames to ensure a specific order
    sort(files.begin(), files.end());
    return files;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Usage: ./sudoku_solver <mode> [input_file/approach]" << endl;
        return 1;
    }

    const int iterations = 10;
    int mode = stoi(argv[1]);
    if (mode == 1)
    {
        if (argc != 4)
        {
            cout << "Usage for mode 1: ./sudoku_solver 1 <input_file> <approach>" << endl;
            return 1;
        }

        ifstream file(argv[2]);
        string input = "";
        for (string line; getline(file, line);)
            input += line;

        Board board = processInput(input);

        cout << "Initial board: \n";
        printBoard(board);

        int approach = stoi(argv[3]);
        solveSudoku(board, approach, iterations);
    }
    else if (mode == 2)
    {
        const string approaches[3] = {"B", "BTFC", "BTFCH"};
        const string directory = "etc"; // Assuming puzzles are in ./etc directory
        vector<string> puzzleFiles = readPuzzleFiles(directory);

        for (int approach = 3; approach > 0; --approach)
        {
            cout << "***Method***: " << approaches[approach - 1] << endl;
            for (const string &filePath : puzzleFiles)
            {
                cout << "Puzzle: " << filePath << endl;

                ifstream file(filePath);
                if (!file.is_open())
                {
                    cerr << "Failed to open " << filePath << endl;
                    continue;
                }

                string input = "";
                for (string line; getline(file, line);)
                    input += line;

                Board board = processInput(input);
                solveSudoku(board, approach, iterations);

                cout << "----------------------------------------" << endl;
            }
            cout << "--------------------------------------------------------------------------------\n\n"
                 << endl;
        }
    }
    else
    {
        cout << "Invalid mode specified. Use 1 for single puzzle solving, 2 for batch solving in etc directory." << endl;
        return 1;
    }

    return 0;
}
