#include "ainode.h"
#include "functionnode.h"
#include "node.h"
#include "mainscene.h"
#include <QObject>
#include "ui_aii2inodewidget.h"
#include "ui_aiw2inodewidget.h"

AIi2iNodeWidget::AIi2iNodeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AIi2iNodeWidget)
{
    ui->setupUi(this);
}

AIi2iNodeWidget::~AIi2iNodeWidget()
{
    delete ui;
}

AIw2iNodeWidget::AIw2iNodeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AIw2iNodeWidget)
{
    ui->setupUi(this);
}

AIw2iNodeWidget::~AIw2iNodeWidget()
{
    delete ui;
}


void AIword2imgNode::downloadPhoto(const QString &url, const QString &saveFilePath)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);
    QObject::connect(reply, &QNetworkReply::finished, [reply, url, saveFilePath,this]() {
        if (reply->error() == QNetworkReply::NoError)
        {
            QByteArray response = reply->readAll();
            QFile file(saveFilePath);
            file.open(QIODevice::WriteOnly);
            file.write(response);
            file.close();
            qDebug() << "Downloaded:" << url;
            // 弹出信息框
            QMessageBox::information(nullptr, "下载完成", "从" + url + "下载完成");
        }
        else
        {
            qDebug() << "Error:" << reply->errorString();
            // 弹出警告框
            QMessageBox::warning(nullptr, "下载失败", "从" + url + "下载失败");
        }
        // 发送信号 finished
        emit requestFinished();
        reply->deleteLater();
    });
}

void AIword2imgNode::interpretJson(const QByteArray &jsonData, const QString &saveFilePath)
{
    QString rawData = jsonData;
    QStringList jsonList = rawData.split('\n', Qt::SkipEmptyParts);

    QRegularExpression re("\"url\":\"(.*?)\"");
    for (const QString &jsonStr : jsonList)
    {
        QRegularExpressionMatch match = re.match(jsonStr);
        if (match.hasMatch()) {
            QString content = match.captured(1);
            qDebug() << "Content:" << content;
            downloadPhoto(content, saveFilePath);
        }
    }
}

void AIword2imgNode::word_to_image(const QString &prompt, const QString &apiKey, const QString &saveFilePath)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager();

    QUrl url("https://api.ttapi.io/openai/v1/images/generations");
    QNetworkRequest request(url);
    request.setRawHeader("TT-API-KEY", apiKey.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject requestBody;
    requestBody["size"] = "1792x1024";
    requestBody["prompt"] = prompt;

    QJsonDocument jsonDoc(requestBody);
    QByteArray jsonData = jsonDoc.toJson();

    QNetworkReply *reply = manager->post(request, jsonData);

    QObject::connect(reply, &QNetworkReply::finished, [reply, saveFilePath,this]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            qDebug() << "Response:" << response;
            interpretJson(response, saveFilePath);
        } else {
            qDebug() << "Error:" << reply->errorString();
            // 弹出警告框
            QMessageBox::warning(nullptr, "Error", "请求失败，请检查你的API-KEY",QMessageBox::Ok);
            _isProcessing = false;
        }
        reply->deleteLater();
    });
}

AIword2imgNode::AIword2imgNode(MainScene *scene, QGraphicsItem *parent)
    : FunctionNode(scene, parent)
{
    _functionNodeType = FunctionNodeType::AIFunctionNode;
    _nodeType = NodeType::ai_word2imgNode;
    _name = "Word to Image";
    qDebug() << "AIword2imgNode::AIword2imgNode";
    _outputTerminals.append(new Terminal(_scene,TerminalType::outputTerminal,this));
    _canvasHeight = _scene->_canvasHeight;
    _canvasWidth = _scene->_canvasWidth;
    initializeBoundingRect();
    initializeTerminal();
    initProxyWidget();
}

AIword2imgNode::~AIword2imgNode()
{
    qDebug() << "~AIword2imgNode";
}

