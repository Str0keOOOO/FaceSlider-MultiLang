# FaceSlider-MultiLang C++(Qt) 组件

这是一个基于C++(Qt)的滑动变脸器组件，根据Web版本实现而成。

## 构建

此C++项目使用cmake构建，要求cmake最低版本为3.16。构建步骤如下：

```
git clone https://github.com/Str0keOOOO/FaceSlider-MultiLang
cd FaceSlider-MultiLang/cpp
cmake -B build
cmake --build ./build --target all
```

如果成功执行了以上步骤的话，会在build目录下生成一个可执行文件以及两个qm翻译文件。

或者您也可以使用Qt Creator, Visual Studio构建此项目。

## 效果展示

![image](https://github.com/user-attachments/assets/79cf60e8-96ad-4b5e-aa54-9746488b502f)

## 不足

本C++项目与Web版本存在一定的差距，主要有三点：

- 在对动画过渡效果方面的支持，Qt明显差于Web。因此本项目在动画流畅度方面是不如Web版本的。本人尽量会做到动画流畅度高一点，之后可能会这方面进行优化改进。

- 不同于Web端，本项目最终会输出为一个客户端，需要考虑布局自适应的情况，因此，本项目在窗口大小调整方面做了部分工作，例如监听了`resizeEvent()`，也因此存在最大化、向下还原调整后眼睛部位的位置不正确的bug。之后可能会对解决此bug。

- Qt在遮罩(mask)方面的处理是较为复杂的，相较于浏览器的一句`overflow: hidden;`，Qt这边所做的要多的多。Qt中如果想要隐藏某一组件的一部分，而另一部分暴露可通过以下几种常用的方法：

  - 父组件对于子组件本身就存在遮挡、裁剪行为，一旦子组件大小超过父组件，超过的部分便会隐藏，但所有组件的实际大小是一个规则图形，即矩形，不论是否设置了圆角，因此这种不能解决非规则图形的遮罩问题。
  - Qt为我们提供了一个专门的遮罩函数`setMask()`，它有两个版本，一个接收`QRegion`，除了上面的矩形外，还可以设置椭圆(Ellipse)、多边形(Polygon)的遮罩，尤其重要的一个版本是可以接收一个`QBitmap`，这个版本使得遮罩的图形可以是任意复杂的图形。

  然而，使用`setMask()`存在严重的缺陷，即无法应用抗锯齿(Antialiasing)，因为遮罩的图形都是使用位图掩码处理的，即要么0，要么1。而抗锯齿是通过在图形边缘使用**半透明像素**或**中间色调**来平滑锯齿状边缘，这方面Web支持得很好。而在本项目中，眼睛和嘴巴部分你可以明显地看到锯齿状。

## 属性

| 属性名            | 类型      | 默认值        | 描述                               |
| ----------------- | --------- | ------------- | ---------------------------------- |
| `maxUnhappyCount` | `size_t`  | `3`           | 最大拒绝次数                       |
| `faceX`           | `double`  | `0.5`         | 对应于Web的`normal.face.px`        |
| `faceY`           | `double`  | `0.5`         | 对应于Web的`normal.face.py`        |
| `happiness`       | `double`  | `0.9`         | 对应于Web的`normal.face.happiness` |
| `drep`            | `double`  | `1.0`         | 对应于Web的`normal.face.drep`      |
| `title`           | `QString` | ——            | 对应于Web的`normal.ui.title`       |
| `subtitle`        | `QString` | ——            | 对应于Web的`normal.ui.subtitle`    |
| `btnHappyText`    | `QString` | `"cancel"`    | 取消按钮文字                       |
| `btnUnhappyText`  | `QString` | `"uninstall"` | 拒绝按钮文字                       |

所有setter方法均返回了类实例的引用，因此可链式调用所有setter方法。调用公有接口示例如下：

```C++
int main() {
    QApplication a;
    // ...
	FaceSlider w;
	w.setFaceX(0.5)
    	.setFaceY(0.5)
    	.setHappiness(0.2)
    	.setDrep(0.0)
    	.setTitle("Hey! Don't do that!")
    	.setSubtitle("Just cancel it, please.")
    	.setMaxUnhappyCount(20)
        .setBtnHappyText("Click me")
        .setBtnUnhappyText("Not this")
		.show();
    return a.exec();
}
```

## 其他

本C++版本还提供了**多语言**功能的支持，您可**右键界面**，选择相应的语言，目前仅支持简体中文、English。

![image](https://github.com/user-attachments/assets/6cf77cc4-5cc0-4f7e-9de6-4122614d3d9e)

## 贡献

欢迎提交 Issue 或 Pull Request 来改进此组件。
