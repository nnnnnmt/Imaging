#include "mainwindow.h"
#include "nodechoice.h"
#include "editablelabel.h"
#include "mainscene.h"
#include "mainview.h"
#include "imagewidget.h"
#include "node.h"
#include "gradientheader.h"
#include <QDebug>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent)
{
    setStyleSheet("QMainWindow::separator { width: 0px; height: 0px; margin: 0px; padding: 0px; }");
    setBasicInformation();
    createMenu();
    createNodeChoiceDock();
    createOutcomeDock();
    //createInformationDock();
    createInformationBar();
    createMainView();
    createTimer();

    _Dock_Head = new QDockWidget(this);
    _Dock_Head->setWidget(new GradientHeader(this));
    _Dock_Head->setTitleBarWidget(new QWidget());
    _Dock_Head->setFeatures(QDockWidget::NoDockWidgetFeatures);
    _Dock_Head->setFixedHeight(50);
    addDockWidget(Qt::TopDockWidgetArea, _Dock_Head);

    _actWidget = new QPushButton(this);
    _actWidget->raise();
    _actWidget->setGeometry(680,640,80,80);
    _actWidget->setStyleSheet("background: none;"
                              "border-image: url(:/window/do.png);"
                              "border: none;");
    connect(_actWidget, &QPushButton::clicked, this, &MainWindow::refreshOutcome);
}

void MainWindow::newProject()
{
    delete _MainScene;
    delete _MainView;
    _CurrentProjectName = QChinese("未命名");
    _canvasWidth = 800;
    _canvasHeight = 600;
    createInformationBar();
    createMainView();
    if (_actWidget != nullptr) _actWidget->raise();
}

void MainWindow::openProject()
{
    // 从ima文件中读取Node的type和位置、以及Node的OutputTerminal、inputTerminal的数量
    // 从ima文件中读取连线的起始Terminal和终止Terminal的位置

    QString fileName = QFileDialog::getOpenFileName(this, QChinese("打开文件"), _CurrentProjectName, "Imaging Project Files (*.ima)");
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this, QChinese("无法打开文件"), file.errorString());
        return;
    }
    newProject();
    // 先删掉场景中的outputNode
    for (int i = 0; i < _MainScene->_nodes->size(); i++)
    {
        Node* node = _MainScene->_nodes->at(i);
        if (node->_nodeType == NodeType::out_outputNode)
        {
            //qDebug() << "delete outputNode";
            _MainScene->_nodes->removeAt(i);
            _MainScene->removeItem(node);
            delete node;
        }
    }
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_7);

    QString projectName;
    int width, height;
    in >> projectName >> width >> height;
    qDebug() <<"读入"<< projectName << width << height;
    _CurrentProjectName = projectName;
    _canvasHeight = height;
    _canvasWidth = width;
    //读取所有Node
    int nodeCount;
    in >> nodeCount;
    qDebug() <<"读入"<< nodeCount;
    QList<std::tuple<QPointF,NodeType,int,int>> nodes;
    for (int i = 0; i < nodeCount; i++)
    {
        QPointF pos;
        int type;
        int inputCount, outputCount;
        in >> pos >> type;
        qDebug() <<"读入"<< pos << type;
        in >> inputCount >> outputCount;
        qDebug() <<"读入"<< inputCount << outputCount;

        nodes.push_back(std::make_tuple(pos,static_cast<NodeType>(type),inputCount,outputCount));
        Node* newNode = _MainView->addNode(static_cast<NodeType>(type),pos);

        newNode->setTerminalNum(TerminalType::inputTerminal,inputCount);
        newNode->setTerminalNum(TerminalType::outputTerminal,outputCount);
    }
    //读取所有连线
    int lineCount;
    in >> lineCount;
    qDebug() <<"读入"<< lineCount;
    for (int i = 0; i < lineCount; i++)
    {
        QPointF start, end;
        in >> start >> end;
        qDebug() <<"读入"<< start << end;
        ConnectLine* _lineToConnect;
        _lineToConnect = new ConnectLine(start,end,_MainScene);
        _MainScene->addItem(_lineToConnect);
        _lineToConnect->redrawline();
    }
    file.close();
}

