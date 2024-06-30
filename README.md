# Imaging：节点式图像处理软件
## 简介
本项目是**北京大学2024年春程序设计实习**课程 “`重生之我编程填空都队`” ( **沈睿弘、陈睿哲、高美珺** 信息科学技术学院23级 ) 的Qt面向对象大作业。我们使用 Qt+OpenCV 开发了一个节点式图像处理软件 `Imaging` ，使用户能通过增减、移动、连接具有不同功能的函数节点对图像进行各种操作与调整，从而实现图像的调色、美化和再创作。

## 功能
### 功能列表
##### 已实现节点
| 节点类型          | 节点名称                      | 功能描述                                                                                             |
|-------------------|-------------------------------|------------------------------------------------------------------------------------------------------|
| **输入节点**      | InputNode                     | 接受原始图像，提供预处理（移动、缩放、裁剪、AI抠图、旋转）                                            |
| **汇聚节点**      | AddNode                       | 添加多个图片输入端，调整图层覆盖顺序，自动调整UI边界                                                  |
| **基础调整节点**      | EditNode                      | 提供移动、裁剪、缩放、旋转功能，支持更精确的操作                                                      |
|                   | ContrastNode                  | 调整对比度                                                                                           |
|                   | BrightnessNode                | 调整亮度                                                                                             |
|                   | SaturationNode                | 调整饱和度                                                                                           |
|                   | HueNode                       | 调整色调                                                                                             |
|                   | ColorTemperatureNode          | 调整色温                                                                                             |
|       | FilterNode                    | 读取滤镜LUT文件，调整滤镜施加程度                                                                     |
| **AI节点**        | AIword2imgNode                | 接受文字prompt生成图像                                                                                |
|                   | AIimg2imgNode                 | 接受图像和文字prompt，根据prompt修改输入图像                                                          |
| **输出节点**      | OutputNode                    | 显示处理后的图像，支持导出图片文件                                                                    |

##### 菜单栏
|菜单 | 功能 | 功能介绍 |
|------|----------------------------------|----------------------------------|
| **文件** | 新建、打开、保存项目             | 新建、读取或保存 ima 文件        |
|      | 保存图像                         | 保存输出节点接受的图像           |
|      | 更改画布大小                     | 更改输出图片的大小               |
|      | 撤销 / `Ctrl+Z` 快捷键           | 撤销连线、新建节点操作           |
|      | 退出                             | 退出软件                         |
| **关于** | 关于                             | 查看软件介绍                     |
|      | 帮助                             | 查看演示视频                     |
##### 其他
| 功能区域               | 描述                                                                                             |
|------------------------|--------------------------------------------------------------------------------------------------|
| **节点选择窗口**       | 支持拖动添加节点                                                                                  |
| **图像预览窗口**       | 支持滚轮缩放和双击放大查看，双击弹出新窗口进行全尺寸输出图片预览                                  |
| **编辑区域**           | 用于放置节点和绘制连接线，支持滚轮缩放和拖动                                                     |
| **信息窗口**           | 显示当前画布大小和上一次自动保存时间，显示并支持修改文件名                                        |
| **运行按钮**           | 位于编辑区域下部，悬浮式按钮，支持手动更新输出图像                                               |

### Demo演示

> #
> ##### *注：关于 AI背景移除、文生图、图生图 功能*
> ### 
> 这部分功能在发布的代码版本中被禁用，如需使用请替换自己的 API-KEY 并注释掉相应代码。
> 我们使用的 API 地址是：
> 1. 背景移除 Removebg https://api.remove.bg/v1.0/removebg
> 2. 文生图 OpenAI Dalle-3 TT-API镜像 https://api.ttapi.io/openai/v1/images/generations
> 3. 图生图 Midjourney 触站API镜像 https://ai.huashi6.com/aiapi/v1/mj/draw
> ##

## 环境及依赖
在本项目中，我们使用的所有环境及依赖库有:
```
Qt 6.7.0
OpenCV 4.9.0 (图像处理功能实现)
Openssl 1.1.1g (访问httpsAPI)
```
1. 下载 Qt Creator、安装 Qt
1. 下载 OpenCV 、OpenSSl 并编译安装
2. 克隆项目源码
```
git clone https://github.com/nnnnnmt/Imaging.git
```
注：下载源码后，需要修改 `QtBigHW.pro` 下的 `INCLUDEPATH` 与 `LIBS` 路径，以确保本项目能够正确编译运行

## 致谢及声明
感谢本课程的任课老师 **刘家瑛老师** 与负责助教 **匡浩伟学长** ，他们在本项目的完成过程中给予了极大的支持和帮助。

本项目在UI设计及功能实现方面部分参考了 [ml blocks](https://www.mlblocks.com/) 与 [Nodegraphic](https://github.com/SynodicMonth/NodeGraphic)，向两个项目的制作团队表示感谢。

由于时间紧迫，本项目不可避免的有许多不完善之处，还请多多指正。如果您发现了bug，欢迎在 issue 中告诉我们！
