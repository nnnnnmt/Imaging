#ifndef INPUTNODE_H
#define INPUTNODE_H

#include "utils.h"

#include "node.h"

class InputNodeWidget;
class ImageRegion;
class CanvasRegion;
class CropRegion;
class CanvasWindow;
class CropWindow;
class PreProcessingWindow;

QT_BEGIN_NAMESPACE
namespace Ui {
class InputNodeWidget;
class PreProcessingWindow;
class CropWindow;
class CanvasWindow;
}
QT_END_NAMESPACE

class InputNodeWidget : public QWidget
{
    Q_OBJECT
public:
    InputNodeWidget(QWidget *parent = nullptr);
    ~InputNodeWidget();

public:
    Ui::InputNodeWidget *ui;
};

class InputNode : public Node
{
public:
    InputNode(MainScene *scene,QGraphicsItem *parent = nullptr);
    virtual ~InputNode();
    virtual void initProxyWidget() override;
public:
    cv::Mat _coreImage;
    QPushButton *_imageSelect;
    QPushButton *_editButton;
    QCheckBox *_checkBox;
    QLabel *_label;
    InputNodeWidget *_widget = nullptr;
    QSlider *_xSlider;
    QSlider *_ySlider;
    QSlider *_scaleSlider;
    PreProcessingWindow *_preProcWindow;
    int _x = 0, _y = 0;
    double _scale = 1.0;
    double _showRatio;
    int _angle = 0;
    bool _preProcWinBuilt = false;
public:
    int type() const override {return 4001;}
    virtual void showEditWindow();
    virtual void selectImageAgain();
    void setCoreImage(cv::Mat coreImage);
    void setImageProperties(int x = -1, int y = -1, double s = -1, int a = -1);
    virtual void updateImage();
    void resizeWidget() override;
    void changeCanvasSize(int width, int height) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

// TO-DO: Pre-processing (Position, Size, Rotation, Crop)
class PreProcessingWindow : public QWidget
{
    Q_OBJECT
public:
    PreProcessingWindow(Node *superior = nullptr, cv::Mat image = cv::Mat());
    ~PreProcessingWindow();
    void closeEvent(QCloseEvent *e) override;
public:
    Ui::PreProcessingWindow *ui;
    InputNode* _superior;
    cv::Mat _coreImage;
    CanvasWindow* _canvasWindow;
    CropWindow* _cropWindow;
};

class CanvasWindow : public QWidget
{
    Q_OBJECT
public:
    CanvasWindow(Node *superior = nullptr, cv::Mat image = cv::Mat());
    ~CanvasWindow();
    QSize sizeHint() const override;
public:
    void xShift(int newX);
    void yShift(int newY);
    void scaleShift(double newScale);
    void apply();
public:
    Ui::CanvasWindow *ui;
    int _W, _H;
    QSpinBox* _xFillin;
    QSpinBox* _yFillin;
    QSlider* _scaleSlider;
    QSlider* _angleSlider;
    InputNode* _superior;
    CanvasRegion* _canvasRegion;
    PreProcessingWindow* _preProcWin;
};

class CanvasRegion : public QWidget
{
public:
    CanvasRegion(CanvasWindow *parent = nullptr);
    ~CanvasRegion();
    QSize sizeHint() const override;
    void updateCanvasSize();
    virtual void loadImage(cv::Mat img);
public:
    CanvasWindow* _canvasWindow;
    ImageRegion* _imageRegion;
    int _cW, _cH;
    int _W = 800, _H = 600;
    double _showRatio;
};

cv::Mat rotateImage(const cv::Mat& inputImage, double angle);

class ImageRegion : public QLabel
{
public:
    ImageRegion(CanvasRegion *parent = nullptr);
    ~ImageRegion();
    void loadImage(cv::Mat img, double ratioShow);
    virtual void reloadImage(cv::Mat img);
    //QSize sizeHint() const override;
public:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
public:
    virtual void inputSetX(int val);
    virtual void inputSetY(int val);
    void resetPos(int x = -1, int y = -1);
    void resetSize(double scale, int angle);
    //void inputSetAngle(int val);
public:
    cv::Mat _originalImage;
    double _ratioShow;
    CanvasRegion *_canvasRegion;
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

class CropWindow : public QWidget
{
    Q_OBJECT
public:
    CropWindow(Node *superior = nullptr, cv::Mat image = cv::Mat());
    ~CropWindow();
    QSize sizeHint() const override;
    //void drawSelectedRect();
    //void cropImage();

public:
    Ui::CropWindow *ui;
    InputNode* _superior;
    CropRegion* _cropRegion;
    PreProcessingWindow* _preProcWin;
    //cv::Mat _editedImage;
    //QList<cv::Mat> _editPath;
};

cv::Mat remove_background(const QString& apiKey, const cv::Mat& inputImage);

class CropRegion : public QLabel
{

public:
    CropRegion(CropWindow *parent = nullptr);
    ~CropRegion();
    void loadMat(cv::Mat src);
    void paintEvent(QPaintEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void aiCrop();
    virtual void apply();
    virtual void cancel();
    QSize sizeHint() const override;

public:
    CropWindow *_cropWindow;
    bool _mousePressed;
    bool _mouseCropChecking;
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

#endif // INPUTNODE_H