void AIword2imgNode::initProxyWidget()
{
    _widget = new AIw2iNodeWidget();
    // 将Prompt显示
    _promptLabel = _widget->ui->promptLabel;
    _promptLabel->setText(QChinese("当前Prompt：")+_prompt);

    connect(_widget->ui->closeButton, &QPushButton::clicked, [this]() {
        this->_scene->_deleteItem((QGraphicsItem*)this);
    });

    _inputPromptButton = _widget->ui->editButton;
    // 如果按下则弹出对话框输入prompt
    connect(_inputPromptButton, &QPushButton::clicked, [this]() {
        bool ok;
        QString text = QInputDialog::getText(nullptr, QChinese("更改Prompt"), QChinese("请输入Prompt:"), QLineEdit::Normal, _prompt, &ok);
        if (ok && !text.isEmpty())
        {
            _prompt = text;
        }
        _promptLabel->setText(QChinese("当前Prompt：")+_prompt);
    });

    _executeButton = _widget->ui->beginButton;
    // 如果按下则发送请求
    connect(_executeButton, &QPushButton::clicked, [this]() {
        if (_isProcessing==true)
        {
            QMessageBox::warning(nullptr, "Warning", "已有图片生成任务正在运行，请等候该任务完成", QMessageBox::Ok);
            return;
        }
        sendAPIRequest();
    });

    // 创建一个checkbox 是否显示preview
    _checkBox = _widget->ui->viewCheckBox;
    // 默认勾选
    _checkBox->setCheckState(Qt::Checked);
    // 显示outputImage
    _label = _widget->ui->showLabel;
    if (_outputImage.empty()&&_isProcessing==false)
    {
        QImage img(":/window/null.png");
        img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
        _label->setPixmap(QPixmap::fromImage(img));
    }
    else if(_isProcessing==true)
    {
        // 显示loading.gif
        QMovie *movie = new QMovie(":/window/processing1.gif");
        //缩小到50*50
        movie->setScaledSize(QSize(200, 64));
        _label->setMovie(movie);
        movie->start();
    }
    else
    {
        QImage img = cvMat2QImage(_outputImage);
        img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
        _label->setPixmap(QPixmap::fromImage(img));
    }
    connect(_checkBox, &QCheckBox::stateChanged, [this](int state) {
        this->_label->setVisible(state == Qt::Checked);
    });
    // 用checkbox控制是否显示
    // 创建一个proxywidget
    _nodeSettings = _scene->addWidget(_widget);
    _nodeSettings->setParentItem(this);
    _nodeSettings->setPos(0, 0);
}

void AIword2imgNode::execute()
{
    ;
}

void AIword2imgNode::sendAPIRequest()
{
    _isProcessing = true;
    //qDebug() << "AIword2imgNode::sendAPIRequest";
    word_to_image(_prompt, _apiKey, "./pics/myoutput.jpg");
    //重设label的gif
    QMovie *movie = new QMovie(":/window/processing1.gif");
    movie->setScaledSize(QSize(200, 64));
    _label->setMovie(movie);
    movie->start();

    //如果requestFinished则显示图片
    connect(this, &AIword2imgNode::requestFinished, [this]() {
        _isProcessing = false;
        cv::Mat img = cv::imread("./pics/myoutput.jpg");
        cv::resize(img, img, cv::Size(_canvasWidth, _canvasHeight));
        _outputImage = img;
        QImage qimg = QImage((const unsigned char*)(img.data), img.cols, img.rows, QImage::Format_RGB888).rgbSwapped();
        qimg = qimg.width() > qimg.height() ? qimg.scaledToWidth(200) : qimg.scaledToHeight(200);
        _label->setPixmap(QPixmap::fromImage(qimg));
    });
}




AIimg2imgNode::AIimg2imgNode(MainScene *scene, QGraphicsItem *parent)
    : FunctionNode(scene, parent)
{
    _functionNodeType = FunctionNodeType::AIFunctionNode;
    _nodeType = NodeType::ai_img2imgNode;
    _name = "Image to Image";
    qDebug() << "AIimg2imgNode::AIimg2imgNode";
    _inputTerminals.append(new Terminal(_scene,TerminalType::inputTerminal,this));
    _outputTerminals.append(new Terminal(_scene,TerminalType::outputTerminal,this));
    _canvasHeight = _scene->_canvasHeight;
    _canvasWidth = _scene->_canvasWidth;
    initializeBoundingRect();
    initializeTerminal();
    initProxyWidget();
}

AIimg2imgNode::~AIimg2imgNode()
{
    qDebug() << "~AIimg2imgNode";
}

