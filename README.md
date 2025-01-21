# GomokuAI
This project is an advanced Gomoku AI (also known as Five in a Row or Gobang), developed in C++ with Qt 6.8.0 and utilizing the Minimax search algorithm to provide a challenging and strategic opponent.

<img src="D:\study_study_study_study_study\code\github_project\GomokuAI\images\screenshot.jpg" alt="screenshot" style="zoom: 67%;" />

# Features

- **Minimax Algorithm**: The AI uses the Minimax algorithm to search the game tree, evaluating all possible moves and selecting the strategy with the highest probability of success. The search explores all potential outcomes, aiming to find the best possible move by considering both the AI’s and the opponent’s possible actions.
- **Alpha-Beta Pruning**: The Minimax algorithm is optimized using Alpha-Beta pruning to reduce the number of nodes evaluated, improving performance by eliminating unnecessary branches in the search tree.
- **Heuristic Move Evaluation**: The AI evaluates potential moves using a heuristic approach, based on pattern matching. It looks for specific patterns in the game, such as "open threes" or "potential four-in-a-rows." When the search tree identifies such patterns near the end of the game, they are assigned higher scores, which can guide the AI to take advantage of winning opportunities.
- **Kill Move Detection**: The AI searches for opportunities to create continuous offensive threats that can lead to a win. Rather than just looking for immediate winning moves, it focuses on detecting positions where multiple consecutive offensive moves can secure a victory in the following turns.
- **Customizable Search Depth**: The depth of the Minimax search can be adjusted, allowing a trade-off between **computation time** and **performance**. A higher depth increases the AI's analysis of the game state but also takes more time to compute, while a lower depth reduces computation time at the cost of potentially less accurate moves.
