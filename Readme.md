## 原始

为了方便大家学习，将Games101的Homework从原Linux平台移植到Window平台。

本仓库仅为Homework的框架，不是Homework的答案。

其中：
1.Opencv已经在Win10+VS2019环境下编译好了。当然也可以自行编译。

## 个人

首先感谢前人整理，该作业对理解games101课程帮助很大。另外添加了最终大作业pdf，包含许多有趣的研究方向。

个人结果放在对应myanswer文件夹中，仅供参考。

> macOS下，opencv需要重新安装（推荐直接brew install安装），cmake中设置find package即可，eigen3不需要修改（也可以重新安装，不过需要在cmakelist中修改include路径为安装位置）。

### myanswer1

支持三角形绕任意轴旋转，可以通过以下命令指定旋转轴，然后同样通过 `A`，`D`按键旋转。

```shell
./Rasterizer -a 0 1 -1
```

### myanswer2

通过一个格子内4个点采样再计算平均值的方式做抗锯齿。

output.png为原图

output-MSAA.png为抗锯齿之后的结果

### myanswer3

修改了小部分main函数，使得奶牛类似前面作业一样转起来（另外似乎是由于帧率太低，cv读不到键盘输入，直接默认一帧转固定角度）。

法向量结果保存在output-normal.png中。

PS：~~不是很能理解 `normal_fragment_shader`的意义~~ 最后的结果看起来是将法向量XYZ三个值直接映射到RGB三色上，导致x轴越大越红，y轴越大越绿，z轴越大越蓝（似乎可以理解为三个坐标轴正方向上有一盏对应颜色的灯向坐标轴负方向照射）。

Blinn-Phong结果保存在output-phong.png中

PS：specular light计算时使用半程向量。 ~~所以为什么还需要max函数？~~ （看起来算法非常的暴力，照不到的面也直接丢到公式里计算结果）

Texture结果保存在output-texture.png中

PS：总算得到了一个能看的奶牛。

Texture 是否开启 Bilinear 的结果保存在output-texture-small.png和output-texture-small-bilinear.png中。（为了看到bilinear的效果，原材质图需要降分辨率，修改后文件保存在./models/spot/spot_texture_small.png中）

bump结果保存在output-bump.png中，displacement结果保存在output-displacement.png中。

#### 3坑 插值计算

作业2里直接拿来用了，没管，但作业3要插值其他属性。深度值理论上取倒数，然后重心坐标系数线性组合一下就完事了，~~怎么代码里多了个w~~（这个是重心坐标系下完整版公式，若加上三角形面限制$\alpha + \beta + \gamma = 1$，公式退化为简易倒数形式）。

搜索网络，发现前人问过[相关问题](https://games-cn.org/forums/topic/zuoye3-guanyushenduzhiwentizijicaidekengheyixiexiangfa/)。原理没有问题，是代码框架的锅。相关问题中讨论的更深入一些，供后续梳理与回顾。

本人菜鸡，没有意识到讨论的问题点在哪里（单纯觉得框架有点难用），以先理解简易框架为目标。

#### 3坑 凹凸贴图

bump mapping计算推导看不懂（据说光追部分会有详细推导），大致意思是先通过贴图计算贴图空间的法向量，然后通过TBN矩阵转化到世界坐标系下。

其他框架不容易理解的地方在作业讨论区里有[详细解答](https://games-cn.org/forums/topic/frequently-asked-questionskeep-updating/)。

### myanswer4

同时开启naive_bezier和recursive_bezier，将产生黄色曲线，结果保存在my_bezier_curve.png中。

### myanswer5

结果保存在binary.ppm中，另外MT算法的推导略有点复杂，直接抄了公式。具体推导可以参考[这里](https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection)

#### 5坑

1. 运行时报错。Renderer.cpp最后保存颜色的数组是unsigned char而中间结果使用char。
2. 地面是黑的。光线与三角形求交没有加判断t>0。（讨论区发现一样的错误，真神奇！）

### myanswer6

复制answer5函数的时候发现光线生成方向计算错了，少算了2倍。（所以answer5答案看起来视口比老师图片里要窄）

简单捋一下框架运行流程如下，最终BVH在scene建立时一起建立，然后在每次castRay递归计算前，快速判断光线与物体是否存在交点，以此来加速计算。

```c++
main.cpp(
    new scene,
    new renderer,
    renderer -> Render(scene)
)
Render(
    generate primary ray direction,
    scene.castRay(ray, depth)
)
castRay(
    Scene::intersect,
    Whitted-syle light transport algorithm
)
Scene::intersect -> bvh.Intersect -> bvh.getIntersection -> bounds3.IntersectP
```

笔记本运行BVH结果如下，图片结果保存在binary.ppm

```shell
BVH Generation complete:
Time Taken: 0 hrs, 0 mins, 0 secs

 - Generating BVH...

BVH Generation complete:
Time Taken: 0 hrs, 0 mins, 0 secs

Render complete: ======================================================] 100 %
Time taken: 0 hours
          : 0 minutes
          : 8 seconds
```

SAH算法实现有点小麻烦，有时间在实现，占坑。

#### 6坑

1. 框架里存在两个BVH结构，一个统管所有场景中的物体（多面体模型只算一个物体），另一个就是模型。这里模型meshTriangle中也构建了一个BVH来快速判断交集，与个人naive的想法不同~~不要愚蠢的以为模型里的intersect调用死循环了~~。
2. 为什么bound3.intersectP需要dirIsNeg？因为bounding box的边界是按照坐标轴方向的，需要转成dir方向。
3. BVH中recursiveBuild里存在一个无用的bound？做SAH计算时需要使用。

### myanswer7

函数功能注释说明

```c++
// in Scene.cpp。求场景与射线的交点。
Intersection intersect(const Ray& ray);
// in Scene.cpp。随机选择一个光源并对光源采样
void sampleLight(Intersection& pos, float& pdf);
// in Scene.cpp
RussianRoulette
// in Material.hpp。对于漫反射材质：在半球面上以某种分布采样一个方向
Vector3f sample(const Vector3f wi, const Vector3f N)
// in Material.cpp。对于漫反射材质：概率密度为1/2pi
float pdf(const Vector3f wi, const Vector3f wo, const Vector3f N)
// in Material.cpp。对于漫反射材质：f_r为Kd/pi
Vector3f eval(const Vector3f wi, const Vector3f wo, const Vector3f N)
```

只完成了最基础的path tracing，且没有多线程，结果保存在binary-spp=16.ppm。

#### 7坑

1. get_random_float变量前加static
2. 看起来trace函数没有完全配套实现，scene里光源就没有特殊存储，所以打到光源还要特殊判断。
3. tExit tEnter存在精度问题，并且需要判断相等，这在之前不用考虑。
4. 直接击中光源的线返回光源材质采样。
5. 判断是否与光源相交可以调用Scene::intersect函数后通过Intersection.obj->hasEmit判断（不太清楚为啥有些解法用距离去判断）。

### myanswer8

库安装搞不定，返回linux安装了opengl完成编译。

finish task1-4
