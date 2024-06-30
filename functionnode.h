#ifndef FUNCTIONNODE_H
#define FUNCTIONNODE_H

#include "utils.h"
#include "node.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class FunctionNodeWidget;
class FilterNodeWidget;
}
QT_END_NAMESPACE

class FunctionNodeWidget : public QWidget
{
    Q_OBJECT
public:
    FunctionNodeWidget(QWidget *parent = nullptr);
    ~FunctionNodeWidget();
public:
    Ui::FunctionNodeWidget *ui;
};

class FilterNodeWidget : public QWidget
{
    Q_OBJECT
public:
    FilterNodeWidget(QWidget *parent = nullptr);
    ~FilterNodeWidget();
public:
    Ui::FilterNodeWidget *ui;
};

class FunctionNode : public Node
{
public:
    FunctionNode(MainScene *scene,QGraphicsItem *parent = nullptr);
    virtual ~FunctionNode();
    QLabel* _label;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {;}
};


class ContrastNode : public FunctionNode
{
public:
    ContrastNode(MainScene *scene,QGraphicsItem *parent = nullptr);
    ~ContrastNode();
    virtual void initProxyWidget() override;
    void execute() override;
    int type() const override {return 4101;}
    void resizeWidget() override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QSlider *_slider;
    QCheckBox *_checkBox;
    FunctionNodeWidget *_widget = nullptr;
};

class BrightnessNode : public FunctionNode
{
public:
    BrightnessNode(MainScene *scene, QGraphicsItem *parent = nullptr);
    ~BrightnessNode();
    virtual void initProxyWidget() override;
    void execute() override;
    int type() const override { return 4102; }
    void resizeWidget() override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QSlider *_slider;
    QCheckBox *_checkBox;
    FunctionNodeWidget *_widget = nullptr;
};

class SaturationNode : public FunctionNode
{
public:
    SaturationNode(MainScene *scene, QGraphicsItem *parent = nullptr);
    ~SaturationNode();
    virtual void initProxyWidget() override;
    void execute() override;
    int type() const override { return 4103; }
    void resizeWidget() override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QSlider *_slider;
    QCheckBox *_checkBox;
    FunctionNodeWidget *_widget = nullptr;
};


class ColorTemperatureNode : public FunctionNode
{
public:
    ColorTemperatureNode(MainScene *scene, QGraphicsItem *parent = nullptr);
    ~ColorTemperatureNode();
    virtual void initProxyWidget() override;
    void execute() override;
    int type() const override { return 4104; }
    /**
     * @brief 传入img与kelvin值，返回色温调整后的图像
     * @param img
     * @param kelvin
     * @return
     */
    cv::Mat adjustColorTemperature(const cv::Mat& img, double kelvin);
    void resizeWidget() override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QSlider *_slider;
    QCheckBox *_checkBox;
    FunctionNodeWidget *_widget = nullptr;
};

class HueNode : public FunctionNode
{
public:
    HueNode(MainScene *scene, QGraphicsItem *parent = nullptr);
    ~HueNode();
    virtual void initProxyWidget() override;
    void execute() override;
    int type() const override { return 4105; }
    /**
     * @brief 传入img与hue值，返回色相调整后的图像
     * @param img
     * @param kelvin
     * @return
     */
    cv::Mat adjustHue(const cv::Mat &img, double hue);
    void resizeWidget() override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QSlider *_slider;
    QCheckBox *_checkBox;
    FunctionNodeWidget *_widget = nullptr;

};

struct LUT3D
{
    int size=0;
    std::vector<cv::Vec3f> data;
};

class FilterNode : public FunctionNode
{
public:
    FilterNode(MainScene *scene, QGraphicsItem *parent = nullptr);
    ~FilterNode();
    virtual void initProxyWidget() override;
    void execute() override;
    int type() const override { return 4106; }
    /**
     * @brief 传入lut与image，根据intensity线性加权得到新的image
     * @param lut
     * @param image
     * @param intensity
     */
    void applyLUT(const LUT3D& lut, cv::Mat& image,float intensity);
    /**
     * @brief 返回颜色查找表中的颜色
     * @param lut
     * @param color
     * @return
     */
    cv::Vec3f applyLUTColor(const LUT3D& lut, const cv::Vec3f& color);
    void loadCubeFile();
    void loadCube(const std::string &fileName);
    void updateLabelWithImage(const cv::Mat &image);
    cv::Mat loadImage(const std::string &filePath);
    void resizeWidget() override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QSlider *_slider;
    QCheckBox *_checkBox;
    FilterNodeWidget *_widget = nullptr;
    LUT3D _lut;
};
#endif // FUNCTIONNODE_H
