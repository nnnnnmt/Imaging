#ifndef EDITABLELABEL_H
#define EDITABLELABEL_H

#include "utils.h"
#define QChinese(str) QString(str)

class EditableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit EditableLabel(const QString& text, QWidget* parent = nullptr);
protected:
    /**
     * @brief 如果鼠标双击，创建一个QLineEdit对象，用于编辑文本
     * @param event
     */
    void mouseDoubleClickEvent(QMouseEvent* event) override ;
private slots:
    /**
     * @brief 编辑完成后，更新文本
     */
    void updateText() ;
signals:
    /**
     * @brief 文本改变时发出信号
     * @param newText
     */
    void textChanged(const QString &newText);
};

#endif // EDITABLELABEL_H
