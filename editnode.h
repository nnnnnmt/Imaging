#ifndef EDITNODE_H
#define EDITNODE_H

#include "node.h"
#include "inputnode.h"
#include "utils.h"
#define QChinese(str) QString(str)

class EditImageRegion;
class EditCanvasRegion;
class EditCropRegion;
class EditCanvasWindow;
class EditCropWindow;
class EditWindow;

QT_BEGIN_NAMESPACE
namespace Ui {
class EditNodeWidget;
class EditWindow;
class EditCropWindow;
class EditCanvasWindow;
}
QT_END_NAMESPACE

class EditNodeWidget : public QWidget
{
    Q_OBJECT
public:
    EditNodeWidget(QWidget *parent = nullptr);
    ~EditNodeWidget();

public:
    Ui::EditNodeWidget *ui;
};

class EditNode : public Node
{
public:
    EditNode(MainScene *scene, QGraphicsItem *parent = nullptr);
    virtual void initProxyWidget() override;
public:
    cv::Mat _lastInputImage = cv::Mat();
    QPushButton *_editButton;
    QCheckBox *_checkBox;
    QLabel *_label;
    EditNodeWidget *_widget = nullptr;
    QSlider *_xSlider;
    QSlider *_ySlider;
    EditWindow *_editWindow;
    cv::Rect _cropRect;
    int _x = 0, _y = 0;
    double _scale = 1.0;
    int _angle = 0;
    bool _editWinBuilt;
    bool _editWinOpen;
public:
    int type() const override {return 4110;}
    virtual void showEditWindow();
    virtual void inputImageUpdate();
    void setImageProperties(int x = -1, int y = -1, double s = -1, int a = -1);
    virtual void updateImage();
    void execute() override;
    void resizeWidget() override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

class EditWindow : public QWidget
{
    Q_OBJECT
public:
    EditWindow(Node *superior = nullptr, cv::Mat image = cv::Mat());
    ~EditWindow();
    void closeEvent(QCloseEvent *e) override;
public:
    Ui::EditWindow *ui;
    EditNode* _superior;
    cv::Mat _originalImage;
    EditCanvasWindow* _canvasWindow;
    EditCropWindow* _cropWindow;
};

class EditCanvasWindow : public QWidget
{
    Q_OBJECT
public:
    EditCanvasWindow(Node *superior = nullptr, cv::Mat image = cv::Mat());
    ~EditCanvasWindow();
    QSize sizeHint() const override;
public:
    void xShift(int newX);
    void yShift(int newY);
    void scaleShift(double newScale);
    void apply();
public:
    Ui::EditCanvasWindow *ui;
    int _W, _H;
    QSpinBox* _xFillin;
    QSpinBox* _yFillin;
    QSlider* _scaleSlider;
    QSlider* _angleSlider;
    EditNode* _superior;
    EditCanvasRegion* _canvasRegion;
    EditWindow* _editWin;
};

class EditCanvasRegion : public QWidget
{
public:
    EditCanvasRegion(EditCanvasWindow *parent = nullptr);
    ~EditCanvasRegion();
    QSize sizeHint() const override;
    void paintEvent(QPaintEvent *e) override;
    void loadImage(cv::Mat img);
public:
    EditCanvasWindow* _canvasWindow;
    EditImageRegion* _imageRegion;
    int _cW, _cH;
    int _W = 800, _H = 600;
    double _showRatio;
};

class EditImageRegion : public QLabel
{
public:
    EditImageRegion(EditCanvasRegion *parent = nullptr);
    ~EditImageRegion();
    void loadImage(cv::Mat img, double ratioShow);
    void reloadImage(cv::Mat img);
    //QSize sizeHint() const override;
public:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
public:
    void inputSetX(int val);
    void inputSetY(int val);
    void resetPos(int x = -1, int y = -1);
    void resetSize(double scale, int angle);
    //void inputSetAngle(int val);
public:
    cv::Mat _originalImage;
    double _ratioShow;
    EditCanvasRegion *_canvasRegion;
    QPoint _originalPosition = QPoint(0, 0);
    QPoint _shownPosition = QPoint(0, 0);
    int _originalWidth, _originalHeight;    // 显示图片的原本尺寸（未做任何操作时的_shown）
    int _shownWidth, _shownHeight;
    int _angle = 0; //0 - 360
    double _scale = 1.00;
    bool _imageMoving;
    QPoint _startPoint;
    QPoint _endPoint;
};

class EditCropWindow : public QWidget
{
    Q_OBJECT
public:
    EditCropWindow(Node *superior = nullptr, cv::Mat image = cv::Mat());
    ~EditCropWindow();
    QSize sizeHint() const override;
    //void drawSelectedRect();
    //void cropImage();

public:
    Ui::EditCropWindow *ui;
    EditNode* _superior;
    EditCropRegion* _cropRegion;
    EditWindow* _editWin;
    //cv::Mat _editedImage;
    //QList<cv::Mat> _editPath;
};

class EditCropRegion : public QLabel
{

public:
    EditCropRegion(EditCropWindow *parent = nullptr);
    ~EditCropRegion();
    void loadMat(cv::Mat src);
    void paintEvent(QPaintEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    virtual void apply();
    virtual void cancel();
    QSize sizeHint() const override;

public:
    EditCropWindow *_cropWindow;
    bool _mousePressed;
    bool _mouseCropChecking;
    bool _cropShow;
    cv::Mat _originalImage;
    cv::Mat _currentCoreImage;
    cv::Mat _tempAiCroppedImage;
    QPixmap _shownImage;
    QRect _paintedRect;
    cv::Rect _croppedRect;
    //cv::Point cv_point;
    QPoint _paintedRectStart;
    QPoint _paintedRectEnd;
    //QPoint double_point;
    int W, H;
    int _upBorder, _downBorder, _leftBorder, _rightBorder;
    int _cropMode;  // 0: None, 1: MouseCrop, 2: AICrop
};

#endif // EDITNODE_H
