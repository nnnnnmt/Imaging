#include "imagewidget.h"
#include <QDebug>
#include <QPainter>

#define WIDTH 300
#define HEIGHT 400

ImageWidget::ImageWidget(const QImage &img, QWidget *parent)
    : QWidget(parent), _img(img)
{
    //setFixedSize(WIDTH, HEIGHT);  // 设置固定大小
    setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                  "stop:0 rgb(230,250, 255),"
                  "stop:0.5 rgb(228,245,235),"
                  "stop:1.0 rgb(255, 250, 220));");
    //设置最小宽度
    setMinimumWidth(WIDTH+30);
    // 使用QLabel显示图片缩略图
    _label = new QLabel(this);
    _label->setAlignment(Qt::AlignCenter);
    setThumbnail(_img);
    QVBoxLayout *layout = new QVBoxLayout();
    //layout->addWidget(_scalingfactorLabel);
    layout->addWidget(_label);

    setLayout(layout);
}

ImageWidget::~ImageWidget()
{
    qDebug() << "~ImageWidget";
}


void ImageWidget::setThumbnail(const QImage &img) {
    QPixmap pixmap = QPixmap::fromImage(img);

    int borderWidth = 2;
    QPainter painter(&pixmap);
    painter.fillRect(0, 0, borderWidth, pixmap.height(), Qt::black);
    painter.fillRect(pixmap.width() - borderWidth, 0, borderWidth, pixmap.height(), Qt::black);
    painter.fillRect(0, 0, pixmap.width(), borderWidth, Qt::black);
    painter.fillRect(0, pixmap.height() - borderWidth, pixmap.width(), borderWidth, Qt::black);
    painter.end();
    // 缩放 customScaleFactor 倍后只显示800x600
    QPixmap scaledPixmap = pixmap.scaled(pixmap.size() * customScaleFactor, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // 调整偏移量以确保图片不会超出边界
    int maxOffsetX = std::max(0, (scaledPixmap.width() - WIDTH) / 2);
    int maxOffsetY = std::max(0, (scaledPixmap.height() - HEIGHT) / 2);
    offset.setX(std::clamp(offset.x(), -maxOffsetX, maxOffsetX));
    offset.setY(std::clamp(offset.y(), -maxOffsetY, maxOffsetY));

    // 裁切或填充为800*600
    QRect visibleRect = QRect((scaledPixmap.width() - WIDTH) / 2 + offset.x(),
                              (scaledPixmap.height() - HEIGHT) / 2 + offset.y(),
                              WIDTH, HEIGHT);
    QPixmap thumbnail = scaledPixmap.copy(visibleRect);
    QPixmap background(":/window/transparent.png");
    QPixmap combinedPixmap(thumbnail.size());
    QPainter newpainter(&combinedPixmap);
    newpainter.drawPixmap(0, 0, background);
    QRect thumbnailRect(0, 0, thumbnail.width(), thumbnail.height());
    newpainter.drawPixmap(thumbnailRect, thumbnail);
    _label->setPixmap(combinedPixmap);
    //_label->setPixmap(thumbnail);
}

void ImageWidget::showFullImage() {
    // 弹出显示全尺寸图片
    QPixmap fullImage = QPixmap::fromImage(_img);
    // debug输出图片大小
    qDebug() << "full image size: " << fullImage.size();
    // 弹窗显示全尺寸图片
    QDialog dialog(this);
    dialog.setWindowTitle("Full Image Preview");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QLabel *imageLabel = new QLabel(&dialog);
    imageLabel->setPixmap(fullImage);
    layout->addWidget(imageLabel);
    dialog.exec();
}

void ImageWidget::setPixmap(const QPixmap &pixmap)
{
    _img = pixmap.toImage();
    _label->setPixmap(pixmap);
    setThumbnail(_img);
}

void ImageWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        lastMousePosition = event->pos();
        isDragging = true;
    }
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        isDragging = false;
    }
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging) {
        QPoint delta = event->pos() - lastMousePosition;
        lastMousePosition = event->pos();

        // 更新偏移量
        offset -= delta;
        setThumbnail(_img);
    }
}

void ImageWidget::wheelEvent(QWheelEvent *event)
{
    int angle = event->angleDelta().y();
    if (angle > 0) {
        customScaleFactor *= 1.1;
    } else {
        customScaleFactor /= 1.1;
    }
    setThumbnail(_img);

}

void ImageWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    showFullImage();
}

CustomDockWidget::CustomDockWidget(const QString &title, QWidget *parent)
    : QDockWidget(title, parent) {
    QLabel *label = new QLabel("Press a key", this);
    setWidget(label);
}

CustomDockWidget::~CustomDockWidget()
{
    qDebug() << "~CustomDockWidget";
}

void CustomDockWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    //执行showFullImage()函数
    QWidget::mouseDoubleClickEvent(event);
    ImageWidget *imgWidget = dynamic_cast<ImageWidget*>(widget());
    if (imgWidget)
    {
        imgWidget->mouseDoubleClickEvent(event);
    }
}

void CustomDockWidget::wheelEvent(QWheelEvent *event)
{
    QDockWidget::wheelEvent(event);
    ImageWidget *imgWidget = dynamic_cast<ImageWidget*>(widget());
    if (imgWidget)
    {
        imgWidget->wheelEvent(event);
    }
}
