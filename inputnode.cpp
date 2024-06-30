#include "inputnode.h"
#include "mainscene.h"
#include "node.h"
#include "ui_inputnodewidget.h"
#include "ui_preprocessingwindow.h"
#include "ui_cropwindow.h"
#include "ui_canvaswindow.h"

InputNodeWidget::InputNodeWidget(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::InputNodeWidget)
{
    ui->setupUi(this);
}

InputNodeWidget::~InputNodeWidget()
{
    delete ui;
}

InputNode::InputNode(MainScene *scene, QGraphicsItem *parent)
    : Node(scene,parent)
{
    _functionNodeType = FunctionNodeType::NotFunctionNode;
    _nodeType = NodeType::input_inputNode;
    _name = "Input";
    qDebug() << "InputNode";
    _outputTerminals.append(new Terminal(_scene,TerminalType::outputTerminal,this));
    _canvasHeight = _scene->_canvasHeight;
    _canvasWidth = _scene->_canvasWidth;
    initializeBoundingRect();
    initializeTerminal();
    initProxyWidget();
}

InputNode::~InputNode()
{
    qDebug() << "~InputNode";
}

void InputNode::initProxyWidget()
{
    _widget = new InputNodeWidget;
    _imageSelect = _widget->ui->pushButton;
    _xSlider = _widget->ui->xSlider;
    _ySlider = _widget->ui->ySlider;
    _scaleSlider = _widget->ui->scaleSlider;
    _editButton = _widget->ui->pushButton_2;
    _checkBox = _widget->ui->checkBox;
    _label = _widget->ui->label;
    _label->setAlignment(Qt::AlignCenter);

    _xSlider->setRange(0, _canvasWidth);
    _xSlider->setValue(0);
    _ySlider->setRange(0, _canvasHeight);
    _ySlider->setValue(0);
    _scaleSlider->setValue(100);
    _scaleSlider->setRange(1, 200);

    connect(_imageSelect, &QPushButton::clicked, [this]() {
        QString filename = QFileDialog::getOpenFileName(nullptr, QChinese("选择图片"), ".", QChinese("图片文件(*.png *.jpg *.jpeg *.bmp)"));
        if (filename.isEmpty())
        {
            return;
        }
        _coreImage = cv::imread(filename.toStdString(), cv::IMREAD_UNCHANGED);
        _coreImage = channelUpgrade(_coreImage);

        _xSlider->setRange(5 - _coreImage.cols, _canvasWidth - 5);
        _xSlider->setValue(0);
        _ySlider->setRange(5 - _coreImage.rows, _canvasHeight - 5);
        _ySlider->setValue(0);
        _xSlider->setEnabled(true);
        _ySlider->setEnabled(true);
        _scaleSlider->setEnabled(true);
        if (!_preProcWinBuilt) {
            _preProcWindow = new PreProcessingWindow(this, _coreImage);
            _preProcWinBuilt = true;
            _preProcWindow->close();
        }

        selectImageAgain();
        //qDebug() << _canvasWidth << ' ' << _canvasHeight;
        _outputImage = cv::Mat::zeros(_canvasHeight, _canvasWidth, CV_8UC4);

        cv::Rect roi(0,0,_coreImage.cols,_coreImage.rows);
        if (roi.width > _canvasWidth) {
            roi.width = _canvasWidth;
        }
        if (roi.height > _canvasHeight) {
            roi.height = _canvasHeight;
        }
        //qDebug()<<_coreImage.cols<<" "<<_coreImage.rows<<" ";
        //qDebug()<<roi.width<<" "<<roi.height;
        _coreImage(roi).copyTo(_outputImage(roi));
        QImage img = cvMat2QImage(_outputImage);

        img = img.width() * 3 > img.height() * 5 ? img.scaledToWidth(250) : img.scaledToHeight(150);
        nodeSetPixmap(QPixmap::fromImage(img),_label);//_label->setPixmap(QPixmap::fromImage(img));
    });

    // 预处理窗口按钮
    connect(_editButton, &QPushButton::clicked, [this]() {
        // 创建和显示编辑窗口
        showEditWindow();
    });

    connect(_widget->ui->closeButton, &QPushButton::clicked, [this]() {
        this->_scene->_deleteItem((QGraphicsItem*)this);
    });

    _xSlider->setEnabled(false);
    connect(_xSlider, &QSlider::valueChanged, [this](int value) {
        _preProcWindow->_canvasWindow->_canvasRegion->_imageRegion->inputSetX(value);
        setImageProperties(value,-1,-1);
        updateImage();
    });

    _ySlider->setEnabled(false);
    //layout->addWidget(_ySlider);
    connect(_ySlider, &QSlider::valueChanged, [this](int value) {
        _preProcWindow->_canvasWindow->_canvasRegion->_imageRegion->inputSetY(value);
        setImageProperties(-1, value, -1);
        updateImage();
    });

    _scaleSlider->setEnabled(false);
    connect(_scaleSlider, &QSlider::valueChanged, [this](int value) {
        _preProcWindow->_canvasWindow->_canvasRegion->_imageRegion->resetSize((double)value / 100, -1);
        setImageProperties(-1, -1, (double)value / 100);
        updateImage();
    });
    // 创建一个checkbox 是否显示preview
    // 默认勾选
    _checkBox->setCheckState(Qt::Checked);
    // 显示outputImage
    if (_coreImage.empty())
    {

        //cv::resize(nullimg, nullimg, cv::Size(_canvasWidth, _canvasHeight));
        QImage img(":/window/null.png");
        //qDebug()<<img.width()<<" "<<img.height();
        img = img.width() * 3 > img.height() * 5 ? img.scaledToWidth(250) : img.scaledToHeight(150);
        nodeSetPixmap(QPixmap::fromImage(img),_label);//_label->setPixmap(QPixmap::fromImage(img));
    }
    else
    {
        QImage img = cvMat2QImage(_coreImage);
        img = img.width() * 3 > img.height() * 5 ? img.scaledToWidth(250) : img.scaledToHeight(150);
        nodeSetPixmap(QPixmap::fromImage(img),_label);//_label->setPixmap(QPixmap::fromImage(img));
    }
    connect(_checkBox, &QCheckBox::stateChanged, [this](int state) {
        this->_label->setVisible(state == Qt::Checked);
    });
    _nodeSettings = _scene->addWidget((QWidget*)_widget);

    //proxyWidget->setAcceptedMouseButtons(Qt::RightButton);
    _nodeSettings->setParentItem(this);
    _nodeSettings->setPos(0, 0);
}