void MainWindow::saveProject()
{
    //把所有Node的type和位置、以及Node的OutputTerminal、inputTerminal的数量保存到文件中
    //把所有连线的起始Terminal和终止Terminal的位置保存到文件中
    //自定义文件扩展名为.ima
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this, QChinese("保存文件"), _CurrentProjectName, "Imaging Project Files (*.ima)");
    if (fileName.isEmpty())
        return;
    saveProjectToPath(fileName);
}

void MainWindow::saveProjectToPath(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this, QChinese("无法打开文件"), file.errorString());
        return;
    }
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_7); //mark

    out << _CurrentProjectName << _canvasWidth << _canvasHeight;
    qDebug() <<"保存Project"<< _CurrentProjectName << _canvasWidth << _canvasHeight;

    //保存所有Node
    QList<Node *> nodes = *_MainScene->_nodes;
    out << int(nodes.size());
    qDebug() <<"保存Nodesize"<< nodes.size();

    for (Node *node : nodes)
    {
        out << node->scenePos() << int(node->_nodeType);
        qDebug() <<"保存pos"<< node->scenePos() << int(node->_nodeType);

        out << int(node->_inputTerminals.size()) << int(node->_outputTerminals.size()) ;
        qDebug() <<"保存terminalsize"<< node->_inputTerminals.size()<< node->_outputTerminals.size();
    }
    //保存所有连线
    QList<QGraphicsItem *> items = _MainScene->items();
    // 新建一个新list 提取所有connectline
    QList<ConnectLine *> lines;
    for(int i = 0; i < items.size(); i++)
        if(items[i]->type() == 3000)
        {
            lines.append(static_cast<ConnectLine *>(items[i]));
        }

    out << int(lines.size());
    qDebug() <<"保存linesize"<< lines.size();

    for (int i = 0; i < lines.size(); i++)
    {
        out << lines[i]->_inTerminal->scenePos() << lines[i]->_outTerminal->scenePos();
        qDebug() <<"保存linepos"<< lines[i]->_inTerminal->scenePos() << lines[i]->_outTerminal->scenePos();
    }
}

void MainWindow::changeCanvasSize()
{
    // 弹窗，输入新的画布大小
    QDialog *dialog = new QDialog(this);
    // 设置窗口标题和大小
    dialog->setWindowTitle(QChinese("更改画布大小"));
    dialog->setFixedSize(500, 500);
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    QLabel *label = new QLabel(QChinese("请输入新的画布大小"));
    // 两个数值输入框
    QLabel *widthLabel = new QLabel(QChinese("宽度"));
    QLineEdit *widthEdit = new QLineEdit(dialog);
    QLabel *heightLabel = new QLabel(QChinese("高度"));
    QLineEdit *heightEdit = new QLineEdit(dialog);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dialog);
    connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
    layout->addWidget(label);
    layout->addWidget(widthLabel);
    layout->addWidget(widthEdit);
    layout->addWidget(heightLabel);
    layout->addWidget(heightEdit);
    layout->addWidget(buttonBox);
    if (dialog->exec() == QDialog::Accepted)
    {
        // 更新画布大小
        updateCanvasSize(widthEdit->text().toInt(), heightEdit->text().toInt());
    }

}

void MainWindow::showAbout()
{
    class AboutDialog : public QDialog
    {
    public:
        AboutDialog(QWidget *parent = nullptr) : QDialog(parent)
        {
            setWindowTitle(QChinese("关于"));
            setMinimumSize(600, 300);
            setWindowIcon(QIcon(":/window/imaging.jpg"));
            QVBoxLayout *layout = new QVBoxLayout(this);

            QLabel *textLabel = new QLabel(QChinese("本项目是北京大学2024年春程序设计实习课程 “重生之我编程填空都队” ( 沈睿弘、陈睿哲、高美珺 信息科学技术学院23级 ) 的Qt面向对象大作业。我们使用 Qt+OpenCV 开发了一个节点式图像处理软件 Imaging ，使用户能通过增减、移动、连接具有不同功能的函数节点对图像进行各种操作与调整，从而实现图像的调色、美化和再创作。"), this); // 替换成你想要显示的文字
            textLabel->setWordWrap(1);
            layout->addWidget(textLabel);

        }
    };

    AboutDialog *aboutDialog = new AboutDialog();
    aboutDialog->exec();
}

void MainWindow::showHelp()
{
    QUrl url("https://github.com/nnnnnmt/Imaging");
    QDesktopServices::openUrl(url);
}

