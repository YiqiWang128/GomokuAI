#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gomokuai.h"

#include <QMessageBox> // 用于弹出对话框

#include<QVBoxLayout>

#include <cstdlib>
#include <ctime>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_board = new GomokuBoard(this);
    ai = new GomokuAI(4,12);

    // 设置布局  - 可以解决棋盘绘制不完整的情况.
    QWidget *centralWidget = new QWidget(this);  // 创建中央部件
    setCentralWidget(centralWidget);  // 设置中央部件
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);  // 创建垂直布局
    layout->addWidget(m_board);  // 将GomokuBoard添加到布局中

    // 设置初始窗口的默认显示大小
    setFixedSize(m_width,m_height);

    // 信号和槽
    connect(m_board, &GomokuBoard::cellClicked,this,&MainWindow::handleCellClicked);

    isPlayerTurn=true;
}

// 处理鼠标点击
void MainWindow::handleCellClicked(int row,int col)
{
    if(m_board->getRowColBoard(row,col)==0)
    {
        int player = (isPlayerTurn == true ? 1 : 2);
        m_board->setBoard(row,col,player);

        m_board->update();

        checkWin();
        // 切换回合到AI
        isPlayerTurn = false;

        aiMakeMove();
    }
}

void MainWindow::checkWin() {
    if (m_board->checkWin(1)) {
        QMessageBox::information(this, "Game Over", "Player wins!");
        resetGame();
    } else if (m_board->checkWin(2)) {
        QMessageBox::information(this, "Game Over", "AI wins!");
        resetGame();
    }
}


void MainWindow::resetGame() {
    for (int row = 0; row < m_board->getBoardSize(); ++row) {
        for (int col = 0; col < m_board->getBoardSize(); ++col) {
            m_board->setBoard(row, col, 0);
        }
    }
    m_board->update();
    isPlayerTurn = true;
    //isAiThinking = false;
}

void MainWindow::aiMakeMove() {
    std::pair<int, int> bestMove = ai->getBestMove(m_board, ai->searchDepth, ai->killSearchDepth);
    if (bestMove.first != -1 && bestMove.second != -1) {
        m_board->setBoard(bestMove.first, bestMove.second, 2);
        m_board->update();
        checkWin(); // 检查AI是否胜利
    }

    isPlayerTurn = true;  // AI 落子后切换回玩家
    //isAiThinking = false; // AI 思考结束
}

MainWindow::~MainWindow()
{
    delete ui;
}