void InputNode::resizeWidget()
{
    if (_widget != nullptr) {
        _widget->setGeometry(0,0,300,_boundingRect.height());
        _widget->ui->frame->setGeometry(0, 0, 300, _boundingRect.height());
        _r = 50.0 / (double)_boundingRect.height();
    }
}

void InputNode::showEditWindow()
{
    // 创建编辑窗口
    if (_coreImage.empty()) {
        //QLabel *label = new QLabel(QChinese("请先选择图片"));
        //label->show();
        return;
    }
    _xSlider->setEnabled(false);
    _ySlider->setEnabled(false);
    _scaleSlider->setEnabled(false);
    _editButton->setEnabled(false);
    _imageSelect->setEnabled(false);
    _preProcWindow->show();
}

void InputNode::setCoreImage(cv::Mat coreImage)
{
    _coreImage = coreImage;
}

void InputNode::changeCanvasSize(int width, int height)
{
    if (_canvasWidth != width || _canvasHeight != height) {
        _canvasWidth = width;
        _canvasHeight = height;
        if (!_outputImage.empty()) {
            selectImageAgain();
            _outputImage = cv::Mat::zeros(_canvasHeight, _canvasWidth, CV_8UC4);

            cv::Rect roi(0,0,_coreImage.cols,_coreImage.rows);
            if (roi.width > _canvasWidth) {
                roi.width = _canvasWidth;
            }
            if (roi.height > _canvasHeight) {
                roi.height = _canvasHeight;
            }
            //qDebug()<<_coreImage.cols<<" "<<_coreImage.rows<<" ";
            //qDebug()<<roi.width<<" "<<roi.height;
            _coreImage(roi).copyTo(_outputImage(roi));
            QImage img = cvMat2QImage(_outputImage);
            img = img.width() * 3 > img.height() * 5 ? img.scaledToWidth(250) : img.scaledToHeight(150);
            nodeSetPixmap(QPixmap::fromImage(img),_label);//_label->setPixmap(QPixmap::fromImage(img));

            _preProcWindow->_canvasWindow->_canvasRegion->updateCanvasSize();
            _preProcWindow->_canvasWindow->_canvasRegion->loadImage(_coreImage);
        }
    }
}


void InputNode::selectImageAgain()
{
    _x = 0;
    _y = 0;
    _scale = 1.00;
    _xSlider->setValue(0);
    _ySlider->setValue(0);
    _angle = 0;
    _preProcWindow->_coreImage = _coreImage;

    _preProcWindow->_canvasWindow->_xFillin->setValue(0);
    _preProcWindow->_canvasWindow->_yFillin->setValue(0);
    _preProcWindow->_canvasWindow->_scaleSlider->setValue(100);
    _preProcWindow->_canvasWindow->_angleSlider->setValue(0);

    double cw = _preProcWindow->_canvasWindow->_canvasRegion->_cW;
    double ch = _preProcWindow->_canvasWindow->_canvasRegion->_cH;
    double ratioW = 750.0 / cw;
    double ratioH = 400.0 / ch;
    if (ratioW > ratioH) {
        _preProcWindow->_canvasWindow->_canvasRegion->_showRatio = ratioH;
        _preProcWindow->_canvasWindow->_canvasRegion->setGeometry(0, 0, 750.0 * ratioH, 400);
        _preProcWindow->_canvasWindow->_canvasRegion->_imageRegion->loadImage(_coreImage, ratioH);
    }
    else {
        _preProcWindow->_canvasWindow->_canvasRegion->_showRatio = ratioW;
        _preProcWindow->_canvasWindow->_canvasRegion->setGeometry(0, 0, 750, 400.0 * ratioW);
        _preProcWindow->_canvasWindow->_canvasRegion->_imageRegion->loadImage(_coreImage, ratioW);
    }
    _preProcWindow->_canvasWindow->_canvasRegion->_imageRegion->_originalPosition = QPoint(0, 0);
    _preProcWindow->_canvasWindow->_canvasRegion->_imageRegion->_shownPosition = QPoint(0, 0);
    _preProcWindow->_canvasWindow->_canvasRegion->_imageRegion->_angle = 0;
    _preProcWindow->_canvasWindow->_canvasRegion->_imageRegion->_scale = 1.00;

    _preProcWindow->_cropWindow->_cropRegion->loadMat(_coreImage);
}

void InputNode::setImageProperties(int x, int y, double s, int a)
{
    if (x != -1) _x = x;
    if (y != -1) _y = y;
    if (s != -1) _scale = s;
    if (a != -1) _angle = a;
}