void MainWindow::autoSave()
{
    // 要求场景中没有线正在被拖拽
    if (_MainScene->_lineToConnect!=nullptr)
        return;
    _AutoSaveTime = QTime::currentTime();
    _timeValueLabel->setText(_AutoSaveTime.toString());
    // 如果autosave文件夹不存在，创建
    QDir dir;
    if (!dir.exists("./autosave"))
        dir.mkdir("./autosave");
    // 保存
    saveProjectToPath("./autosave/" + _CurrentProjectName + _AutoSaveTime.toString("hhmmss") + ".ima");
    qDebug() << "autosave";
}

void MainWindow::setBasicInformation()
{
    setMinimumSize(1440, 900);
    setWindowTitle(tr(QChinese("程序名称").toStdString().c_str()));
    setWindowIcon(QIcon("./icos/1.ico"));
    //_outputImage = cv::imread("./pics/null.png", -1); // 替换成你的图片路径
    QFile file(":/window/null.png");
    cv::Mat m;
    if(file.open(QIODevice::ReadOnly))
    {
        qint64 sz = file.size();
        std::vector<uchar> buf(sz);
        file.read((char*)buf.data(), sz);
        m = cv::imdecode(buf, -1);
    }
    _outputImage = m.clone();
    _canvasWidth = 800;
    _canvasHeight = 600;
}

void MainWindow::createMenu()
{
    _MenuBar = new QMenuBar(this);
    _MenuBar->setWindowFlags(Qt::FramelessWindowHint);//
    _MenuFile = new QMenu(QChinese("文件"), this);
    _MenuAbout = new QMenu(QChinese("关于"), this);
    //_MenuNode = new QMenu(QChinese("节点"), this);
    _Action_File_NewProject = new QAction(QChinese("新建项目"), this);
    _Action_File_OpenProject = new QAction(QChinese("打开项目"), this);
    _Action_File_SaveProject = new QAction(QChinese("保存项目"), this);
    _Action_File_SaveOutput = new QAction(QChinese("保存图像"), this);
    _Action_File_ChangeCanvasSize = new QAction(QChinese("更改画布大小"), this);
    _Action_File_CtrlZ = new QAction(QChinese("撤销"), this);
    _Action_File_Exit = new QAction(QChinese("退出"), this);
    _Action_About_About = new QAction(QChinese("关于"), this);
    _Action_About_Help = new QAction(QChinese("帮助"), this);
    //_Action_Node_Delete = new QAction(QChinese("删除节点"), this);
    _MenuFile->addAction(_Action_File_NewProject);
    _MenuFile->addAction(_Action_File_OpenProject);
    _MenuFile->addAction(_Action_File_SaveProject);
    _MenuFile->addAction(_Action_File_SaveOutput);
    _MenuFile->addAction(_Action_File_ChangeCanvasSize);
    _MenuFile->addAction(_Action_File_CtrlZ);
    _MenuFile->addAction(_Action_File_Exit);
    _MenuAbout->addAction(_Action_About_About);
    _MenuAbout->addAction(_Action_About_Help);
    //_MenuNode->addAction(_Action_Node_Delete);
    _MenuBar->addMenu(_MenuFile);
    //_MenuBar->addMenu(_MenuNode);
    _MenuBar->addMenu(_MenuAbout);
    //_MenuBar->setStyleSheet(""); TO-DO
    setMenuBar(_MenuBar);
    connect(_Action_File_NewProject, &QAction::triggered, this, &MainWindow::newProject);
    connect(_Action_File_OpenProject, &QAction::triggered, this, &MainWindow::openProject);
    connect(_Action_File_SaveProject, &QAction::triggered, this, &MainWindow::saveProject);
    connect(_Action_File_SaveOutput, &QAction::triggered, this, &MainWindow::saveOutput);
    connect(_Action_File_ChangeCanvasSize, &QAction::triggered, this, &MainWindow::changeCanvasSize);
    connect(_Action_File_CtrlZ, &QAction::triggered, this, &MainWindow::undo);
    connect(_Action_File_Exit, &QAction::triggered, this, &QApplication::quit);
    connect(_Action_About_About, &QAction::triggered, this, &MainWindow::showAbout);
    connect(_Action_About_Help, &QAction::triggered, this, &MainWindow::showHelp);
}

