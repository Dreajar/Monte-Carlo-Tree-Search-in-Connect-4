#include <Mcts.h>
nodeType get_nodes(Position initial_pos, double time_limit)
{
    nodeType nodes;
    nodes[initial_pos] = std::make_tuple(0.0, 0.0, std::unordered_map<Position, int>{{initial_pos, 0}});
    auto start_time = std::chrono::high_resolution_clock::now();

    while (std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start_time).count() < time_limit)
    {
        // root to leaf traversal - return entire traversal path
        std::vector<Position> leaf_path = get_leaf(nodes, initial_pos);

        // the leaf is the last entry in the path
        Position leaf = leaf_path.back();

        int ni = std::get<2>(nodes[leaf])[leaf];

        if (ni > 0 && !leaf.terminal)
        {
            std::vector<int> legal_moves = leaf.legal_moves();
            for (int loc : legal_moves)
            {
                Position new_pos = leaf.move(loc);

                // so we don't override existing data
                if (nodes.find(new_pos) == nodes.end())
                {
                    nodes[new_pos] = std::make_tuple(0.0, 0.0, std::unordered_map<Position, int>{{leaf, 0}});
                }
            }

            // choose random child node to explore
            int loc = legal_moves[std::rand() % legal_moves.size()];

            Position child_pos = leaf.move(loc);
            double reward = 0;
            int num_runs = 10;

            for (int i = 0; i < num_runs; i++)
            {
                reward += randomly_play(child_pos);
            }

            double w = std::get<0>(nodes[child_pos]);
            int n = std::get<1>(nodes[child_pos]);
            std::unordered_map<Position, int> parent_n_dict = std::get<2>(nodes[child_pos]);

            if (parent_n_dict.find(leaf) == parent_n_dict.end())
            {
                parent_n_dict[leaf] = 0;
            }
            parent_n_dict[leaf] += 1;
            nodes[child_pos] = std::make_tuple(w + reward, n + num_runs, parent_n_dict);
        }
        else
        {
            // otherwise randomly play that leaf
            double reward = 0;
            int num_runs = 10;
            for (int i = 0; i < num_runs; i++)
            {
                reward += randomly_play(leaf);
            }

            // update all positions in path to reflect new information
            Position parent = initial_pos;
            for (Position position : leaf_path)
            {
                double w = std::get<0>(nodes[position]);
                double n = std::get<1>(nodes[position]);
                std::unordered_map<Position, int> parent_n_dict = std::get<2>(nodes[position]);
                parent_n_dict[parent] += num_runs;
                nodes[position] = std::make_tuple(w + reward, n + num_runs, parent_n_dict);
                parent = position;
            }
        }
    }
    return nodes;
}

std::vector<Position> get_leaf(nodeType& nodes, Position root)
{
    Position current_node = root;
    std::vector<Position> path;

    while (true)
    {
        float w = std::get<0>(nodes[current_node]); // std::get<n>(my_tuple)
        int ni = std::get<1>(nodes[current_node]);

        path.push_back(current_node);
        if (ni == 0)
        {
            return path;
        }

        std::vector<int> legal_moves = current_node.legal_moves();
        int next_player = current_node.turn;

        float best_score = (next_player == 1) ? INFINITY : -INFINITY;
        Position next_best_node(100, 0, 0, 0); // temporary object; turn = 100

        for (const int loc : legal_moves)
        {
            Position result_position = current_node.move(loc);
            if (nodes.find(result_position) == nodes.end())
            {
                return path;
            }

            float temp_w = std::get<0>(nodes[result_position]);
            int temp_ni = std::get<1>(nodes[result_position]);
            std::unordered_map<Position, int>& temp_parent_n_count = std::get<2>(nodes[result_position]);

            if (temp_parent_n_count.find(current_node) == temp_parent_n_count.end())
            {
                temp_parent_n_count[current_node] = 0;
            }

            if (temp_parent_n_count[current_node] == 0)
            {
                path.push_back(result_position);
                return path;
            }

            // N is the number of times parent has been visited

            float score = get_score(std::get<1>(nodes[current_node]), temp_parent_n_count[current_node], temp_w / temp_ni, next_player);
            if ((score < best_score && next_player == 1) || (score > best_score && next_player == 0))
            {
                best_score = score;
                next_best_node = result_position;
            }
        }

        current_node = next_best_node;
        if (current_node.turn == 100)
        {
            return path;
        }
    }
}

// MISSING: changed function header
Position ucb2_agent(double time_limit, Position pos)
{
    // MISSING: CHECK RETURN TYPE; we're returing next_best_move
    nodeType nodes = get_nodes(pos, time_limit);

    int player = pos.turn;
    double best_score = (player == 1) ? INFINITY : -INFINITY;
    int next_best_move;
    for (int loc : pos.legal_moves())
    {
        Position next_pos = pos.move(loc);
        double w, n, score;
        std::unordered_map<Position, int> parent_n_dict;
        if (nodes.find(next_pos) == nodes.end())
        {
            score = 0.0;
        }
        else
        {
            w = std::get<0>(nodes[next_pos]);
            n = std::get<1>(nodes[next_pos]);
            score = (n == 0) ? 0.0 : w / n;
        }

        // update best score & next move
        if ((player == 1 && score < best_score) || (player == 0 && score > best_score))
        {
            best_score = score;
            next_best_move = loc;
        }
    }
    return next_best_move;
};

float randomly_play(const Position& pos)
{
    Position cur_pos = pos;
    while (!cur_pos.terminal)
    {
        std::vector<int> moves = cur_pos.legal_moves();
        int loc = moves[std::rand() % moves.size()];
        cur_pos = cur_pos.move(loc);
    }
    return static_cast<float>(cur_pos.result);
}

float get_score(int N, int ni, float r, int player, float c = 2.0f)
{
    return (player == 0) ? r + std::sqrt(c * std::log(N) / ni)
        : r - std::sqrt(c * std::log(N) / ni);
}