void InputNode::updateImage()
{
    //qDebug()<<"input Image update called";
    if (_coreImage.channels()==3) {
        _outputImage = cv::Mat::zeros(_canvasHeight, _canvasWidth, CV_8UC3);
    }
    else {
        _outputImage = cv::Mat::zeros(_canvasHeight, _canvasWidth, CV_8UC4);
    }

   // qDebug()<<"scale "<<_scale;
    //qDebug()<<"_x "<<_x<<" _y "<<_y;
    // 缩放
    cv::Size scaledSize((double)_coreImage.cols * _scale, (double)_coreImage.rows * _scale);
    cv::Mat scaledImage;
    cv::resize(_coreImage, scaledImage, scaledSize);

    // 旋转
    cv::Mat shownImage = rotateImage(scaledImage, _angle);
    //qDebug()<<"shownImage: "<<shownImage.cols<<" "<<shownImage.rows;

    if (_x + shownImage.cols < 5) {
        _x = 5 - shownImage.cols;
        _xSlider->setValue(5 - shownImage.cols);
    }
    if (_y + shownImage.rows < 5) {
        _y = 5 - shownImage.rows;
        _ySlider->setValue(5 - shownImage.rows);
    }
    _xSlider->setRange(5 - shownImage.cols, _canvasWidth - 5);
    _ySlider->setRange(5 - shownImage.rows, _canvasHeight - 5);

    cv::Rect dstROI(_x, _y, shownImage.cols, shownImage.rows);
    cv::Rect srcROI(0, 0, dstROI.width, dstROI.height);

    if (dstROI.width + dstROI.x > _canvasWidth) {
        dstROI.width = _canvasWidth - dstROI.x;
    }
    if (dstROI.height + dstROI.y > _canvasHeight) {
        dstROI.height = _canvasHeight - dstROI.y;
    }
    if (_x < 0) {
        dstROI.x = 0;
        srcROI.x = -_x;
        dstROI.width += _x;
    }
    if (_y < 0) {
        dstROI.y = 0;
        srcROI.y = -_y;
        dstROI.height += _y;
    }

    srcROI.width = dstROI.width;
    srcROI.height = dstROI.height;

    //qDebug()<<"srcROI: "<<srcROI.x<<" "<<srcROI.y<<" "<<srcROI.width<<" "<<srcROI.height;
    //qDebug()<<"dstROI: "<<dstROI.x<<" "<<dstROI.y<<" "<<dstROI.width<<" "<<dstROI.height;

    shownImage(srcROI).copyTo(_outputImage(dstROI));

    //QImage img = QImage((const unsigned char*)(_outputImage.data), _outputImage.cols, _outputImage.rows, QImage::Format_RGB888).rgbSwapped();
    QImage img = cvMat2QImage(_outputImage);
    img = img.width() * 3 > img.height() * 5 ? img.scaledToWidth(250) : img.scaledToHeight(150);

    nodeSetPixmap(QPixmap::fromImage(img),_label);//_label->setPixmap(QPixmap::fromImage(img));
}

void InputNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //qDebug()<<"r";
    QString sty = QString("border-radius: 15px;"
                          "border: 3px solid %1;"
                          "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                          "stop:0 rgb(255,204,150), "
                          "stop:%2 rgb(255,204,150), "
                          "stop:%3 rgb(255,250,240), "
                          "stop:1 rgb(255, 250, 240));");
    if (_widget != nullptr) {
        if (isSelected()) _widget->ui->frame->setStyleSheet(sty.arg("blue").arg(QString::number(_r)).arg(QString::number(_r+0.0001)));
        else _widget->ui->frame->setStyleSheet(sty.arg("black").arg(QString::number(_r)).arg(QString::number(_r+0.0001)));
    }
    return;
}

// PreProcessingWindow
PreProcessingWindow::PreProcessingWindow(Node *superior, cv::Mat image)
    : QWidget(nullptr),
    ui(new Ui::PreProcessingWindow)
{
    ui->setupUi(this);
    setWindowTitle(QChinese("预处理"));
    setFixedSize(800, 600);
    _superior = (InputNode*)superior;

    QPushButton *cropButton = ui->cropButton;   //new QPushButton(QChinese("裁剪"), this);
    QPushButton *canvasButton = ui->canvasButton;   //new QPushButton(QChinese("画布"), this);
    QPushButton *acceptButton = ui->acceptButton;   //new QPushButton(QChinese("完成"), this);

    _coreImage = image;
    _cropWindow = new CropWindow(superior, _coreImage);
    _cropWindow->_preProcWin = this;
    _canvasWindow = new CanvasWindow(superior, _coreImage);
    _canvasWindow->_preProcWin = this;
    //qDebug()<<"built";
    QStackedWidget* preProcStackWindow = ui->stackedWidget; //new QStackedWidget;
    preProcStackWindow->addWidget(_cropWindow);
    preProcStackWindow->addWidget(_canvasWindow);
    preProcStackWindow->setCurrentIndex(2);

    connect(cropButton, &QPushButton::clicked, [preProcStackWindow]() {
        preProcStackWindow->setCurrentIndex(2);
    });
    connect(canvasButton, &QPushButton::clicked, [preProcStackWindow]() {
        preProcStackWindow->setCurrentIndex(3);
    });
    connect(acceptButton, &QPushButton::clicked, [this]() {
        // 传递 _coreImage
        this->_superior->setCoreImage(this->_cropWindow->_cropRegion->_currentCoreImage);
        // qDebug()<<"loaded new core image";
        // 传递 _imageGeometry

        int x = this->_canvasWindow->_canvasRegion->_imageRegion->_shownPosition.x();
        // qDebug()<<"_shownPositionX: "<<x;
        int y = this->_canvasWindow->_canvasRegion->_imageRegion->_shownPosition.y();
        //qDebug()<<"_shownPositionY: "<<y;
        double ratio = this->_canvasWindow->_canvasRegion->_showRatio;
        double scale = this->_canvasWindow->_canvasRegion->_imageRegion->_scale;
        int angle = this->_canvasWindow->_canvasRegion->_imageRegion->_angle;
        //qDebug()<<"inf"<<x<<" "<<y<<" "<<ratio;
        this->_superior->setImageProperties(
            qCeil((double)x / ratio), qCeil((double)y / ratio), scale, angle
            );
        this->_superior->updateImage();
        this->_superior->_xSlider->setValue((double)x/ratio);
        this->_superior->_ySlider->setValue((double)y/ratio);

        double sn = qSin((double)angle * M_PI / 180.0);
        double cs = qCos((double)angle * M_PI / 180.0);
        if (sn < 0) {
            sn = -sn;
        }
        if (cs < 0) {
            cs = -cs;
        }

        int coreWidth = this->_cropWindow->_cropRegion->_currentCoreImage.cols;
        int coreHeight = this->_cropWindow->_cropRegion->_currentCoreImage.rows;
        int shownWidth = qCeil(qCeil(((double)coreWidth * cs + (double)coreHeight * sn)) * scale);
        int shownHeight = qCeil(qCeil(((double)coreWidth * sn + (double)coreHeight * cs)) * scale);

        this->_superior->_xSlider->setRange(
            5 - shownWidth,
            this->_superior->_canvasWidth - 5
            );

        this->_superior->_ySlider->setRange(
            5 - shownHeight,
            this->_superior->_canvasHeight - 5
            );
        this->_superior->_scaleSlider->setValue(scale * 100);
        this->close();
    });
}

