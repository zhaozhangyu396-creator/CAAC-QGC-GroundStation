# 兴趣区域窗口切换与高级航点编辑验证

## 问题与修复

用户报告开启“兴趣区域”后切换到其他软件闪退，提供的 3.03 秒录屏显示规划页和兴趣区域入口，未包含软件退出过程。
在原生 Qt 6.11.1 Xcb 窗口测试中，开启兴趣区域、切换窗口、返回并点击地图可以稳定触发 SIGSEGV。
中英文测试均在同一操作处崩溃，调用栈指向 Qt Quick 的鼠标释放处理。

兴趣点插入关闭 ROI 模式并恢复 CAAC 航点编辑，原生 `MultiPointTouchArea.mouseEnabled` 随之关闭。
Qt 的 `setMouseEnabled(false)` 清空内部鼠标触点，而 `mouseReleaseEvent()` 在释放信号返回后仍使用该触点。
`FlightMap.qml` 现在在事件结束后发送地图点击信号，避免在 Qt 正在处理释放时切换鼠标接收区域。
取消触摸时还会停止长按定时器、清除按压状态并结束地图平移。

`CaacRouteEditorUITest::_roiWindowFocus` 在 Windows 和 Linux Xcb 中启动独立的原生窗口程序，重复三次开启 ROI、
应用失焦与重新激活、关闭 ROI，随后放置兴趣点并再次切换，检查应用状态、主窗口、任务数量和 ROI 状态。
辅助程序通过本地套接字接收返回请求，从前台程序激活原窗口；Windows 使用原生前台窗口接口，Xcb 使用当前时间的原生焦点请求。
辅助程序仅用于测试，不安装到软件中。其他原生平台保留第二窗口回归，尚未验证外部程序切换。
这不等同于手动覆盖所有外部软件、输入设备和 Windows DPI 设置。

## 本地验证

- 修复前：中英文原生界面均在 ROI 放置阶段 SIGSEGV；记录 `caac-roi-focus-console.log`。
- 修复后：规划模型、模板几何、中英文界面共 4/4 通过，25.76 秒；记录 `caac-roi-fixed.xml`。
- 全部单元测试：344/344 通过，53.76 秒；记录 `caac-advanced-unit.xml`。
- 新鲜源码翻译审计：6594 条消息、30 个技术标识、1 条空来源、0 个错误。
- 原生 Xcb／Mesa 125% 缩放：页面、工具栏、参数弹窗、飞行地图及 CAAC 界面 8/8 通过，153.04 秒；记录 `caac-advanced-ui-125.xml`。
- 度分秒输入宽度修正后，125% 中英文界面 2/2 通过，19.90 秒，检查实际字体宽度与输入框内边距，保留 ROI 窗口切换测试；记录 `caac-coordinate-layout-125.xml`。
- 改为独立程序切换后，125% 中英文界面 2/2 通过，47.24 秒；检查实际应用失焦及重新激活，记录 `caac-external-native-focus-125.xml`。
- 测试辅助程序限定为 Windows／Linux、保留其他平台原有窗口回归后，125% 中英文最终检查 2/2 通过，45.73 秒；记录 `caac-external-focus-complete-125.xml`。其余平台不计为外部程序切换验收。
- 启动检查和本批变更静态检查通过。度分秒截图已检查数字与单位完整显示，记录 `/workspace/caac-advanced-waypoint.png`。
- Windows 验证结果在完成后补充。Linux 缩放验证不代替用户 Windows 手动 DPI 验收。

证据位于 `/workspace` 和 `/workspace/build/qgc-baseline`。此处记录实际结果，未运行的检查不计为通过。

## 同包高级编辑

新增十进制度／度分秒切换，转换保持未编辑坐标原始精度；支持南北纬、东西经和负零，拒绝非法度分秒及边界外输入。
普通 `MAV_CMD_NAV_WAYPOINT` 可编辑停留秒数与指定朝向，使用原生任务 Facts，保留其余参数及命令。
坐标、高度、速度、停留和朝向作为一次事务；无效字段拒绝全部变更；保存 `.plan`、加载、撤销及重做保持一致。
原生界面测试覆盖格式切换不修改任务、非法分钟阻止保存、合法度分秒及朝向写入和撤销。

这不代表定点／协调／自适应转弯、每航点垂直速度、爬升模式、闭合循环、任务结束行为、模拟应急或 UOM 接入已完成。
这些功能仍按实施清单继续开发；现有界面与 Mock 测试不代替 SITL 或真实飞行验证。

## Windows 构建

先前源 `33d1521378` 的 Release x64 构建、安装升级、排除 Qt/GStreamer 构建 SDK 后启动和卸载成功，但该包没有本次崩溃修复和高级编辑。
同源 Debug 检查因模板几何的每组数据反复启动任务控制器而超出 60 秒；其他六类检查通过。
几何测试现独立于任务集成测试，仍保留所有数据和断言，不增加超时或跳过检查。
新版 Windows 验证必须对应包含本次修复的源码提交；最终身份和下载链接在完成后记录。

源 `28fe9574251ad3f67aa762e92796c1304522db74` 的 Windows x64 Debug 原生检查已完成：8/8 通过，0 失败、0 跳过，189 秒。
运行 [38148539049](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38148539049)，
x64 作业 `114496982614`。结果包含两种界面的 ROI 原生窗口切换、度分秒编辑、任务模型、模板几何、Fact 所有权及飞行数据检查。
本地结果保存于 `/workspace/caac-28fe-debug-results/junit-caac.xml`。
同源 Release 运行 `38149545727` 的 x64 作业 `114499995825` 已成功：Portable 10/10，编译后启动、实际 NSIS 中文审计、
安装校验值、安装与升级、移除 Qt/GStreamer 构建 SDK 路径后的安装版启动、卸载及清理均通过。
两份包内中文审计均为 6594 条消息、30 个技术标识、1 条空来源、0 错误。

## 新版安装包

[下载 Windows x64 Release 验证包](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38149545727/artifacts/11706493935)。
登录 GitHub 下载 ZIP，解压后运行 `build/QGroundControl-installer-AMD64.exe`。
Artifact `11706493935`，名称 `caac-validation-amd64-Release`，过期时间 2026-10-25 15:19:50 UTC。
该包的应用源码为 `28fe9574251ad3f67aa762e92796c1304522db74`，由工作流完整 HEAD 确认。

| 文件 | 字节数 | SHA256 |
| --- | --- | --- |
| 下载 ZIP | 135232537 | `17ca54f2acbd777a0a094562ff589e6b0dd59b2970cf070ac3a20681673bf551` |
| 安装 EXE | 135241996 | `0d8c366509696b3b8b48355a5c96eac58315bb6c4da9026c6b2981e23c5acb38` |

ZIP 与 GitHub API digest 一致，EXE 与包内 `.sha256` 一致。
本地下载与审计记录为 `/workspace/caac-28fe-release-*`，Portable 结果为 `/workspace/caac-28fe-release-portable/junit-portable.xml`。
该包包含已完成的凌动布局、航点与模板编辑、窗口拖动、航段标注、菜单字体与中文修正、本次 ROI 修复和高级参数编辑，
仍有“同包高级编辑”部分列出的高级功能未完成，不称为完整凌动等效软件。

测试辅助程序与外部软件切换回归随后提交于 `b07019bc214db501ce4df841e27f980511a8dcb7`，未改变安装包的应用行为。
其独立 Windows Debug 验证运行 `38150862151` 尚未完成，不将它与上述安装包的 8/8 原生结果混为同一源码验收。
