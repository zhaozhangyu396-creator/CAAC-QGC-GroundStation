# 阶段三：航线几何编辑与中文界面

更新日期：2026-10-11（Asia/Shanghai）。分支：`phase3/caac-route-geometry`。
继承阶段二 `79046aac004bca22c60157e4a3a0e4d756c0f7a9`，沿用已验证的 QGC 开发基线。
本报告记录实际实现和验证；Windows 产物信息在验证完成后更新。

## 功能和交互

规划页原生功能区改为“CAAC 航线工作台”，保留原任务树、地图、文件工具、PX4/MAVLink 及任务控制器。
新增代码使用 C++ / QML 和官方 Custom Build 扩展；没有 HTML 页面，也没有复制凌动专有代码或资源。

| 操作 | 实际行为 |
| --- | --- |
| 旋转 / 定向 | 任意正负角度，正角度顺时针；旋转中心可选起飞点或航点、地图空白处、精确经纬度 |
| 两点定向 | 选择 A、B 和独立旋转中心，将旋转后的 A→B 对准目标真北方位角 |
| 相对坐标 | 真北 0°、顺时针增加；输入米制距离，插入指定航点之后 |
| 夹角 | 使用基准点指向前一坐标点的反向方位角加夹角；允许负角，没有前段时拒绝生成 |
| 撤销 / 重做 | 最多 100 次编辑；一次拖点按一个操作记录；撤销后新编辑清除重做分支 |
| 保存 / 加载 | 复用 QGC `.plan`；可选 CAAC 元数据保存旋转中心，经纬度不舍入；原版 `.plan` 仍可加载 |

夹角示例：前一航段向北，夹角 +90° 生成西侧点，180° 继续向北，−90° 生成东侧点。
旋转保持高度、速度、任务命令、坐标框架、非坐标参数和顺序。作为中心的点经纬度保持完全一致；
起飞点、降落点不是中心时随航线移动，遵循用户最终确认。

包含测绘、降落图案等复杂任务项时，整条旋转在提交前拒绝，避免只修改部分航线。
包含 DO_JUMP 时仍限制插入、删除和排序，沿用阶段二保护；旋转不改变顺序。
这些限制在界面显示中文原因，不转换或删除用户任务。

## 几何和历史结构

`CaacRouteGeometry` 复用 QGC 的 GeographicLib WGS84 椭球测地线计算，与界面及任务模型分离。
旋转保持各点到中心的椭球距离，改变中心出发方位角；不宣称任意全球尺度旋转是椭球上的刚体等距变换。
两点定向求解旋转后的实际测地方位角，不简单把原方位角差当作最终旋转角。
所有输入和目标坐标先验证，失败不提交部分变更。

历史保存原 MissionController 的任务 JSON、选择位置和旋转中心；恢复使用原加载接口。
围栏和集结点不纳入任务撤销，不会被任务撤销覆盖；不会自动上传任务或控制飞行器。
原生 Fact、航点拖动和属性修改接入事务，保留单一任务数据来源。

## 中文覆盖

CAAC 构建未显式选择语言时默认简体中文；原版 QGC 保留系统语言行为。
已完成当前源码及 JSON 元数据登记的 6,440 条非空文本，另有 1 条空来源文本。
28 条标准协议、产品名、格式、遥测符号或内部诊断字段按清单保留；没有未完成译文、变量占位符或枚举分隔符错误。
QGC 启动、菜单、任务、参数元数据、设置、开发工具和新增功能使用中文；标准名称 PX4、MAVLink 等保留。
地图显示名称单独翻译，存储键和地图下载标识保持原值。
安装和卸载界面选择简体中文，显卡兼容启动快捷方式也使用中文名称。

初稿使用公开源码文本的机器辅助翻译，再修订飞行、解锁/上锁、复飞、导航、云台和定位术语。
覆盖检查不等于逐条专业语言验收；飞控传来的动态消息、设备自报名称、第三方底图地名和原始协议日志
不受本地翻译目录控制。后续人工验收发现的表述问题应持续修订。
审计不会修改协议键、MAVLink 命令值或参数名。

## 新增和修改文件

新增：

- `custom/src/Route/CaacRouteGeometry.h/.cc`：独立 WGS84 几何。
- `custom/src/PlanView/CaacGeometryDialog.qml`：旋转、定向和相对坐标弹窗。
- `custom/tools/check_chinese_translations.py`、`chinese-identifiers.json`：中文覆盖及格式审计。
- `src/QmlControls/MapNames.qml`：地图名称本地化显示。

主要修改：

