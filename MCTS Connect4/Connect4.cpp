#include <connect4.h>

Connect4::Connect4() : turn(0), result(), terminal(false) {}

Position Connect4::get_initial_position()
{
    return Position(turn);
}

Position::Position(int turn, std::uint64_t mask = 0, std::uint64_t position = 0, int num_turns = 0)
    : turn(turn), result(), terminal(false), num_turns(num_turns), mask(mask), position(position)
{
    _compute_hash();
}

Position Position::move(int loc)
{
    std::uint64_t new_position = position ^ mask;
    std::uint64_t new_mask = mask | (mask + (1ULL << (loc * 7)));

    Position new_pos(static_cast<int>(!turn), new_mask, new_position, num_turns + 1);
    new_pos.game_over();
    return new_pos;
}

// return list of legal moves
std::vector<int> Position::legal_moves() const
{
    std::vector<int> bit_moves;
    for (int i = 0; i < 7; i++)
    {
        std::uint64_t col_mask = 0b111111ULL << (7 * i);
        if (col_mask != (mask & col_mask))
        {
            bit_moves.push_back(i);
        }
    }
    return bit_moves;
}

void Position::game_over()
{
    // sets result to -1, 0, or 1 if game is over (otherwise self.result is nullptr)
    bool connected_4 = connected_four_fast();

    if (connected_4)
    {
        terminal = true;
        result = int(turn == 1 ? 1 : -1);
    }
    else
    {
        terminal = false;
        result = 2;
    }

    // mask when all spaces are full
    if (mask == 279258638311359ULL)
    {
        terminal = true;
        result = 0;
    }
}

bool Position::connected_four_fast() const
{
    std::uint64_t other_position = position ^ mask; // XOR

    // Horizontal check
    std::uint64_t m = other_position & (other_position >> 7);
    if (m & (m >> 14))
    {
        return true;
    }
    // Diagonal \
    m = other_position & (other_position >> 6);
    if (m & (m >> 12))
    {
        return true;
    }
    // Diagonal /
    m = other_position & (other_position >> 8);
    if (m & (m >> 16))
    {
        return true;
    }
    // Vertical
    m = other_position & (other_position >> 1);
    if (m & (m >> 2))
    {
        return true;
    }
    // Nothing found
    return false;
}

// Check if have to rewrite; mcts.cpp might use Position as a key in maps!
void Position::_compute_hash()
{
    std::uint64_t position_1 = (turn == 0) ? position : position ^ mask;
    // hash = 2 * std::hash<std::pair<std::uint64_t, std::uint64_t>>{}(std::make_pair(position_1, mask)) + turn;
    // MISSING: CHECK IF THIS IS OK. I don't want to create a temporary Position object just to find its hash value
    hash = 2 * (std::hash<uint64_t>{}(position_1) ^ (std::hash<uint64_t>{}(mask) << 1)) + turn;
}

struct PositionHash
{
    std::size_t operator()(const Position& p) const
    {
        return (std::hash<uint64_t>()(p.position) ^ (std::hash<uint64_t>()(p.mask) << 1));
    }
};

bool Position::operator==(const Position& other) const
{
    return turn == other.turn && mask == other.mask && position == other.position;
}
// MISSING: DEFINE HASH OF THE OBJECT POSITION


//////////////////////////////////////
/*
class Connect4
{
public:
    int turn;
    int result;
    bool terminal;

public:
    Connect4() : turn(0), result(), terminal(false) {}

    Position get_initial_position()
    {

    }
};

class Position
{
public:
    int turn;
    int result;
    // MISSING: Note that result = 2 instead of nullptr; see what this breaks

    bool terminal;
    int num_turns;
    std::uint64_t mask;
    std::uint64_t position;
    std::size_t hash;

public:
    Position(int turn, std::uint64_t mask = 0, std::uint64_t position = 0, int num_turns = 0)
        : turn(turn), result(), terminal(false), num_turns(num_turns), mask(mask), position(position)
    {
        _compute_hash();
    }

    // returns new position
    Position move(int loc)
    {
        std::uint64_t new_position = position ^ mask;
        std::uint64_t new_mask = mask | (mask + (1ULL << (loc * 7)));

        Position new_pos(static_cast<int>(!turn), new_mask, new_position, num_turns + 1);
        new_pos.game_over();
        return new_pos;
    }

    // return list of legal moves
    std::vector<int> legal_moves()
    {
        std::vector<int> bit_moves;
        for (int i = 0; i < 7; i++)
        {
            std::uint64_t col_mask = 0b111111ULL << (7 * i);
            if (col_mask != (mask & col_mask))
            {
                bit_moves.push_back(i);
            }
        }
        return bit_moves;
    }

    void game_over()
    {
        // sets result to -1, 0, or 1 if game is over (otherwise self.result is nullptr)
        bool connected_4 = connected_four_fast();

        if (connected_4)
        {
            terminal = true;
            result = int(turn == 1 ? 1 : -1);
        }
        else
        {
            terminal = false;
            result = 2;
        }

        // mask when all spaces are full
        if (mask == 279258638311359ULL)
        {
            terminal = true;
            result = 0;
        }
    }

    bool connected_four_fast()
    {
        std::uint64_t other_position = position ^ mask; // XOR

        // Horizontal check
        std::uint64_t m = other_position & (other_position >> 7);
        if (m & (m >> 14))
        {
            return true;
        }
        // Diagonal \
        m = other_position & (other_position >> 6);
        if (m & (m >> 12))
        {
            return true;
        }
        // Diagonal /
        m = other_position & (other_position >> 8);
        if (m & (m >> 16))
        {
            return true;
        }
        // Vertical
        m = other_position & (other_position >> 1);
        if (m & (m >> 2))
        {
            return true;
        }
        // Nothing found
        return false;
    }

    void _compute_hash()
    {
        std::uint64_t position_1 = (turn == 0) ? position : position ^ mask;
        // hash = 2 * std::hash<std::pair<std::uint64_t, std::uint64_t>>{}(std::make_pair(position_1, mask)) + turn;
        hash = 2 * (std::hash<uint64_t>{}(position_1) ^ (std::hash<uint64_t>{}(mask) << 1)) + turn;
    }


    std::size_t hash() const
    {
        return hash;
    }


    bool operator==(const Position &other) const
    {
        return turn == other.turn && mask == other.mask && position == other.position;
    }
};
*/