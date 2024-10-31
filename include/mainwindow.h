#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <gomokuboard.h>
#include <gomokuai.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    // 处理鼠标点击
    void handleCellClicked(int row,int col);

private:
    Ui::MainWindow *ui;
    static const int m_width = 650;
    static const int m_height = 660; // 660
    GomokuBoard* m_board;
    bool isPlayerTurn;
    // 检查局面是否出现某一方的胜负
    void checkWin();
    void resetGame();

    void aiMakeMove(); // AI 下棋

    GomokuAI *ai;
};
#endif // MAINWINDOW_H
