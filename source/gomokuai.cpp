#include "gomokuai.h"
#include <limits>
#include <algorithm>
#include <vector>
#include <set>
#include <QDebug>


// 定义一些分数常量
const int SCORE_WIN = 100000000;  // 五连

const int INF = std::numeric_limits<int>::max();
const int NEG_INF = std::numeric_limits<int>::min();

std::pair<int, int> GomokuAI::getBestMove(GomokuBoard* board, int depth, int killSearchDepth) {
    std::pair<int, int> bestMove = {-1, -1};
    int bestScore = std::numeric_limits<int>::min();

    // 1. 优先处理必胜或阻挡对手必胜的情况
    std::pair<int, int> killMove = checkKillMove(board, 2);  // AI 是否有必胜机会
    if (killMove.first != -1) {
        return killMove;  // AI 直接落子取得胜利
    }


    killMove = checkKillMove(board, 1);  // 玩家是否有必胜机会
    if (killMove.first != -1) {
        qDebug() << "prevent palyer from winning at:(" << killMove.first << "," << killMove.second << ")";
        return killMove;  // AI 阻止玩家取胜
    }


    auto killSearchResult = killSearch(board,killSearchDepth, 2);
    qDebug() << "result of killSearch(flag,x,y):(" << killSearchResult.flag << "," << killSearchResult.bestMove.first << "," << killSearchResult.bestMove.second << ")" ;
    if(killSearchResult.flag == 1) {
        qDebug() << "find kill move for AI at:(" << killSearchResult.bestMove.first << "," << killSearchResult.bestMove.second << ")";
        return killSearchResult.bestMove;
    }


    // 2. 获取候选点并进行启发式排序
    std::vector<std::pair<int, int>> candidateMoves = getRoundCells(board);

    // 为候选点进行启发式评估并排序
    std::vector<std::pair<std::pair<int, int>, int>> moveScores;
    for (const auto& move : candidateMoves) {
        board->setBoard(move.first, move.second, 2);  // AI尝试落子
        int score = evaluateBoard(board);  // 评估局面
        moveScores.push_back({move, score});
        board->setBoard(move.first, move.second, 0);  // 撤回落子
    }

    // 按照分数排序候选点
    std::sort(moveScores.begin(), moveScores.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();

    // 3. 使用 Minimax 算法继续搜索
    for (const auto& moveScore : moveScores) {
        int row = moveScore.first.first;
        int col = moveScore.first.second;

        board->setBoard(row, col, 2);  // AI 尝试落子

        int flag = killSearch(board,killSearchDepth, 1).flag;
        int score = minimax(board, depth - 1, alpha, beta, false);
        //int score = minimax(board, depth - 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false);
        alpha = std::max(alpha,score);
        board->setBoard(row, col, 0);  // 撤回落子

        if(flag==-1)
        {
            score -= 100000000;
        }

        if (score > bestScore) {
            bestScore = score;
            bestMove = {row, col};
        }
    }

    if(bestMove.first==-1 && bestMove.second==-1)
        bestMove = moveScores[0].first;

    // 输出最佳分数到控制台
    qDebug() << "Best move so far at (" << bestMove.first << "," << bestMove.second << ") with score:" << bestScore;

    return bestMove;  // 返回最佳落子位置
}





// 检查是否有必胜机会或阻止对方必胜的机会
std::pair<int, int> GomokuAI::checkKillMove(GomokuBoard* board, int player) {
    int boardSize = board->getBoardSize();

    for (int row = 0; row < boardSize; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            if (board->getRowColBoard(row, col) == 0) {  // 仅检查空位
                board->setBoard(row, col, player);  // 模拟落子

                // 检查是否可以形成 5 连
                if (checkWin(board, player)) {
                    board->setBoard(row, col, 0);  // 恢复棋盘状态
                    return {row, col};  // 找到必胜点，直接返回
                }

                board->setBoard(row, col, 0);  // 撤回模拟落子
            }
        }
    }

    return {-1, -1};  // 没有找到必胜点
}

