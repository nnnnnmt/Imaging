#ifndef UTILS_H
#define UTILS_H

#endif // UTILS_H
// Qt Core
#include <QCoreApplication>
#include <QTime>
#include <QTimer>
#include <QEvent>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QThread>
#include <QMap>
#include <QPoint>
#include <QRect>

// Qt Network
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// Qt Widgets
#include <QListWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <QSlider>
#include <QFileDialog>
#include <QMainWindow>
#include <QApplication>
#include <QMenuBar>
#include <QStackedWidget>
#include <QDockWidget>
#include <QDialog>
#include <QStatusBar>
#include <QDialogButtonBox>
#include <QTreeWidget>
#include <QSpinBox>

// Qt GUI
#include <QPixmap>
#include <QImage>
#include <QMovie>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionGraphicsItem>

// Qt Graphics View Framework
#include <QGraphicsPathItem>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsPathItem>

// Qt Input
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDropEvent>
#include <QMimeData>

// Qt Misc
#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QSignalMapper>
#include <QMenu>
#include <QAction>
#include <QScrollBar>

// opencv 涉及头文件
#include <opencv2/opencv.hpp>


QPixmap cvMat2QPixmap(cv::Mat image);
QImage cvMat2QImage(cv::Mat image);
cv::Mat channelUpgrade(cv::Mat image);
cv::Mat QImage2cvMat(const QImage& image);
