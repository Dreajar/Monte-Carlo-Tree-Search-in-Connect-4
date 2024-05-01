#pragma once
#include <connect4.h>
#include <random>
#include <cmath>
#include <chrono>
#include <unordered_map>

// pointers to a position might not be unique; so key must be Position -> cannot use nodeType*
// the class Position is just 7 integers; pass by value is better
typedef std::unordered_map<Position, std::tuple<double, double, std::unordered_map<Position, int>>> nodeType;

nodeType get_nodes(Position initial_pos, double time_limit);
Position ucb2_agent(double time_limit, Position pos);
float randomly_play(const Position& pos);
std::vector<Position> get_leaf(nodeType& nodes, Position root);
float get_score(int N, int ni, float r, int player, float c = 2.0f);