#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include "utils.h"

#define QChinese(str) QString(str)
class ImageWidget : public QWidget
{
public:
    ImageWidget(const QImage &img, QWidget *parent = nullptr);
    ~ImageWidget();


    QImage _img;
    QLabel *_label;
    qreal customScaleFactor = 1.0;
    QPoint lastMousePosition;
    QPoint offset = QPoint(0, 0);
    bool isDragging = false;

    /**
     * @brief 设置输出缩略图显示
     * @param img
     */
    void setThumbnail(const QImage &img);
    /**
     * @brief 弹窗展示全尺寸图片
     */
    void showFullImage();

    void setPixmap(const QPixmap &pixmap);
    /**
     * @brief 中键拖动图片
     * @param event
     */
    void mousePressEvent(QMouseEvent *event);
    /**
     * @brief 中键拖动图片
     * @param event
     */
    void mouseReleaseEvent(QMouseEvent *event);
    /**
     * @brief 中键拖动图片
     * @param event
     */
    void mouseMoveEvent(QMouseEvent *event);
    /**
     * @brief 检测滚轮，改变scale factor
     * @param event
     */
    void wheelEvent(QWheelEvent *event);
    /**
     * @brief 鼠标双击，显示全尺寸图片
     * @param event
     */
    void mouseDoubleClickEvent(QMouseEvent *event);
};

class CustomDockWidget : public QDockWidget {
    Q_OBJECT

public:
    CustomDockWidget(const QString &title, QWidget *parent = nullptr);
    ~CustomDockWidget();
protected:
    /**
     * @brief 如果双击，弹出全尺寸预览窗口
     * @param event
     */
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    /**
     * @brief 检测滚轮，如果有事件传给ImageWidget
     * @param event
     */
    void wheelEvent(QWheelEvent *event) override;

};

#endif // IMAGEWIDGET_H
