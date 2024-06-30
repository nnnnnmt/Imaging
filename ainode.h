#ifndef AINODE_H
#define AINODE_H

#include "functionnode.h"
#include "node.h"
#include "utils.h"

#define QChinese(str) QString(str)

QT_BEGIN_NAMESPACE
namespace Ui {
class AIi2iNodeWidget;
class AIw2iNodeWidget;
}
QT_END_NAMESPACE

class AIi2iNodeWidget : public QWidget
{
    Q_OBJECT
public:
    AIi2iNodeWidget(QWidget *parent = nullptr);
    ~AIi2iNodeWidget();

public:
    Ui::AIi2iNodeWidget *ui;
};

class AIw2iNodeWidget : public QWidget
{
    Q_OBJECT
public:
    AIw2iNodeWidget(QWidget *parent = nullptr);
    ~AIw2iNodeWidget();

public:
    Ui::AIw2iNodeWidget *ui;
};

class AIword2imgNode : public FunctionNode
{
    Q_OBJECT
public:
    AIword2imgNode(MainScene *scene, QGraphicsItem *parent=nullptr);
    ~AIword2imgNode();
    virtual void initProxyWidget() override;
    void execute() override;
    void sendAPIRequest();
    int type() const override {return 4111;};
    bool _isProcessing=false; // 是否正在处理
    QLabel *_label;
    QLabel *_promptLabel;
    QCheckBox *_checkBox;
    QPushButton *_executeButton;
    QPushButton *_inputPromptButton;
    AIw2iNodeWidget *_widget;
    QString _apiKey = "请替换成你的API-KEY"; // c7289d9d-471c-369a-0974-0a2c8056202d
    QString _prompt = QChinese("请输入Prompt");
    void downloadPhoto(const QString &url, const QString &saveFilePath);
    void interpretJson(const QByteArray &jsonData, const QString &saveFilePath);
    void word_to_image(const QString &prompt, const QString &apiKey, const QString &saveFilePath);
signals:
    void requestFinished();
};

class AIimg2imgNode : public FunctionNode
{
    Q_OBJECT
public:
    AIimg2imgNode(MainScene *scene, QGraphicsItem *parent=nullptr);
    ~AIimg2imgNode();
    virtual void initProxyWidget() override;
    void sendAPIRequest();
    //void execute() override;
    int type() const override {return 4112;};

    bool _isProcessing=false; // 是否正在处理
    QLabel *_label;
    QCheckBox *_checkBox;
    QLabel *_promptLabel;
    QPushButton *_executeButton;
    QPushButton *_viewOutputsButton;
    QPushButton *_inputPromptButton;
    AIi2iNodeWidget *_widget;
    QString _apiKey = "请替换成你的API-KEY";//"17e4923469114dc88b1cfaae6563acba";
    QList<cv::Mat> _generatedImages;
    int _generatedImagesIndex = 0;
    QString _prompt = QChinese("请输入Prompt");

    void downloadPhoto(const QString &url, const QString &saveFilePath);
    QList<QString> interpretJson_all(const QString head, const QByteArray &jsonData);
    void downloadimg_img2img(const QString& apiKey,const QString& content,const QString& outputFilePath);
    void image_to_image(const QString& apiKey,const QString& inputFilePath, const QString& outputFilePath,const QString &prompt);
signals:
    void requestFinished();
};

#endif // AINODE_H
