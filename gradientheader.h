#ifndef GRADIENTHEADER_H
#define GRADIENTHEADER_H


#include "utils.h"

#define QChinese(str) QString(str)
class GradientHeader : public QWidget
{
public:
    GradientHeader(QWidget *parent = nullptr);
    ~GradientHeader();
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // GRADIENTHEADER_H
