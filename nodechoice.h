#ifndef NODECHOICE_H
#define NODECHOICE_H

#include "utils.h"

#define QChinese(str) QString(str)

QT_BEGIN_NAMESPACE
namespace Ui {
class NodeChoice;
}
QT_END_NAMESPACE

class NodeChoice : public QWidget
{
    Q_OBJECT
public:
    NodeChoice(QWidget *parent = nullptr);
    ~NodeChoice();
public:
    Ui::NodeChoice *ui;
    /**
     * @brief 绘制鼠标按下后的效果
     * @param event
     */
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // NODECHOICE_H