PreProcessingWindow::~PreProcessingWindow()
{
    qDebug()<<"~PreProcessingWindow";
}

void PreProcessingWindow::closeEvent(QCloseEvent *e)
{
    QWidget::closeEvent(e);
    _superior->_xSlider->setEnabled(true);
    _superior->_ySlider->setEnabled(true);
    _superior->_scaleSlider->setEnabled(true);
    _superior->_editButton->setEnabled(true);
    _superior->_imageSelect->setEnabled(true);
}

// CanvasWindow
CanvasWindow::CanvasWindow(Node *superior, cv::Mat image)
    : QWidget(nullptr),
    ui(new Ui::CanvasWindow)
{
    ui->setupUi(this);
    _superior = (InputNode*)superior;

    setWindowTitle(QChinese("画布"));
    setFixedSize(800, 550);

    QVBoxLayout *canvasRegionLayout = ui->canvasRegionLayout;
    canvasRegionLayout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    _canvasRegion = new CanvasRegion(this);
    _canvasRegion->loadImage(image);
    canvasRegionLayout->addWidget(_canvasRegion);

    _xFillin = ui->xSpinBox;
    _xFillin->setRange(-image.cols * _canvasRegion->_showRatio + 5, _W - 5);
    _xFillin->setSingleStep(1);
    _xFillin->setValue(0);
    connect(_xFillin, QOverload<int>::of(&QSpinBox::valueChanged), [this](int newValue) {
        _canvasRegion->_imageRegion->resetPos(newValue, -1);
    });

    _yFillin = ui->ySpinBox;
    _yFillin->setRange(-image.rows * _canvasRegion->_showRatio + 5, _H - 5);
    _yFillin->setSingleStep(1);
    _yFillin->setValue(0);
    connect(_yFillin, QOverload<int>::of(&QSpinBox::valueChanged), [this](int newValue) {
        _canvasRegion->_imageRegion->resetPos(-1, newValue);
    });

    _scaleSlider = ui->scaleSlider;
    _scaleSlider->setRange(1, 200);
    _scaleSlider->setSingleStep(1);
    _scaleSlider->setValue(100);
    connect(_scaleSlider, &QSlider::valueChanged, [this](int newValue) {
        _canvasRegion->_imageRegion->resetSize((double)newValue / (double)100, -1);
    });

    _angleSlider = ui->rotateSlider;
    _angleSlider->setRange(0, 360);
    _angleSlider->setValue(0);
    connect(_angleSlider, &QSlider::valueChanged, [this](int newValue) {
        _canvasRegion->_imageRegion->resetSize(-1, newValue);
    });

    show();
}

CanvasWindow::~CanvasWindow()
{
    qDebug()<<"~CanvasWindow";
}

QSize CanvasWindow::sizeHint() const
{
    return QSize(1000, 750);
}

// CanvasRegion & ImageRegion
CanvasRegion::CanvasRegion(CanvasWindow *parent) : QWidget((QWidget*)parent)
{
    _canvasWindow = parent;
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setStyleSheet("background-color: white;");
    _imageRegion = new ImageRegion(this);
    updateCanvasSize();
}

CanvasRegion::~CanvasRegion()
{
    qDebug()<<"~CanvasRegion";
}

void CanvasRegion::updateCanvasSize()
{
    _cW = _canvasWindow->_superior->_canvasWidth;
    _cH = _canvasWindow->_superior->_canvasHeight;

    //qDebug()<<"updateCanvasSize: ";
    double ratioW = 750.0 / (double)_cW;
    double ratioH = 400.0 / (double)_cH;

    if (ratioW > ratioH) {
        setGeometry(0, 0, (double)_cW * ratioH, 400);
        //qDebug()<<"newGeometry: "<<(double)_cW * ratioH<<" "<<400;
        _showRatio = ratioH;
        _H = 400;
        _W = (double)_cW * ratioH;
        _canvasWindow->_W = (double)_cW * ratioH;
        _canvasWindow->_H = 400;
    }
    else {
        setGeometry(0, 0, 750, (double)_cH * ratioW);
        //qDebug()<<"newGeometry: "<<750<<" "<<(double)_cH * ratioW;
        _showRatio = ratioW;
        _H = (double)_cH * ratioW;
        _W = 750;
        _canvasWindow->_W = 750;
        _canvasWindow->_H = (double)_cH * ratioW;
    }
    updateGeometry();
    //qDebug()<<"H "<<_H<<" W "<<_W<<" cH "<<_cH<<" cW "<<_cW;
    //qDebug()<<"ratioW "<<ratioW<<" ratioH "<<ratioH;
}