- `custom/src/Route/CaacRouteEditorController.h/.cc`、`CaacCorePlugin.h/.cc`：操作事务、历史、文件元数据和中文默认语言。
- `custom/src/PlanView/CaacRouteEditor.qml`、`custom/CMakeLists.txt`：原生工作台及组件注册。
- `custom/test/`：几何精度、属性不变、历史、真实 UI、中文启动及地图键兼容测试。
- `src/API/QGCCorePlugin.h`、`src/QGCApplication.cc`：插件默认语言及测试启动语言选择。
- `src/FlightMap/FlightMap.qml`、`src/PlanView/MissionItemMapVisualBase.qml`：拖点事务信号。
- `src/QmlControls/{QGCComboBox,LabelledComboBox}.qml`、`CMakeLists.txt`：独立显示转换，不改变配置值。
- 两份 `translations/*_zh_CN.ts`、语言列表及少量此前未标记的 QML/C++ 显示文本。
- `cmake/install/CPack/CreateCPackNSIS.cmake`、Windows 工作流及安装验证：中文安装、卸载及快捷方式。

完整修改清单以本阶段 Git 提交为准；部分已有 CMake 文件按仓库强制格式检查整理。

## 可复现命令和当前结果

Linux 使用 Qt 6.11.1、GCC 14.2、CMake 3.31.10、Ninja 和工作区 GStreamer sysroot。
无需管理员安装，也未把云端 Linux 编译当作 Windows 编译结果。

```bash
source /workspace/qgc-cloud/activate.sh
cmake --build /workspace/build/qgc-baseline --parallel 4
cd /workspace/build/qgc-baseline
CI=1 QGC_TEST_VERBOSE=1 python /workspace/CAAC-QGC-GroundStation/.github/scripts/cmake_helper.py ctest \
  --build-type Debug --jobs 1 --tests-regex '^Caac(RouteEditor(Test|UITest)|ChineseUITest)$' \
  --include-labels 'Unit|Integration' --exclude-labels 'Flaky|Network' \
  --junit-output phase3-editor-final.xml --ctest-output phase3-editor-final.txt
```

专项 3/3 通过，8.39 秒。覆盖赤道、高纬、日期变更线、正负角、多圈角、独立 WGS84 基准值、
逆旋转、属性及任务 JSON 保留、多步历史、非法输入原子性、文件往返和真实中英文 UI。
样例精度断言：100 米中心距离及逆旋转误差小于 1 微米，方位误差小于 `1e-7` 度；
两点定向通过旋转后的实际方位验证。此为测试样例结果，不是全球任意任务的精度承诺。

中文审计命令：`python custom/tools/check_chinese_translations.py`；
加 `--source-ts` 可检查最新 Qt/JSON 提取目录的覆盖，加 `--output` 保存 JSON 证据。
打包相关 Python 测试、完整回归、启动和 Windows Debug / Release 的最终结果待验证后补充。

### 验证发现和修复

首轮完整 Linux 回归为 424/426，487.47 秒，未标记全部通过。
PX4 / ArduPilot 配置 UI 的语言切换测试将中文重启提示与固定英文匹配，导致两项失败。
`VehicleConfigUITestBase.cc` 改为使用实际提示来源 `Fact` 的当前语言文本，并转义正则；
仍要求真实弹窗出现、准确消息被捕获以及页面内容随语言变化。
修复后配置 UI 和 CAAC 三类专项共 5/5 通过，59.40 秒。

初次 Windows Debug（`38058082140`）的模型及英文 UI 通过；中文 UI 完成全部编辑操作，
随后关闭窗口的正常异步清理信息未匹配已有英文规则，严格日志检查失败。
通过官方 Qt 中文目录确定对应 `QQmlComponent` 消息，`QmlUITestBase.cc` 的既有关闭规则
精确识别该同一消息的当前译文和英文；没有添加一般错误过滤或修改编辑断言。

初次 Windows Release（`38058138103`）编译、Portable 10/10（1.13 秒）、启动、安装、升级、
移除 SDK PATH 后启动和卸载均通过；但复核产物中的最终 `project.nsi` 发现中文 GUI 回调缺失。
因此该安装包没有作为完成汉化的交付版本。
CPack 在组装组件时覆盖 `CPACK_NSIS_DEFINES`，部署过程也可能改变模块搜索路径。
修复改为将回调写入派生模板，并在打包前恢复模板搜索路径；保留 CPack 生成的组件定义。
新增使用真实 CPack 和编译器桩的生成测试，验证部署路径被改变后最终安装脚本仍包含中文回调、
中文安装提示及运行文件组件；桩输出明确不是安装程序，不当作 Windows 编译证据。
Windows 工作流进一步检查真正编译过的 `project.nsi`，不再只检查配置变量。
`--nsis-script` 可独立复核该产物；审计结果随安装包归档。

## 遗留事项

考试倒计时、`.caacroute`、考生提交、考官检查及可配置规则进入阶段四；课程、成绩管理进入阶段五。
Windows 验证版不等于阶段六的正式品牌、签名、长期发布和实机验收版本。
凌动 `.ld` 仍未解码；未知许可证的 Geometry V1 不直接纳入发行代码。
自动测试使用 QGC 测试瓦片占位图，截图不作为在线地图下载验收证据。
地图选中心和两点定向弹窗需要补充 Windows 人工操作验收；核心计算和模型已经自动测试。
