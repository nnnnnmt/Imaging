#include "gradientheader.h"

GradientHeader::GradientHeader(QWidget *parent) : QWidget(parent) {

    // 创建和添加标题标签
    QLabel *titleLabel = new QLabel(QChinese("Imaging"), this);
    titleLabel->setStyleSheet("font-size: 25px; font-weight: bold; color: black;font-family:Microsoft YaHei;");

    // 标题显示图标
    QPixmap *icon = new QPixmap(":/window/imaging.jpg");
    QLabel *iconLabel = new QLabel(this);
    iconLabel->setPixmap(icon->scaled(40,40, Qt::KeepAspectRatio));
    iconLabel->setFixedSize(40, 40);

    // 创建水平布局
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(iconLabel);
    layout->addWidget(titleLabel);
    //居中
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(10, 0, 0, 0);
    layout->setSpacing(10);
    setLayout(layout);

}

GradientHeader::~GradientHeader()
{
    qDebug() << "~GradientHeader";
}

void GradientHeader::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    QLinearGradient gradient(0, 0, width(), 0);
    gradient.setColorAt(0.0, QColor(255, 232, 224)); // 渐变开始颜色
    gradient.setColorAt(0.2, QColor(255, 232, 224));
    gradient.setColorAt(0.5, QColor(240, 230, 210));
    gradient.setColorAt(0.8, QColor(230, 250, 255));
    gradient.setColorAt(1.0, QColor(230, 250, 255)); // 渐变结束颜色

    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());
}