QSize CanvasRegion::sizeHint() const
{
    //qDebug()<<"sizeHint: "<<_W<<" "<<_H;
    return QSize(_W, _H);
}

void CanvasRegion::loadImage(cv::Mat img)
{
    //qDebug()<<"showRatio: "<<_showRatio;
    _imageRegion->loadImage(img, _showRatio);
    //qDebug()<<"finish";
}

cv::Mat rotateImage(const cv::Mat& inputImage, double angle) {  // mark: to be debugged
    //qDebug()<<"rotateImage called";
    angle = 360 - angle;
   // qDebug()<<"angle "<<angle;
    int inputWidth = inputImage.cols;
    int inputHeight = inputImage.rows;
    cv::Size rotatedSize = cv::RotatedRect(cv::Point2f(inputWidth / 2.0, inputHeight / 2.0), inputImage.size(), angle).boundingRect().size();
    cv::Mat rotatedImage(rotatedSize, inputImage.type(), cv::Scalar(0, 0, 0));
    cv::Mat rotationMatrix = cv::getRotationMatrix2D(cv::Point2f(rotatedSize.width / 2.0, rotatedSize.height / 2.0), angle, 1.0);
    //qDebug()<<rotatedSize.width<<" "<<rotatedSize.height;
    //qDebug()<<inputWidth<<" "<<inputHeight;
    cv::Mat roi(rotatedImage, cv::Rect((rotatedSize.width - inputWidth) / 2, (rotatedSize.height - inputHeight) / 2, inputWidth, inputHeight));
    inputImage.copyTo(roi);
    cv::warpAffine(rotatedImage, rotatedImage, rotationMatrix, rotatedSize, cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    return rotatedImage;
}

ImageRegion::ImageRegion(CanvasRegion *parent) : QLabel((QWidget*)parent)
{
    setMouseTracking(true);
    _imageMoving = false;
    _canvasRegion = parent;
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

ImageRegion::~ImageRegion()
{
    qDebug()<<"~ImageRegion";
}

void ImageRegion::resetPos(int x, int y)
{
    if (x != -1) {
        _shownPosition.setX(x);
    }
    if (y != -1) {
        _shownPosition.setY(y);
    }
    setGeometry(_shownPosition.x(), _shownPosition.y(), _shownWidth, _shownHeight);
}

void ImageRegion::resetSize(double scale, int angle)
{
    if (scale != -1) {
        _scale = scale;
    }
    if (angle != -1) {
        _angle = angle;
    }

    double sn = qSin((double)_angle * M_PI / 180.0);
    double cs = qCos((double)_angle * M_PI / 180.0);
    if (sn < 0) {
        sn = -sn;
    }
    if (cs < 0) {
        cs = -cs;
    }

    _shownWidth = qCeil(qCeil(((double)_originalWidth * cs + (double)_originalHeight * sn)) * _scale);
    _shownHeight = qCeil(qCeil(((double)_originalWidth * sn + (double)_originalHeight * cs)) * _scale);

    //qDebug()<<"original: "<<_originalWidth<<" "<<_originalHeight;

    //qDebug()<<"shown: "<<_shownWidth<<" "<<_shownHeight;
    if (_shownPosition.x() + _shownWidth <= 4) {
        _shownPosition.setX(5 - _shownWidth);
    }
    if (_shownPosition.y() + _shownHeight <= 4) {
        _shownPosition.setY(5 - _shownHeight);
    }

    _canvasRegion->_canvasWindow->_xFillin->setRange(5 - _shownWidth, _canvasRegion->_canvasWindow->_W - 5);
    _canvasRegion->_canvasWindow->_yFillin->setRange(5 - _shownHeight, _canvasRegion->_canvasWindow->_H - 5);

    setGeometry(_shownPosition.x(), _shownPosition.y(), _shownWidth, _shownHeight);

    cv::Mat displayImg;
    cv::resize(_originalImage, displayImg, cv::Size(_originalImage.cols * _scale, _originalImage.rows * _scale));

    //QImage displayQImg = QImage((const unsigned char*)(displayImg.data), displayImg.cols, displayImg.rows, displayImg.step, QImage::Format_RGB888).rgbSwapped();
    QImage displayQImg = cvMat2QImage(displayImg);
    QPixmap shownPixmap = QPixmap::fromImage(displayQImg);

    QTransform transform;
    transform.rotate(_angle);
    shownPixmap = shownPixmap.transformed(transform);
    setPixmap(shownPixmap);
}

// 加载图像：只有在创建时发起
void ImageRegion::loadImage(cv::Mat img, double ratioShow)
{
    qDebug()<<"ImageRegion::loadImage()";
    // 将img按比例缩放，作为_originalImage（_coreImage : _originalImage = _canvasSize : _canvasRegionSize）

    _ratioShow = ratioShow;
    cv::resize(img, img, cv::Size(img.cols * ratioShow, img.rows * ratioShow));
    _originalImage = img;

    _originalWidth = img.cols;
    _originalHeight = img.rows;
    _shownWidth = img.cols;
    _shownHeight = img.rows;

    setGeometry(_shownPosition.x(), _shownPosition.y(), _shownWidth, _shownHeight);

    // 显示
    //QImage displayQImg = QImage((const unsigned char*)(img.data), img.cols, img.rows, img.step, QImage::Format_RGB888).rgbSwapped();
    QImage displayQImg = cvMat2QImage(img);
    setPixmap(QPixmap::fromImage(displayQImg));

}

void ImageRegion::reloadImage(cv::Mat img)
{
    cv::resize(img, img, cv::Size(img.cols * _ratioShow, img.rows * _ratioShow));
    _originalImage = img;
    _originalWidth = img.cols;
    _originalHeight = img.rows;

    double sn = qSin((double)_angle * M_PI / 180.0);
    double cs = qCos((double)_angle * M_PI / 180.0);
    if (sn < 0) {
        sn = -sn;
    }
    if (cs < 0) {
        cs = -cs;
    }

    _shownWidth = qCeil(qCeil(((double)_originalWidth * cs + (double)_originalHeight * sn)) * _scale);
    _shownHeight = qCeil(qCeil(((double)_originalWidth * sn + (double)_originalHeight * cs)) * _scale);

    if (_shownPosition.x() + _shownWidth <= 4) {
        //qDebug()<<"adjusted";
        _shownPosition.setX(5 - _shownWidth);

        //qDebug()<<_shownPosition.x();
    }
    if (_shownPosition.y() + _shownHeight <= 4) {
        //qDebug()<<"adjusted";
        _shownPosition.setY(5 - _shownHeight);
        //qDebug()<<_shownPosition.y();
    }

    _canvasRegion->_canvasWindow->_xFillin->setRange(5 - _shownWidth, _canvasRegion->_canvasWindow->_W - 5);
    _canvasRegion->_canvasWindow->_yFillin->setRange(5 - _shownHeight, _canvasRegion->_canvasWindow->_H - 5);

    setGeometry(_shownPosition.x(), _shownPosition.y(), _shownWidth, _shownHeight);

    cv::Mat displayImg;
    cv::resize(_originalImage, displayImg, cv::Size(_originalImage.cols * _scale, _originalImage.rows * _scale));

    //QImage displayQImg = QImage((const unsigned char*)(displayImg.data), displayImg.cols, displayImg.rows, displayImg.step, QImage::Format_RGB888).rgbSwapped();
    QImage displayQImg = cvMat2QImage(displayImg);
    QPixmap shownPixmap = QPixmap::fromImage(displayQImg);

    QTransform transform;
    transform.rotate(_angle);
    shownPixmap = shownPixmap.transformed(transform);
    setPixmap(shownPixmap);
    //qDebug()<<"ok";
}

// 按下鼠标：开始拖动
void ImageRegion::mousePressEvent(QMouseEvent *e)
{
    QLabel::mousePressEvent(e);
    if (e->button() == Qt::LeftButton)
    {
        // 设置“拖动中”状态，记录鼠标落点以供后续计算
        _imageMoving = true;
        _startPoint = e->pos();
        _endPoint = e->pos();
    }
}

// 释放鼠标：结束拖动
void ImageRegion::mouseReleaseEvent(QMouseEvent *e)
{
    //qDebug()<<"mouserelease";
    QLabel::mouseReleaseEvent(e);
    if (e->button() == Qt::LeftButton && _imageMoving)
    {
        // 结束“拖动中”状态，更新_originalPosition供下一次拖动时计算
        _imageMoving = false;
        _originalPosition = _shownPosition;
    }
}

// 鼠标移动：（按下时）拖动图片
void ImageRegion::mouseMoveEvent(QMouseEvent *e)
{
    if (_imageMoving)
    {
        // 不断更新最终落点，供绘图
        //qDebug()<<"mouseMove";
        _endPoint = e->pos();

        //qDebug()<<"startPoint: "<<_startPoint<<" endPoint: "<<_endPoint;

        // 计算图像显示位置并实时显示
        QPoint dPos = _endPoint - _startPoint;
        _shownPosition = mapToParent(dPos);
        //qDebug()<<"shownPosition: "<<_shownPosition;

        if (_shownPosition.x() <= -_shownWidth) {
            _shownPosition.setX(5 - _shownWidth);
        }
        if (_shownPosition.y() <= -_shownHeight) {
            _shownPosition.setY(5 - _shownHeight);
        }
        if (_shownPosition.x() >= _canvasRegion->_W) {
            _shownPosition.setX(_canvasRegion->_W - 5);
        }
        if (_shownPosition.y() >= _canvasRegion->_H) {
            _shownPosition.setY(_canvasRegion->_H - 5);
        }

        setGeometry(_shownPosition.x(), _shownPosition.y(), _shownWidth, _shownHeight);

        // 更新位置输入框数值
        _canvasRegion->_canvasWindow->_xFillin->setValue(_shownPosition.x());
        _canvasRegion->_canvasWindow->_yFillin->setValue(_shownPosition.y());
        //update();
    }
}

// 使用InputNode的滑块修改坐标
void ImageRegion::inputSetX(int val)
{
    //qDebug()<<"inputSetX Called";
    _shownPosition.setX(val * _ratioShow);
    setGeometry(_shownPosition.x(), _shownPosition.y(), _shownWidth, _shownHeight);
    // 更新输入框的数值
    _canvasRegion->_canvasWindow->_xFillin->setValue(val * _ratioShow);
}

void ImageRegion::inputSetY(int val)
{
    //qDebug()<<"inputSetY Called";
    _shownPosition.setY(val * _ratioShow);
    setGeometry(_shownPosition.x(), _shownPosition.y(), _shownWidth, _shownHeight);
    _canvasRegion->_canvasWindow->_yFillin->setValue(val * _ratioShow);
}

// CropWindow
CropWindow::CropWindow(Node *superior, cv::Mat image)
    : QWidget(nullptr), ui(new Ui::CropWindow)
{
    ui->setupUi(this);
    setWindowTitle(QChinese("裁剪"));
    setFixedSize(800, 550);
    _superior = (InputNode*)superior;

    _cropRegion = new CropRegion(this);
    _cropRegion->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    //QImage qImage(image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
    _cropRegion->loadMat(image);
    ui->cropRegionLayout->addWidget(_cropRegion);

    QPushButton *aiCropButton = ui->aiCropButton;
    connect(aiCropButton, &QPushButton::clicked, [this]() {
        _cropRegion->aiCrop();
    });

    QPushButton *acceptButton = ui->acceptButton;
    connect(acceptButton, &QPushButton::clicked, [this]() {
        this->_cropRegion->apply();
    });

    QPushButton *cancelButton = ui->cancelButton;
    connect(cancelButton, &QPushButton::clicked, [this]() {
        this->_cropRegion->cancel();
    });

    show();
}

CropWindow::~CropWindow()
{
    qDebug()<<"CropWindow::~CropWindow()";
}

QSize CropWindow::sizeHint() const
{
    QSize s;
    s.setWidth(1000);
    s.setHeight(750);
    return s;
}

cv::Mat remove_background(const QString& apiKey, const cv::Mat& inputImage)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager();

    QUrl url("https://api.remove.bg/v1.0/removebg");
    QNetworkRequest request(url);
    request.setRawHeader("X-Api-Key", apiKey.toUtf8());
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));

    QByteArray ba;
    ba.append("size=auto&");
    ba.append("image_file_b64=");

    // Convert OpenCV Mat to QByteArray (Base64 encoded)
    std::vector<uchar> buf;
    cv::imencode(".png", inputImage, buf);  // Assuming PNG format for simplicity
    QByteArray imageData(reinterpret_cast<const char*>(buf.data()), buf.size());
    ba.append(imageData.toBase64(QByteArray::Base64UrlEncoding));

    QNetworkReply* reply = manager->post(request, ba);

    // Wait for the API response (synchronous approach, not recommended for GUI applications)
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    cv::Mat resultImage;

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        std::vector<uchar> imageData(response.begin(), response.end());
        resultImage = cv::imdecode(cv::Mat(imageData), cv::IMREAD_UNCHANGED);
        qDebug() << "Background removed successfully";
    } else {
        qDebug() << "Error:" << reply->errorString();
        // 弹出警告框
        QMessageBox::warning(nullptr, QChinese("错误"), QChinese("移除背景失败，请检查API Key是否正确与网络连接"));
    }

    reply->deleteLater();
    delete manager;
    return resultImage;
}