// 我（AI）下了某一步之后，交给玩家（AI的对手）去选择各种情况，假如玩家绝顶聪明，它会把最小分数留给我
// minimax计算的就是，我下了某一步之后，这个局面下，我能得到的分数
// 调用minimax需要枚举自己（AI）的每一种下法，选择for循环中多次调用minimax返回值的最大值作为下棋策略
// minimax 搜索的是红方的某个节点，返回的是该红方节点的儿子最小值，也就是beta（红方节点目前已知分数的上界）
int GomokuAI::minimax(GomokuBoard* board, int depth, int alpha, int beta, bool isMaximizingPlayer) {
    // 如果某方胜利或达到最大深度，停止递归
    if (checkWin(board, 2)) { // AI 胜利
        return SCORE_WIN;
    } else if (checkWin(board, 1)) { // 玩家胜利
        return -SCORE_WIN;
    } else if (depth == 0) {
        killSearchResult res = killSearch(board, searchDepth, 2); // 这里就用启发式搜索的depth来进行killSearch，因为这里深度已经很深了
        if(res.flag==1)
            return SCORE_WIN;
        else if(res.flag==-1)
            return -SCORE_WIN;
        else
            return evaluateBoard(board); // 评估当前棋盘
    }

    std::vector<std::pair<int, int>> candidateMoves = getRoundCells(board); // 获取候选落子点
    // 为候选点进行启发式评估并排序
    std::vector<std::pair<std::pair<int, int>, int>> moveScores;
    for (const auto& move : candidateMoves) {
        board->setBoard(move.first, move.second, 2);  // AI尝试落子
        int score = evaluateBoard(board);  // 评估局面
        moveScores.push_back({move, score});
        board->setBoard(move.first, move.second, 0);  // 撤回落子
    }

    // 按照分数排序候选点
    std::sort(moveScores.begin(), moveScores.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    if (isMaximizingPlayer) {
        int maxEval = NEG_INF;
        for (const auto& moveScore : moveScores) {
            int row = moveScore.first.first;
            int col = moveScore.first.second;
            board->setBoard(row, col, 2); // AI 落子
            int eval = minimax(board, depth - 1, alpha, beta, false);
            board->setBoard(row, col, 0); // 撤回落子
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) {
                break; // 剪枝
            }
        }
        return maxEval;
    } else {
        int minEval = INF;
        for (const auto& moveScore : moveScores) {
            int row = moveScore.first.first;
            int col = moveScore.first.second;
            board->setBoard(row, col, 1); // 玩家落子
            int eval = minimax(board, depth - 1, alpha, beta, true);
            board->setBoard(row, col, 0); // 撤回落子
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) {
                break; // 剪枝
            }
        }
        return minEval;
    }
}

// 获取棋盘上所有棋子周围的空位
std::vector<std::pair<int, int>> GomokuAI::getRoundCells(GomokuBoard* board) {
    std::vector<std::pair<int, int>> emptyCells;
    std::set<std::pair<int, int>> uniqueCells;
    int boardSize = board->getBoardSize();

    for (int row = 0; row < boardSize; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            if (board->getRowColBoard(row, col) != 0) {
                // 当前非空，获取周围的空位
                for (int dRow = -1; dRow <= 1; ++dRow) {
                    for (int dCol = -1; dCol <= 1; ++dCol) {
                        int newRow = row + dRow;
                        int newCol = col + dCol;
                        if (newRow >= 0 && newRow < boardSize && newCol >= 0 && newCol < boardSize) {
                            if (board->getRowColBoard(newRow, newCol) == 0 && uniqueCells.find({newRow, newCol}) == uniqueCells.end()) {
                                emptyCells.emplace_back(newRow, newCol);
                                uniqueCells.emplace(newRow, newCol);
                            }
                        }
                    }
                }
            }
        }
    }

    return emptyCells;
}

// 评估当前棋盘上的局面得分
int GomokuAI::evaluateBoard(GomokuBoard* board) {
    int score = 0;

    // 遍历棋盘，分别对每个方向的棋子进行评分
    for (int row = 0; row < board->getBoardSize(); ++row) {
        for (int col = 0; col < board->getBoardSize(); ++col) {
            int currentPlayer = board->getRowColBoard(row, col);
            if (currentPlayer != 0) {
                score += evaluateDirection(board, row, col, 1, 0);  // 水平
                score += evaluateDirection(board, row, col, 0, 1);  // 垂直
                score += evaluateDirection(board, row, col, 1, 1);  // 左上到右下
                score += evaluateDirection(board, row, col, 1, -1); // 右上到左下
            }
        }
    }
    return score;
}
int GomokuAI::evaluateDirection(GomokuBoard* board, int row, int col, int dRow, int dCol) {
    int boardSize = board->getBoardSize();
    int pattern[6] = {0};  // 存储当前方向上的模式

    // 正向探测6格
    for (int i = 0; i < 6; ++i) {
        int newRow = row + i * dRow;
        int newCol = col + i * dCol;

        if (newRow < 0 || newRow >= boardSize || newCol < 0 || newCol >= boardSize) {
            pattern[i] = 3;  // 超出边界
        } else {
            pattern[i] = board->getRowColBoard(newRow, newCol);  // 获取棋盘上的值
        }
    }

    // 根据pattern模式查询chessStatus并返回权重
    return stateScoreMap[chessStatus[getIndex(pattern[0], pattern[1], pattern[2], pattern[3], pattern[4], pattern[5])]];
}




// 检查是否有一方胜利
bool GomokuAI::checkWin(GomokuBoard* board, int player) {
    int boardSize = board->getBoardSize();
    for (int row = 0; row < boardSize; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            if (board->getRowColBoard(row, col) == player) {
                if (board->checkDirection(row, col, 1, 0, player) ||  // 水平
                    board->checkDirection(row, col, 0, 1, player) ||  // 垂直
                    board->checkDirection(row, col, 1, 1, player) ||  // 左上到右下
                    board->checkDirection(row, col, 1, -1, player)) { // 右上到左下
                    return true;
                }
            }
        }
    }
    return false;
}


