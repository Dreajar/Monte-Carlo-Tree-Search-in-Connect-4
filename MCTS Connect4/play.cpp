#include <iostream>
#include <algorithm>
#include <thread>
#include <string>
#include <cstdlib>
#include <ctime>
#include <Mcts.h>
#include <Mouse.h>
#include <windows.h> // for pixel = pyautogui.pixel(mid_x, mid_y)
#include <Robot.h>
#include <FindGrid.h>
ROBOT_NS_USE_ALL;

std::vector<std::vector<std::tuple<int, int>>> get_grid_coords(std::tuple<int, int, int, int> grid)
{
    //grid = (left, top, width, height)
    // get coordinates of the centers of 42 squares in the game

    int left = std::get<0>(grid);
    int top = std::get<1>(grid);
    int width = std::get<2>(grid);
    int height = std::get<3>(grid);

    std::vector<int> xs, ys;
    for (int i = 0; i < 7; i++) { xs.push_back(int(left + width / 14 + i * width / 7)); }
    for (int i = 0; i < 6; i++) { ys.push_back(int(top + height / 12 + i * height / 6)); }

    std::vector<std::vector<std::tuple<int, int>>> coords; // similar to a matrix storing the coordinate of each entry
    for (int y : ys)
    {
        std::vector<std::tuple<int, int>> row;
        for (int x : xs)
        {
            row.push_back(std::make_tuple(x, y)); 
        }
        coords.push_back(row);
    }
    return coords;       
}

bool turn_first(std::tuple<int, int, int, int> grid, std::vector<std::vector<std::tuple<int, int>>>& coords)
{
    // figure out turn and colors; MISSING: TRANSLATE INTO C++
    Mouse::SetPos(std::get<0>(coords[3][3]), std::get<1>(coords[3][3]));

    // sleep for 0.1 seconds
    std::this_thread::sleep_for(std::chrono::milliseconds(100));


    int mid_x = (std::get<0>(coords[3][3]) + std::get<0>(coords[4][3])) / 2;
    int mid_y = (std::get<1>(coords[3][3]) + std::get<1>(coords[4][3])) / 2;

    HDC hDC = GetDC(0);
    GetPixel(hDC, mid_x, mid_y);
    COLORREF pixel = GetPixel(hDC, mid_x, mid_y);

    // why ? Column is highlighted in dark gray if its your turn
    // so if its light gray, computer goes second

    bool light_gray = pix_equal(pixel, { 206, 212, 218 });
    return !light_gray;
}

bool pix_equal(const COLORREF& pixel, const std::vector<int>& goal) { return pixel == RGB(goal[0], goal[1], goal[2]); }

void board_move(std::vector<std::vector<int>>& board, int loc, int turn)
{
    for (int i = board.size() - 1; i >= 0; i--)
    {
        if (board[i][loc] == 0)
        {
            board[i][loc] = turn == 0 ? 1 : 2;
            break;
        }
    }
}

std::pair<std::vector<std::vector<int>>, Position> run(std::tuple<int, int, int, int> grid, Mouse& mouse)
{
    // find 6x7 grid and coords of all squares
    std::vector<std::vector<std::tuple<int, int>>> coords = get_grid_coords(grid);

    // initalize game
    Position pos = Connect4().get_initial_position();
    
    std:: vector<std::vector<int>> board(6, std::vector<int>(7, 0));
    // vector<int>(n times, v value)

    // figure out turns
    bool first_player = turn_first(grid, coords);

    // define strategy (seconds per move as argument)
    Position strategy = ucb2_agent(7, pos); // MISSING: CHECK ARGUMENTS

    while (!pos.terminal)
    {
        // computer's turn
        if ((first_player && pos.turn == 0) || (!first_player && pos.turn == 1))
        {
            // computer computes best move
            int move = strategy(pos);

            // move and click mouse to make move on physical game
            Mouse::SetPos(std::get<0>(coords[0][move]), std::get<1>(coords[0][move]));
            mouse.Click(ButtonLeft);

            // Do we have to pass the mouse object by refernce?

            board_move(board, move, pos.turn);
            pos = pos.move(move);
            // player's turn
        }
        else {
            bool found_move = false;
            for (int i = 0; i < board.size(); i++) {
                for (int j = 0; j < board[0].size(); j++) {
                    if (found_move) continue;
                    
                    HDC hDC = GetDC(0);
                    COLORREF pix = GetPixel(hDC, std::get<0>(coords[i][j]), std::get<1>(coords[i][j]));

                    if (!pix_equal(pix, { 255, 255, 255 }) && board[i][j] == 0) {
                        board_move(board, j, pos.turn);
                        pos = pos.move(j);
                        found_move = true;
                    }
                }
            }
        }
    }

    return std::make_pair(board, pos);
}

int main() {
    std::tuple<int, int, int, int> grid(make_tuple(-100, -100, 0, 0));
    // left, top, width, height

    // must create mouse object before we can do stuff with it
    Mouse mouse;

    // keep running until you find it
    while (std::get<0>(grid) == -100 && std::get<1>(grid) == -100)
    {
        grid = locateGrid();
    }
    std::cout << "Found grid" << std::endl;
    std::pair<std::vector<std::vector<int>>, Position> boardAndPos = run(grid, mouse);
    std::vector<std::vector<int>> board = boardAndPos.first;
    Position pos = boardAndPos.second;
}