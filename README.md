
# Connect 4 with Monte Carlo Tree Search

This repository implements a Connect 4 AI player using the Monte Carlo Tree Search (MCTS) algorithm. MCTS is a powerful technique for games where building a perfect decision tree is impractical.

**Connect4.cpp** includes the implementations of the board using bitwise operations.\
**Mcts.cpp** contains the modified MCTS algorithm implementation.\
**play.cpp** is a template to show how to run the algorithm on a Connect 4 website.\
**FindGrid.cpp** is required for the algorithm to know where to look for connect 4 pieces on the website.

## Why Monte Carlo Tree Search?
A popular algorithm in Minimax. Minimax analyzes *every* move at *every* position, which leads to the *optimal* move every turn. However, according to *OEIS*, there are 4531985219092 possible Connect 4 board states. This means running Minimax will take a long time.

On the other hand, the focus of MCTS is on the analysis of the most promising moves, expanding the search tree based on *random sampling* of the search space. The application of MCTS in games is based on many playouts, also called roll-outs. In each playout, the game is played out to the very end by selecting moves at random. The final game result of each playout is then used to weight the nodes in the game tree so that *better nodes are more likely to be chosen* in future playouts.

## About the Project
This project is a Connect 4 playing agent that utilizes MCTS to make strategic decisions. Here's a breakdown of the key aspects:

1. Game Representation: The code represents the Connect 4 board as 2 bitstrings *position* and *mask*. *position* tracks the placement of both player's discs, while *mask* tracks the placement of player 1's discs. The placement of player 2's discs can be found by XORing *position* and *mask*. Accordingly, we calculate whether the game is over via bitshift operators.

2. MCTS Implementation: The core algorithm follows the four main steps of MCTS:
- Selection: Traverses the tree from the root, selecting the child node with the highest Upper Confidence Bound (UCB) value. UCB balances exploration (trying new moves) and exploitation (favoring winning moves).
- Expansion: If a non-visited child node exists at the selected node, it's added to the tree for exploration.
- Simulation: Plays out a random game simulation from the selected node until a terminal state (win, loss, or tie) is reached.
- Backpropagation: Updates the statistics (visit count and win score) of the nodes traversed during selection, based on the simulation outcome.

3. Playing the Game: The AI agent uses the MCTS to determine the best move for its turn. It simulates games through the tree, favoring moves that lead to more wins in simulations.

## Further Optimisations
This project serves as a foundation for building upon. Here are some potential improvements:

1. **Use of caching (transposition tables & Python LRU cache)**. We can add the position and mask bitmap together to form a unique key. This key can be stored in a dictionary with the corresponding upper or lower bound. Now, before we start iterating over all our children, we can already initialize our current value, by getting the corresponding bound from the cache first. Another very simple optimization is adding lru_cache decorators to every helper-function.

2. **Earlier determination of winning state**. Initially, an ending state (leaf node) was reached when either four tokens were connected or 42 moves were made. We can already determine one move earlier if the game will end or not (namely if there are three tokens connected and the user can make a move that introduces the fourth token in the connection). This is a very efficient check, and makes our trees more shallow.

3. **Go for any winning path, not the winning path with shortest number of moves**. Instead of looking for a maximum or minimum value, we can just follow the path of the first strictly positive or negative (respectively) value. Those paths are not the optimal one (we will not win with a minimal number of moves or lose with a maximum number of moves), but we were more than happy with a regular win (it did not have to be the optimal one).

4. **Other optimizations**. One can also try multithreading, sorting the order of leaf traversals, pruning subtrees early that lead to a game loss etc

5. **Storing the first 20 optimal moves**. Note that Connect 4 is solved, so we can have our algorithm play against a minimax algorithm to collect the first 20 optimal moves. However, in some sense this defeats the point of using MCTS.


## Closing Remarks
Note that this is a basic outline. Feel free to add details specific to your implementation.