void MainWindow::createNodeChoiceDock()
{
    _NodeChoice = new NodeChoice(this);
    _NodeChoice->setMinimumWidth(300);
    _Dock_NodeChoice = new QDockWidget(this);
    _Dock_NodeChoice->setWidget(_NodeChoice);
    _Dock_NodeChoice->setTitleBarWidget(new QWidget());
    _Dock_NodeChoice->setFeatures(QDockWidget::NoDockWidgetFeatures);
    addDockWidget(Qt::LeftDockWidgetArea, _Dock_NodeChoice);
}

void MainWindow::createOutcomeDock()
{
    _Dock_Outcome = new CustomDockWidget(QChinese("输出图像"), this);
    _Dock_Outcome->setTitleBarWidget(new QWidget());
    _Dock_Outcome->setFeatures(QDockWidget::NoDockWidgetFeatures);
    cv::resize(_outputImage, _outputImage, cv::Size(_canvasWidth, _canvasHeight));
    QImage img = cvMat2QImage(_outputImage);

    _outcomeLabel = new ImageWidget(img, this);
    _Dock_Outcome->setWidget(_outcomeLabel);
    //_OutComelabel = new QLabel(QChinese("输出图像"), this);
    //_Dock_Outcome->setWidget(_OutComelabel);

    //_OutComelabel->setPixmap(QPixmap::fromImage(img));
    // img = img.width() > img.height() ? img.scaledToWidth(800) : img.scaledToHeight(800);
    addDockWidget(Qt::RightDockWidgetArea, _Dock_Outcome);
}

void MainWindow::createInformationDock()
{
    _Dock_Information = new CustomDockWidget(QChinese("信息"), this);
    _InformationLabel = new QLabel(QChinese("用于记录历史记录等东西"), this);
    _Dock_Information->setWidget(_InformationLabel);
    _Dock_Information->setFeatures(QDockWidget::NoDockWidgetFeatures);
    addDockWidget(Qt::RightDockWidgetArea, _Dock_Information);
}

void MainWindow::createInformationBar()
{
    _InformationBar = new QStatusBar(this);
    _InformationBar->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                                   "stop:0 rgb(230,250,255),"
                                   "stop:0.2 rgb(230,250,255),"
                                   "stop:0.5 rgb(228,245,235),"
                                   "stop:0.8 rgb(255,250,220),"
                                   "stop:1 rgb(255,250,220));");
    setStatusBar(_InformationBar);
    QWidget* centralWidget = new QWidget(_InformationBar);
    centralWidget->setStyleSheet("background:none;");
    QHBoxLayout* layout = new QHBoxLayout(centralWidget);
    layout->addStretch(1);  // 添加伸缩项目以居中显示标签
    EditableLabel* nameLabel = new EditableLabel(QChinese("未命名项目"), centralWidget);
    nameLabel->setStyleSheet("background:none;");
    connect(nameLabel, &EditableLabel::textChanged, this, &MainWindow::updateProjectName);
    qDebug() << _CurrentProjectName;
    layout->addWidget(nameLabel);
    layout->addStretch(1);  // 添加伸缩项目以居中显示标签
    centralWidget->setLayout(layout);
    _InformationBar->addPermanentWidget(centralWidget, 1);
    // 在右侧显示当前画布大小
    _sizeLabel = new QLabel(QChinese("当前画布大小："), this);
    _sizeLabel->setStyleSheet("background:none;");
    _InformationBar->addPermanentWidget(_sizeLabel);
    _sizeValueLabel = new QLabel(QString::number(_canvasWidth) + "x" + QString::number(_canvasHeight), this);
    _sizeValueLabel->setStyleSheet("background:none;");
    _InformationBar->addPermanentWidget(_sizeValueLabel);
    // 在右侧显示_AutoSaveTime
    QLabel* timeLabel = new QLabel(QChinese("上一次自动保存时间："), this);
    timeLabel->setStyleSheet("background:none;");
    _InformationBar->addPermanentWidget(timeLabel);
    _timeValueLabel = new QLabel(_AutoSaveTime.toString(), this);
    _timeValueLabel->setStyleSheet("background:none;");
    _InformationBar->addPermanentWidget(_timeValueLabel);

}

