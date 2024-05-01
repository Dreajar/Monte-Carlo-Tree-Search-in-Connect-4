#pragma once
#include <bitset>
#include <cstdint>
#include <functional>
#include <vector>

class Connect4
{
public:
    int turn;
    int result;
    bool terminal;

public:
    Connect4() : turn(0), result(), terminal(false) {}

    Position get_initial_position();
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
    Position(int turn, std::uint64_t mask = 0, std::uint64_t position = 0, int num_turns = 0);
    // returns new position
    Position move(int loc);

    // return list of legal moves
    std::vector<int> legal_moves() const;

    void game_over();

    bool connected_four_fast() const;

    void _compute_hash();
    /*
    std::size_t hash() const
    {
        return hash;
    }
    */

    bool operator==(const Position& other) const;

    std::size_t operator()(const Position& p) const;
};
