## 原始

为了方便大家学习，将Games101的Homework从原Linux平台移植到Window平台。

本仓库仅为Homework的框架，不是Homework的答案。

其中：
1.Opencv已经在Win10+VS2019环境下编译好了。当然也可以自行编译。

## 个人

首先感谢前人整理，该作业对理解games101课程帮助很大。

个人结果放在对应myanswer文件夹中，仅供参考。

macOS下，opencv需要重新安装，cmake中设置find package即可，eigen3不需要修改。

### myanswer1

支持三角形绕任意轴旋转，可以通过以下命令指定旋转轴，然后同样通过`A`，`D`按键旋转。

```shell
./Rasterizer -a 0 1 -1
```