void AIimg2imgNode::downloadPhoto(const QString &url, const QString &saveFilePath)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);
    QObject::connect(reply, &QNetworkReply::finished, [reply, url, saveFilePath,this]() {
        if (reply->error() == QNetworkReply::NoError)
        {
            QByteArray response = reply->readAll();
            QFile file(saveFilePath);
            file.open(QIODevice::WriteOnly);
            file.write(response);
            file.close();
            qDebug() << "Downloaded:" << url;
            // 弹出信息框
            QMessageBox::information(nullptr, "下载完成", "从" + url + "下载完成", QMessageBox::Ok);
            emit requestFinished();
        }
        else
        {
            qDebug() << "Error:" << reply->errorString();
            // 弹出警告框
            QMessageBox::warning(nullptr, "Error", "下载失败，请检查URL", QMessageBox::Ok);
        }
        reply->deleteLater();
    });
}

QList<QString> AIimg2imgNode::interpretJson_all(const QString head, const QByteArray &jsonData)
{
    QString rawData = jsonData;
    QStringList jsonList = rawData.split(',', Qt::SkipEmptyParts);

    QRegularExpression re("\""+head+"\":\"(.*?)\"");
    QList<QString> contentList;

    for (const QString &jsonStr : jsonList)
    {
        QRegularExpressionMatch match = re.match(jsonStr);
        if (match.hasMatch()) {
            QString content = match.captured(1);
            qDebug() << "content:" << content;
            contentList.append(content);
        }
    }
    return contentList;
}
void AIimg2imgNode::downloadimg_img2img(const QString& apiKey,const QString& content,const QString& outputFilePath)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager();
    QUrl url("https://ai.huashi6.com/aiapi/v1/mj/task/progress");
    QNetworkRequest request(url);
    request.setRawHeader("Auth-Token", apiKey.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject requestBody;
    requestBody["taskId"] = content;
    requestBody["waitUtilEnd"] = true;
    QJsonDocument jsonDoc(requestBody);
    QByteArray jsonData = jsonDoc.toJson();

    QNetworkReply* reply = manager->post(request, jsonData);

    QObject::connect(reply, &QNetworkReply::finished, [reply,outputFilePath,this]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            qDebug() << "Response received img2img";
            qDebug() << response;
            auto urls = interpretJson_all("imageUrl",response);
            for(int i=0;i<urls.size();i++)
            {
                qDebug() << "Content:" << urls[i];
                qDebug() << outputFilePath+QString::number(i)+".jpg";
                downloadPhoto(urls[i], outputFilePath+QString::number(i)+".jpg");
            }
            // 确保前面的图片下载完成
            emit requestFinished();
        } else {
            qDebug() << "Error:" << reply->errorString();
            // 弹出警告框
            QMessageBox::warning(nullptr, "Error", "下载图片失败"+reply->errorString(), QMessageBox::Ok);
        }
        reply->deleteLater();
    });
}

void AIimg2imgNode::image_to_image(const QString& apiKey,const QString& inputFilePath, const QString& outputFilePath,const QString &prompt)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager();

    QUrl url("https://ai.huashi6.com/aiapi/v1/mj/draw");
    QNetworkRequest request(url);
    request.setRawHeader("Auth-Token", apiKey.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // 打开inputFilePath 读取文件转为Base64
    QFile* file = new QFile(inputFilePath);
    if (!file->open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open input file";
        // 弹出警告框
        QMessageBox::warning(nullptr, "Error", "无法打开输入文件", QMessageBox::Ok);
        delete file;
        return;
    }
    QString ba;
    ba.append(file->readAll().toBase64(QByteArray::Base64UrlEncoding));
    file->close();
    delete file;

    qDebug()<<"image_to_image";
    QJsonObject requestBody;
    requestBody["imageUrl"] = ba;
    requestBody["prompt"] = prompt;
    //requestBody["type"] = "fast";

    QJsonDocument jsonDoc(requestBody);
    QByteArray jsonData = jsonDoc.toJson();

    QNetworkReply* reply = manager->post(request, jsonData);

    QObject::connect(reply, &QNetworkReply::finished, [reply, outputFilePath,apiKey,this]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            qDebug() << "Response received painting signs";
            qDebug() << response;
            auto paintingSigns = interpretJson_all("paintingSign",response);
            if(paintingSigns.size()==0)
            {
                // 弹窗警告 当前已有任务正在进行
                QMessageBox::warning(nullptr, "Warning", "当前已有任务正在进行", QMessageBox::Ok);
                qDebug() << "Error: paintingSigns is empty";
                emit requestFinished();
                return;
            }
            QString paintingSign = paintingSigns[0];
            // 弹出信息框
            QMessageBox::information(nullptr, "任务已提交", "任务已提交，任务序号"+ paintingSign, QMessageBox::Ok);
            downloadimg_img2img(apiKey,paintingSign,outputFilePath);
        } else {
            qDebug() << "Error:" << reply->errorString();
        }
        reply->deleteLater();
    });
}