void MainWindow::createMainView()
{
    _MainScene = new MainScene(this);
    _MainView = new MainView(_MainScene, this);
    _MainScene->syncNodes(&_MainView->_nodes);
    _MainScene->changeCanvasSize(_canvasWidth,_canvasHeight);
    setCentralWidget(_MainView);
    _NodeChoice->raise();
}

void MainWindow::createTimer()
{
    _OutcomeRefreshTimer = new QTimer(this);
    connect(_OutcomeRefreshTimer, &QTimer::timeout, this, &MainWindow::refreshOutcome);
    _OutcomeRefreshTimer->start(2000);

    _AutoSaveTimer = new QTimer(this);
    connect(_AutoSaveTimer, &QTimer::timeout, this, &MainWindow::autoSave);
    _AutoSaveTimer->start(60000);
}

void MainWindow::refreshOutcome()
{
    // 从_Nodes中查找输出节点
    qDebug() << "Refreshing outcome...";
    _MainScene->refresh_NodeOutputs();
    Node* outputNode = nullptr;
    for (Node* node : _MainView->_nodes)
    {
        if (node->_nodeType == NodeType::out_outputNode)
        {
            outputNode = node;
            break;
        }
    }
    if (outputNode == nullptr)
    {
        qDebug() << "No output node found!";
        // 弹出警告框
        QMessageBox::warning(nullptr,"Error","没有输出节点",QMessageBox::Ok);
        return;
    }
    // 提取输出图像
    _outputImage = outputNode->_outputImage;
    // 显示在OutcomeDock中
    if (_outputImage.empty())
    {
       // qDebug() << "Output image is empty!";

        QFile file(":/window/null.png");
        cv::Mat nullimg;
        if(file.open(QIODevice::ReadOnly))
        {
            qint64 sz = file.size();
            std::vector<uchar> buf(sz);
            file.read((char*)buf.data(), sz);
            nullimg = cv::imdecode(buf, -1);
        }
        cv::resize(nullimg, nullimg, cv::Size(_canvasWidth, _canvasHeight));
        QImage img = cvMat2QImage(nullimg);
        _outcomeLabel->setPixmap(QPixmap::fromImage(img));
        return;
    }
    else
    {
        //qDebug()<<"got output";
        cv::Mat scaledImage;
        cv::resize(_outputImage, scaledImage, cv::Size(_canvasWidth, _canvasHeight));
        QImage img = cvMat2QImage(scaledImage);
        _outcomeLabel->setPixmap(QPixmap::fromImage(img));
    }
}

void MainWindow::saveOutput()
{
    //  把outputImage保存到文件，默认为_CurrentProjectName
    QString fileName = QFileDialog::getSaveFileName(this, QChinese("保存输出图像"), _CurrentProjectName, QChinese("Images (*.png *.jpg)"));
    if (fileName.isEmpty())
    {
        return;
    }
    cv::imwrite(fileName.toStdString(), _outputImage);
    qDebug() << "Saved output image to:" << fileName;
}

void MainWindow::undo()
{
    // 撤销上一个添加的节点或连接线
    _MainScene->undo();
}

void MainWindow::updateProjectName(const QString &newName)
{
    qDebug() << "Updated project name to:" << newName;
    _CurrentProjectName = newName;
}

void MainWindow::updateCanvasSize(int width,int height)
{
    qDebug() << "Updated canvas size to:" << width << "x" << height;
    _canvasWidth = width;
    _canvasHeight = height;
    // 更新OutcomeDock中的图片
    cv::resize(_outputImage, _outputImage, cv::Size(_canvasWidth, _canvasHeight));
    QImage img = cvMat2QImage(_outputImage);
    _outcomeLabel->setPixmap(QPixmap::fromImage(img));

    // 更新InformationBar中的画布大小
    _sizeValueLabel->setText(QString::number(_canvasWidth) + "x" + QString::number(_canvasHeight));
    _MainScene->changeCanvasSize(_canvasWidth,_canvasHeight);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // 如果按下Ctrl+G 调整画布大小
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_G)
    {
        changeCanvasSize();
    }
    // 如果按下Ctrl+S 保存项目
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_S)
    {
        saveProject();
    }
    // 如果按下Ctrl+Alt+S 保存输出
    if (event->modifiers() == (Qt::ControlModifier | Qt::AltModifier) && event->key() == Qt::Key_S)
    {
        saveOutput();
    }
    // 如果按下Ctrl+Z 撤销
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Z)
    {
        undo();
    }
}
