# 飞行数据与菜单字号验证记录

应用源码：`10a11194aaee9b3baf096dd4aea514d4c418bb88`。
分支：`phase3/caac-route-geometry`。日期：2026-10-11（Asia/Shanghai）。
本记录及随后提交的两处测试修正不改变该安装包的应用行为。

## 本次实现

- 普通菜单采用默认字号的 1.2 倍，原有大字号飞行模式标题在原字号基础上放大 1.2 倍，保留用户字体和界面缩放设置。
  覆盖顶部操作、设置页主/子导航、下拉与右键菜单、标签、飞行模式菜单和航线操作栏。
- 工具栏按可见文字宽度适配，航线操作栏跟随左侧工具栏实际宽度定位，修复放大字号后“撤销”与原生“起飞”按钮重叠造成的误触。
  设置页长名称支持换行，下拉菜单随字体变化重新计算内容宽度。
- 新增飞行数据三页：飞行时间/主 GPS 卫星数/海拔高度/空速、实时原始飞行日志、四向累计里程。
  支持滑动和点击页点；无飞行器时显示未获取数据。
- 四向里程按 WGS84 水平距离和方位角分解，以米显示。东北飞行 100 米分别累计约 70.7 米至北、东；返回时南、西继续累计。
  实际离地清零，降落后保留，重新起飞再次清零；各飞行器分别保存。
  无效定位、遥测中断及恢复时断开采样连续性，不补算未知轨迹。
- 右侧准心点击后将当前无人机居中。
- 修复 QML 调用 `getFact()` 后错误回收父对象为空的 C++ 内嵌 Fact 的崩溃。
  借用查询保持 C++ 所有权；直接/嵌套查询、强制 GC 和地图引擎销毁有回归覆盖。

## 验证

| 检查 | 结果 |
| --- | --- |
| Linux Debug 编译及仓库支持的启动检查 | 通过 |
| 相同应用代码重点回归 | 8/8，63.31 秒 |
| 完整 Unit/Integration 回归 | 首次 426/428，1127.58 秒；全部 342 项 Unit 通过 |
| 两项失败测试修正后复测 | 2/2，47.01 秒 |
| 中英文菜单、125% 缩放 | TopLevelViewsTest 与 CaacChineseUITest 2/2，22.49 秒 |
| 修改文件 pre-commit 检查 | 通过 |
| 最新 lupdate 中文审计 | 6496 条消息、28 个技术标识、1 条空原文、0 错误 |
| 工作流及安装版 PATH 相关 Python 测试 | 22/22，1.66 秒 |
| Windows x64 Debug 原生测试 | 6/6，134 秒；x64 作业成功 |
| Windows x64 Release Portable 测试 | 10/10，1.13 秒 |
| Windows x64 Release 安装/升级/启动/卸载 | 全部通过；x64 作业成功 |

首次完整回归中的 TopLevelViewsTest 点击了尚未滚入视口的设置子项，ToolbarIndicatorUITest 向未激活的窗口发送 Escape。
修正测试前置条件：点击前滚动子项到视口，发送键盘事件前请求并等待原生窗口激活；保留原有交互和断言。
完整回归及失败项复测合计覆盖全部 428 项，未重复执行整套回归，也不记作单次 428/428。

Linux 使用 Qt 6.11.1、GCC 14.2、GStreamer 1.26.2、原生 Xcb/Xvfb 与 Mesa OpenGL RHI。
使用受支持的 `QGC_TEST_ONSCREEN=ON`，UI 测试串行执行。
完整回归排除 `Flaky|Network|StockUI`，排除项不记为通过。
125% 缩放验证属于 Linux 原生 Qt 测试，不代替 Windows 人工高 DPI 验收。

Windows Debug 六项为 FactGroupTest、CaacFlightDistanceTest、CaacRouteEditorTest、CaacChineseUITest、CaacRouteEditorUITest、CaacFlightDataUITest。
Release 验证包括编译后启动、实际 NSIS 中文审计、校验值检查、安装与升级、移除 Qt/GStreamer 构建 SDK 路径后的安装版启动、卸载及清理。
Release 中仅为 Debug 配置执行的原生测试跳过，不记为通过。

Linux 证据为 `/workspace/caac-menu-*`、`/workspace/build/qgc-baseline/caac-menu-*.xml`；
Windows 证据为 `/workspace/caac-menu-validation-10a11194aa/`。
截图地图瓦片为离线测试占位内容；飞行数据使用 MockLink 和注入定位信号验证，不构成在线地图、SITL 或实机飞行验收。

## 安装包身份

Windows Debug 运行 `38130493264`、x64 作业 `114443671554`；Release 运行 `38130532031`、x64 作业 `114443776405`，均指向上述应用源码。

[下载 Windows x64 Release 安装包](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38130532031/artifacts/11698679302)。
登录 GitHub 下载 ZIP，解压后运行 `build/QGroundControl-installer-AMD64.exe`。
Artifact：`11698679302`，名称 `caac-validation-amd64-Release`，过期时间 2026-10-25 18:24:19（Asia/Shanghai）。

| 文件 | 字节数 | SHA256 |
| --- | --- | --- |
| 下载 ZIP | 135223760 | `3c949341d604f3b006fc8c32970794c0b257e73a0e994e9abd580675c7d42140` |
| 安装 EXE | 135233237 | `797eff1f6dd321c4c536d73dca0144b0c5c9f6ec5f848614945d4801df63922a` |

ZIP 与 GitHub API digest 一致，EXE 与包内 `.sha256` 一致。
包中两份中文审计均为 0 错误；源码身份由工作流运行的完整 HEAD SHA 确定。
该包未作为正式签名或 CAAC 认证发布。旧源码 `5fc662e165` 的安装包不包含本次功能。

## 后续范围

完整四入口布局、任务详情和地图设置、全局高级设置、模板生成、整条航线拖拽、完整航点高级编辑与度分秒输入仍待实施，详见 [功能布局清单](lingdong-ui-implementation.md)。
模拟重启清零接口已实现并验证，实际模拟重启控制流程尚未接入。
飞行日志保留原始文本及任务号，MAV 消息中文释义尚未实现；里程尚未加入 GPS 噪声过滤。