class ChooseGeneratedImageWidget : public QWidget{
    Q_OBJECT
public:
    ChooseGeneratedImageWidget(QWidget *parent = nullptr,const QList<cv::Mat> &images = {});
public slots:
    void handleButton();
signals:
    void imageSelected(int index);
public:
    QLabel *labels[4];
    QPushButton *buttons[4];
    QGridLayout *gridLayout;
    int selectedIndex = -1;
};

ChooseGeneratedImageWidget::ChooseGeneratedImageWidget(QWidget *parent,const QList<cv::Mat> &images) : QWidget(parent)
{
    gridLayout = new QGridLayout(this);
    for (int i = 0; i < 4; ++i) {
        labels[i] = new QLabel(this);
        QImage img = QImage((const unsigned char*)(images[i].data), images[i].cols, images[i].rows, QImage::Format_RGB888).rgbSwapped();
        labels[i]->setPixmap(QPixmap::fromImage(img).scaled(300,300, Qt::KeepAspectRatio));
        buttons[i] = new QPushButton("Select", this);
        buttons[i]->setProperty("index", i);
        connect(buttons[i], &QPushButton::clicked, this, &ChooseGeneratedImageWidget::handleButton);

        int row = i / 2;
        int col = i % 2;

        gridLayout->addWidget(labels[i], row, col * 2);
        gridLayout->addWidget(buttons[i], row, col * 2 + 1);
    }
}

void ChooseGeneratedImageWidget::handleButton()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    selectedIndex = button->property("index").toInt();
    emit imageSelected(selectedIndex);
}

class ChooseImageDialog : public QDialog {
    Q_OBJECT
public:
    ChooseImageDialog(QWidget *parent = nullptr, const QList<cv::Mat> &images = {});
    int getSelectedIndex() const { return selectedIndex; }
private slots:
    void onImageSelected(int index);
private:
    ChooseGeneratedImageWidget *widget;
    int selectedIndex = -1;
};

ChooseImageDialog::ChooseImageDialog(QWidget *parent, const QList<cv::Mat> &images)
    : QDialog(parent), selectedIndex(-1)
{
    widget = new ChooseGeneratedImageWidget(this, images);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(widget);
    setLayout(layout);
    connect(widget, &ChooseGeneratedImageWidget::imageSelected, this, &ChooseImageDialog::onImageSelected);
}

void ChooseImageDialog::onImageSelected(int index)
{
    selectedIndex = index;
    accept(); // Close the dialog with QDialog::Accepted status
}

