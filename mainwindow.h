#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "utils.h"
#include "imagewidget.h"


#define QChinese(str) QString(str)

class NodeChoice;
class MainView;
class MainScene;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    void newProject();
    void openProject();
    void saveProject();
    void saveProjectToPath(const QString& path);
    void changeCanvasSize();
    void showAbout();
    void showHelp();
    void autoSave();
private:
    void setBasicInformation();
    void createMenu();
    void createNodeChoiceDock();
    void createOutcomeDock();
    void createInformationDock();
    void createInformationBar();
    void createMainView();
    void createTimer();
    void refreshOutcome();
    void saveOutput();
    void undo();
public slots:
    void updateProjectName(const QString& newName);
    void updateCanvasSize(int width,int height);
private:
    QMenuBar* _MenuBar; // 菜单栏
    QMenu* _MenuFile; // 文件菜单
    QMenu* _MenuAbout; // 关于菜单
    QMenu* _MenuNode; // 节点菜单
    QAction* _Action_File_NewProject;
    QAction* _Action_File_ChangeCanvasSize;
    QAction* _Action_File_OpenProject;
    QAction* _Action_File_SaveProject;
    QAction* _Action_File_SaveOutput;
    QAction* _Action_File_CtrlZ;
    QAction* _Action_File_Exit;
    QAction* _Action_About_About;
    QAction* _Action_About_Help;
    QAction* _Action_Node_Delete;

    NodeChoice *_NodeChoice; // 节点选择窗口
    MainScene *_MainScene; // 节点场景
    MainView *_MainView; // 节点视图

    cv::Mat _outputImage;
    int _canvasWidth;
    int _canvasHeight;

    QString _CurrentProjectName = "未命名项目";
    QStackedWidget _StackedWidget; // 中央节点编辑区
    QStatusBar *_InformationBar; // 状态栏
    QDockWidget *_Dock_NodeChoice; // 左侧浮动节点选择窗口
    CustomDockWidget *_Dock_Outcome; // 右侧浮动结果窗口
    QDockWidget *_Dock_Information; // 右下册浮动信息窗口
    QLabel *_InformationLabel; // 信息窗口
    QPushButton* _actWidget; //运行浮窗
    QDockWidget *_Dock_Head;

    QTime _AutoSaveTime = QTime::currentTime();
    QTimer* _AutoSaveTimer; // 自动保存定时器
    QTimer* _OutcomeRefreshTimer; // 结果刷新定时器

    QLabel* _sizeLabel;
    QLabel* _sizeValueLabel;
    ImageWidget* _outcomeLabel;
    QLabel* _timeValueLabel;

    void keyPressEvent(QKeyEvent *event);
};
#endif // MAINWINDOW_H
