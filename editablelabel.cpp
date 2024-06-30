#include "editablelabel.h"
#include <QDebug>


EditableLabel::EditableLabel(const QString &text, QWidget *parent)
    : QLabel(text, parent)
{
    setMinimumSize(200, 30);
}

void EditableLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    QLineEdit* editor = new QLineEdit(this->text(), this);
    editor->setFrame(true);  // 移除边框
    editor->setAlignment(Qt::AlignCenter);
    connect(editor, &QLineEdit::editingFinished, this, &EditableLabel::updateText);
    connect(editor, &QLineEdit::editingFinished, editor, &QLineEdit::deleteLater);
    editor->show();
    editor->setFocus();
}

void EditableLabel::updateText()
{
    QLineEdit* editor = qobject_cast<QLineEdit*>(sender());
    if (editor)
    {
        setText(editor->text());
        emit textChanged(editor->text());
    }
}