void AIimg2imgNode::initProxyWidget()
{
    _widget = new AIi2iNodeWidget();
    // 将Prompt显示
    _promptLabel = _widget->ui->promptLabel;
    _promptLabel->setText(QChinese("当前Prompt：")+_prompt);

    connect(_widget->ui->closeButton, &QPushButton::clicked, [this]() {
        this->_scene->_deleteItem((QGraphicsItem*)this);
    });

    _inputPromptButton = _widget->ui->editButton;
    // 如果按下则弹出对话框输入prompt
    connect(_inputPromptButton, &QPushButton::clicked, [this]() {
        bool ok;
        QString text = QInputDialog::getText(nullptr, QChinese("更改Prompt"), QChinese("请输入Prompt:"), QLineEdit::Normal, _prompt, &ok);
        if (ok && !text.isEmpty())
        {
            _prompt = text;
        }
        _promptLabel->setText(QChinese("当前Prompt：")+_prompt);
    });

    _executeButton = _widget->ui->beginButton;
    // 如果按下且_inputImages不为空则发送请求
    connect(_executeButton, &QPushButton::clicked, [this]() {
        if(_inputImages.empty())
        {
            // 弹出警告框
            QMessageBox::warning(nullptr, "Error", "请首先选择一张图片", QMessageBox::Ok);
            return;
        }
        if (_isProcessing)
        {
            // 弹出警告框
            QMessageBox::warning(nullptr, "Error", "当前已有任务正在进行", QMessageBox::Ok);
            return;
        }
        if (!_inputImages.empty())
        {
            qDebug() << "AIimg2imgNode::sendAPIRequest";
            _outputImage = cv::Mat();
            _generatedImagesIndex = -1;
            sendAPIRequest();
        }
    });

    _viewOutputsButton = _widget->ui->viewButton;
    connect(_viewOutputsButton, &QPushButton::clicked, [this]()
    {
        if (_generatedImages.empty())
        {
            return;
        }
        ChooseImageDialog dialog(nullptr, _generatedImages);
        if (dialog.exec() == QDialog::Accepted)
        {
            int selectedIndex = dialog.getSelectedIndex();
            _generatedImagesIndex = selectedIndex;
            qDebug() << "User selected image index:" << selectedIndex;
            _outputImage = _generatedImages[_generatedImagesIndex];
            cv::resize(_outputImage, _outputImage, cv::Size(_canvasWidth, _canvasHeight));
            QImage img = QImage((const unsigned char*)(_outputImage.data), _outputImage.cols, _outputImage.rows, QImage::Format_RGB888).rgbSwapped();
            img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
            _label->setPixmap(QPixmap::fromImage(img));
        }
        else
        {
            qDebug() << "Dialog was canceled";
        }

    });
    // 创建一个checkbox 是否显示preview
    _checkBox = _widget->ui->viewCheckBox;
    // 默认勾选
    _checkBox->setCheckState(Qt::Checked);
    // 显示outputImage
    _label = _widget->ui->showLabel;
    if (_outputImage.empty()&&_isProcessing==false)
    {

        QImage img(":/window/null.png");
        img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
        _label->setPixmap(QPixmap::fromImage(img));
    }
    else if(_isProcessing==true)
    {
        // 显示loading.gif
        QMovie *movie = new QMovie(":/window/processing1.gif");
        movie->setScaledSize(QSize(200, 64));
        _label->setMovie(movie);
        movie->start();
    }
    else
    {
        QImage img = QImage((const unsigned char*)(_outputImage.data), _outputImage.cols, _outputImage.rows, QImage::Format_RGB888).rgbSwapped();
        img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
        _label->setPixmap(QPixmap::fromImage(img));
    }

    connect(_checkBox, &QCheckBox::stateChanged, [this](int state) {
        this->_label->setVisible(state == Qt::Checked);
    });
    // 用checkbox控制是否显示

    // 创建一个proxywidget
    _nodeSettings = _scene->addWidget(_widget);
    _nodeSettings->setParentItem(this);
    _nodeSettings->setPos(0, 0);
}

void AIimg2imgNode::sendAPIRequest()
{
    _isProcessing = true;
    //qDebug() << "AIword2imgNode::sendAPIRequest";
    // 将_inputImages[0]保存到/tmp/tmpimagetogenerate.jpg
    cv::imwrite("./tmp/tmpimagetogenerate.jpg", _inputImages[0]);
    QString outputDIR = "./tmp/outputtt";
    image_to_image(_apiKey, "./tmp/tmpimagetogenerate.jpg", outputDIR,_prompt);
    //重设label的gif
    QMovie *movie = new QMovie(":/window/processing1.gif");
    movie->setScaledSize(QSize(200, 64));
    _label->setMovie(movie);
    movie->start();

    //如果requestFinished则显示图片
    connect(this, &AIimg2imgNode::requestFinished, [this,outputDIR]() {
        qDebug() << "AIimg2imgNode::requestFinished";
        _isProcessing = false;
        // 读取所有/tmp/generated*.jpg 读入_generatedImages
        _generatedImages.clear();
        for (int i = 0; i < 4; ++i)
        {
            // 先判断文件是否存在
            std::string filename = outputDIR.toStdString()+std::to_string(i)+".jpg";
            QFile file(QString::fromStdString(filename));
            if (!file.exists())
            {
                break;
            }
            cv::Mat img = cv::imread(outputDIR.toStdString()+std::to_string(i)+".jpg");
            _generatedImages.push_back(img);
        }
        qDebug() << "Generated images size:" << _generatedImages.size();
        if (_generatedImages.empty())
        {
            _label->clear();

            QImage img(":/window/null.png");
            img = img.width() > img.height() ? img.scaledToWidth(200) : img.scaledToHeight(200);
            _label->setPixmap(QPixmap::fromImage(img));
            return;
        }
        _label->clear();
        _outputImage = _generatedImages[0];
        QImage qimg = QImage((const unsigned char*)(_outputImage.data), _outputImage.cols, _outputImage.rows, QImage::Format_RGB888).rgbSwapped();
        qimg = qimg.width() > qimg.height() ? qimg.scaledToWidth(200) : qimg.scaledToHeight(200);
        _label->setPixmap(QPixmap::fromImage(qimg));
    });
}

#include "ainode.moc"