// CropRegion
CropRegion::CropRegion(CropWindow *parent) : QLabel((QWidget*)parent)
{
    _cropWindow = parent;
    W = 580;
    H = 410;
    setMouseTracking(true);
    setGeometry(0, 0, W, H);
    setStyleSheet("background: white;");
    //qDebug() << "Geometry: " << geometry();
    //setScaledContents(true);
    _mousePressed = false;
    //show();
}

CropRegion::~CropRegion()
{
    qDebug()<<"CropRegion::~CropRegion()";
}

void CropRegion::loadMat(cv::Mat src)
{
    //QImage qt_img = QImage(src.data, src.cols, src.rows, src.step, QImage::Format_RGB888).rgbSwapped();
    QImage qt_img = cvMat2QImage(src);
    _originalImage = src;
    _currentCoreImage = src;
    _shownImage = QPixmap::fromImage(qt_img);
    _shownImage = _shownImage.scaled(geometry().width(), geometry().height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    int w = _shownImage.width(), h = _shownImage.height();
    setPixmap(_shownImage);
    setAlignment(Qt::AlignCenter);
    _upBorder = (geometry().height() - h) / 2;
    _downBorder = (geometry().height() + h) / 2;
    _leftBorder = (geometry().width() - w) / 2;
    _rightBorder = (geometry().width() + w) / 2;
    //qDebug() << "pixmap " << _upBorder << ' ' << _downBorder << ' ' << _leftBorder << ' ' << _rightBorder;
}

void CropRegion::paintEvent(QPaintEvent *e)
{
    QLabel::paintEvent(e);

    QPainter painter(this);
    //设置画笔，颜色、宽度
    QPen pen;
    pen.setColor(Qt::red);
    pen.setWidth(2);
    painter.setPen(pen);
    //painter.drawPixmap(0, 0, this->width(), this->height(), qt_pixmap);
    //painter.setViewport(0, 0, this->width(), this->height());
    if (_mousePressed || _mouseCropChecking)
    {
        _paintedRect = QRect(_paintedRectStart, _paintedRectEnd);
        painter.drawRect(_paintedRect);
    }
}

void CropRegion::mousePressEvent(QMouseEvent *e)
{
    if (e->buttons()&Qt::LeftButton)
    {
        if (_cropMode == 2) {
            QLabel* lb = new QLabel("正在AI抠图中，请在完成或取消后再手动截取图像。");
            lb->show();
            return;
        }
        _mouseCropChecking = false;
        _cropMode = 1;
        QPoint p = e->pos();
        if (p.y() < _upBorder) {
            p.setY(_upBorder);
        }
        if (p.y() > _downBorder) {
            p.setY(_downBorder);
        }
        if (p.x() < _leftBorder) {
            p.setX(_leftBorder);
        }
        if (p.x() > _rightBorder) {
            p.setX(_rightBorder);
        }
        _paintedRectStart = p;
        _paintedRectEnd = p;
        //qDebug() << "start_point: " << _paintedRectStart;
        _mousePressed = true;
    }

    update();
    QLabel::mousePressEvent(e);
}

void CropRegion::mouseReleaseEvent(QMouseEvent *e)
{
    if (_mousePressed) {
        _mousePressed = false;
        if (e->button() == Qt::LeftButton)
        {
            qDebug() << "end_point: " << _paintedRectEnd;
            _mouseCropChecking = true;
        }
    }
}

void CropRegion::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons()&Qt::LeftButton)
    {
        if (_mousePressed)
        {
            QPoint p = e->pos();
            if (p.y() < _upBorder) {
                p.setY(_upBorder);
            }
            if (p.y() > _downBorder) {
                p.setY(_downBorder);
            }
            if (p.x() < _leftBorder) {
                p.setX(_leftBorder);
            }
            if (p.x() > _rightBorder) {
                p.setX(_rightBorder);
            }
            _paintedRectEnd = p;
            update();
        }
    }
}

