# 飞虎角度计算器

独立的 Windows 64 位离线工具。安装后通过桌面或开始菜单中的「飞虎角度计算器」打开。
无需安装地面站、连接飞控或输入激活码。

包含四项原有功能：

- 风向角度：按预设或手工风向、相对来风方向计算航向角。
- 时间角度：24 小时对应 360°，1 小时对应 15°。
- 三角函数：sin、cos、tan、arcsin、arccos、arctan，支持角度和弧度。
- 直角三角形：输入两条边，或一条边和夹角 θ，计算其余边角并显示示意图。

每项结果均可复制。直角三角形重新计算前可点击「清空」。窗口支持缩放，内容过多时可滚动。

## 构建

本目录是独立 CMake 工程，不依赖地面站构建。需要 C++20、CMake 3.25、Qt 6.8 或更高版本的
Widgets、Qml、Test 模块。Windows 安装包由 `.github/workflows/calculator.yml` 构建、测试和验证。

```sh
cmake -S custom/tools/calculator -B build/calculator -DCMAKE_PREFIX_PATH=/path/to/Qt
cmake --build build/calculator --config Release
ctest --test-dir build/calculator -C Release --output-on-failure -L Unit
```

计算引擎在编译时直接嵌入 `custom/src/PlanView/CaacCalculatorMath.js`，与地面站使用同一份公式。
发行包附带源代码压缩包和开源许可，详见 `NOTICE_zh-CN.txt`。

内置字体覆盖工具界面文字，无需依赖系统中文字体。维护界面文字后，可安装 `fonttools` 并运行
`python custom/tools/calculator/make_font.py /path/to/NotoSansCJK-Regular.ttc` 重新生成字体子集。
