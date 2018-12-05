
---
title: cocos2dx基础概念
---
*《cocos2d-x游戏开发》初学笔记*

## 导演，场景，层和精灵
---
### 导演（Director）
* 导演是控制游戏流程的主要组件，包括创建和处理游戏如何执行
### 场景（Scene）
* 一个游戏通常由多个场景构成，包括需要显示的所有游戏元素
### 层（Layer）
* 概要与场景类似，层通常包含屏幕上呈现的具体内容，包括更细致的游戏元素（如精灵等）
### 精灵（Sprite）
* 精灵是开发的最主要组成部分，带包游戏中一个最小的可见单位。

## cocos2dx的组成对象关系图
---
<div align=center>![cocos基本角色](https://github.com/knightjk/knightjk.github.io/blob/hexo/source/images/cocos2dx%E5%9F%BA%E6%9C%AC%E6%A6%82%E5%BF%B5/manjor_concept.PNG?raw=true)
<div align=left>

由以上分类，cocos2dx将对象分为节点类（Node），导演类（Director），场景类（Scene），层类（Layer）和精灵（Sprite）类

## Node类（Node）
---
node类从Ref类公有继承而来，同时也是其他节点的父类，在精灵类（Sprite）头文件有如下定义：
```c
//cocos2d\cocos\2d\CCSprite.h
class CC_DLL Sprite : public Node, public TextureProtocol{....};
```
在场景类（Scene）头文件中：
```c
//cocos2d\cocos\2d\CCScene.h
class CC_DLL Scene : public Node{....};
```
在层类（Layer）中
```c
//cocos2d\cocos\2d\CCLayer.h
class CC_DLL Layer : public Node{......};
```
其头文件声明在工程目录cocos2d\cocos\2d\CCNode.h下。一个Node主要功能有：
* 包含其他节点

* 通过定时器预定消息和使用回调

* 执行动作
### Node类的成员变量如下
```c
//cocos2d\cocos\2d\CCNode.h
class CC_DLL Node : public Ref
{
    ......
protected:
    float _rotationX;               ///< 表示节点X轴的水平旋转角度
    float _rotationY;               ///< 表示节点Y轴的水平旋转角度

    // rotation Z is decomposed in 2 to simulate Skew for Flash animations
    float _rotationZ_X;             ///< rotation angle on Z-axis, component X
    float _rotationZ_Y;             ///< rotation angle on Z-axis, component Y
    
    Quaternion _rotationQuat;       ///rotation using quaternion, if _rotationZ_X == _rotationZ_Y, _rotationQuat = RotationZ_X * RotationY * RotationX, else _rotationQuat = RotationY * RotationX

    float _scaleX;                  ///< scaling factor on x-axis
    float _scaleY;                  ///< scaling factor on y-axis
    float _scaleZ;                  ///< scaling factor on z-axis
    ......
}
```
### Node类的成员函数
>Node的成员函数分为三类，分别是对节点的处理，如何执行动作和使用定时器预定信息，还有其他少数应用功能的函数

* 节点处理函数

* 节点动作

* 定时器

* 其他

## 导演类（Director）
---
导演只有一个，所以Director是一个单例类，通过getInstance()函数获取实例，导演类的功能有以下：

* 初始化OpenGL环境

* 创建，访问和切换场景，且一个时间点上只能运行一个场景

* 访问Cocos2d-x配置

* 访问视图

* 暂停，恢复和结束游戏
## 场景类（Scene）
---
场景类是层类的容器，按游戏内容分为三类：
* 展示场景类：提示，游戏介绍，帮助

* 选项类场景：菜单，参数

* 游戏内容场景：游戏内容

## 层类（Layer）
---
允许用户输入
## 精灵类（Sprite）
---
精灵是游戏中最小可见单位，在游戏中显示为一个图像，该图像被加载入内存时，被处理成未压缩的纹理图像来存储，再次使用时可以直接从内存读取该图像而不用重新加载图像内存，纹理即图像，纹理的宽度和高度必须是2的n次方，纹理相关类如下：
* Texture2D（纹理）：创建OpenGL 2D纹理图像
* TextureCache（纹理缓存）：单例，加载和管理纹理图像
* TextureAtlas（纹理图集）：可以是Texture2D支持的文件类型

### 精灵类（Sprite）
精灵类不像其他类一样只继承Node，同时继承和派生其他精灵，如下图所示：
<div align=center>![精灵类](https://github.com/knightjk/knightjk.github.io/blob/hexo/source/images/cocos2dx%E5%9F%BA%E6%9C%AC%E6%A6%82%E5%BF%B5/sprite.jpg?raw=true)
<div align=left>

### 精灵帧类（SpriteFrame）
精灵帧是通过被其快速更换来实现动态效果，精灵帧类从Ref类直接继承而不从Node类继承，这一点与精灵类不一样：
```c
//cocos2d\cocos\2d\CCSpriteFrame.h
class CC_DLL SpriteFrame : public Ref, public Clonable{......}
```
### 精灵帧缓存类（SpriteFrameCache）
精灵帧缓存类是一个单例类(通过getInstance获取)，也是直接继承自Ref类，所有精灵可共享用于存储精灵帧来提高运行效率。
```c
//cocos2d\cocos\2d\CCSpriteFrameCache.h
class CC_DLL SpriteFrameCache : public Ref{......}
```
### 精灵表单类（SpriteBatchNode）
OpenGL ES将图片绑定Sprite和渲染会大量消耗CPU，每个屏幕上的精灵会导致程序运行越来越卡，通过精灵表单（SpriteBatchNode），使用同一张纹理图片，这样OpenGL ES只需要一次纹理绘制，提高运行速度。
精灵表单的限制：
* SpriteBatchNode只能接受精灵对象为其子节点

* 所有精灵对象必须使用同一张干净的纹理图片
>精灵表单是一张组合成的图片，并且附带有一个图片文件列表记录图片(后缀名为*.plist)。通过加载*.plist的信息，从组合成的图片里面提取对应的图片区域，就生成对应区域的精灵；精灵表单的制作可用TexturePacker。

## 调度器（Schedule）
---