void CropRegion::aiCrop()
{
    if(1)
    {
        QMessageBox::information(nullptr, QChinese("提示"), QChinese("该功能在 release 版本中暂未开放，如需使用请在 inputnode.cpp 中替换自己的 API-KEY ，并注释掉这段代码。"));
        return;
    }

    if (_cropMode == 1) {
        //QLabel* lb = new QLabel("正在手动截取图像中，请在完成或取消后再使用AI抠图。");
        //lb->show();
        QMessageBox::warning(nullptr, QChinese("Error"), QChinese("正在手动截取图像中，请在完成或取消后再使用AI抠图。"));
        return;
    }
    else {
        _cropMode = 2;
        // AI 抠图
        QString apiKey = "请替换成你的API-KEY"; // "WCqqLXvfKEHBJchaT5214Es3"; // API Key
        _tempAiCroppedImage = remove_background(apiKey, _currentCoreImage);
        if (_tempAiCroppedImage.empty()) {
            //QLabel* lb = new QLabel("AI抠图失败，请检查网络连接或API Key。");
            //lb->show();
            return;
        }
        cv::resize(_tempAiCroppedImage, _tempAiCroppedImage, cv::Size(_currentCoreImage.cols, _currentCoreImage.rows));
        QImage qt_img = cvMat2QImage(_tempAiCroppedImage);
        _shownImage = QPixmap::fromImage(qt_img);
        _shownImage = _shownImage.scaled(geometry().width(), geometry().height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        setPixmap(_shownImage);
    }

}

void CropRegion::apply()    // 执行裁剪
{
    if (_cropMode == 1) {
        _mouseCropChecking = false;     // 结束确认阶段

        // 计算裁剪区域
        int minX, minY;
        if (_paintedRectStart.x() > _paintedRectEnd.x()) {
            minX = _paintedRectEnd.x();
        }
        else {
            minX = _paintedRectStart.x();
        }
        if (_paintedRectStart.y() > _paintedRectEnd.y()) {
            minY = _paintedRectEnd.y();
        }
        else {
            minY = _paintedRectStart.y();
        }
        int dX = abs(_paintedRectEnd.x() - _paintedRectStart.x());
        int dY = abs(_paintedRectEnd.y() - _paintedRectStart.y());
        if (dX < 5 || dY < 5) {
            update();
            return;
        }

        _croppedRect.x = (int)(((double)minX - (double)_leftBorder) * (double)_currentCoreImage.cols / (double)_shownImage.width());
        _croppedRect.y = (int)(((double)minY - (double)_upBorder) * (double)_currentCoreImage.rows / (double)_shownImage.height());
        _croppedRect.width = (int)((double)dX * (double)_currentCoreImage.cols / (double)_shownImage.width());
        _croppedRect.height = (int)((double)dY * (double)_currentCoreImage.rows / (double)_shownImage.height());
        if (_croppedRect.x + _croppedRect.width > _currentCoreImage.cols) {
            _croppedRect.width = _currentCoreImage.cols - _croppedRect.x;
        }
        if (_croppedRect.y + _croppedRect.height > _currentCoreImage.rows) {
            _croppedRect.height = _currentCoreImage.rows - _croppedRect.y;
        }

        // 裁剪
        _currentCoreImage = _currentCoreImage(_croppedRect).clone();

        // 在 QLabel 中显示图像
        _cropWindow->_preProcWin->_canvasWindow->_canvasRegion->_imageRegion->reloadImage(_currentCoreImage);
        //_cropWindow->_preProcWin->_canvasWindow->_canvasRegion->update();

        QImage qt_img = cvMat2QImage(_currentCoreImage);
        _shownImage = QPixmap::fromImage(qt_img);
        _shownImage = _shownImage.scaled(geometry().width(), geometry().height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        int w = _shownImage.width(), h = _shownImage.height();
        setPixmap(_shownImage);
        _upBorder = (geometry().height() - h) / 2;
        _downBorder = (geometry().height() + h) / 2;
        _leftBorder = (geometry().width() - w) / 2;
        _rightBorder = (geometry().width() + w) / 2;
    }

    else if (_cropMode == 2) {
        _currentCoreImage = _tempAiCroppedImage.clone();
        _cropWindow->_preProcWin->_canvasWindow->_canvasRegion->_imageRegion->reloadImage(_currentCoreImage);
        _tempAiCroppedImage = cv::Mat().clone();
    }
    _cropMode = 0;
}

void CropRegion::cancel()
{
    if (_cropMode == 1) {
        _mouseCropChecking = false;
        _cropMode = 0;
        update();
    }
    else if (_cropMode == 2) {
        QImage qt_img = cvMat2QImage(_currentCoreImage);
        _shownImage = QPixmap::fromImage(qt_img);
        _shownImage = _shownImage.scaled(geometry().width(), geometry().height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        setPixmap(_shownImage);
        _cropMode = 0;
    }
}

QSize CropRegion::sizeHint() const
{
    QSize s;
    s.setWidth(W);
    s.setHeight(H);
    return s;
}
