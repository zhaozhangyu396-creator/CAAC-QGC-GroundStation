# CAAC 教学考试地面站：阶段一源码与架构分析

日期：2026-10-09（Asia/Shanghai）。目标平台：Windows 64 位。
本次交付为源码分析、设计和当前机器验证记录；没有实现 CAAC 功能。

## 1. 基线与开发环境

仓库：`zhaozhangyu396-creator/CAAC-QGC-GroundStation`。
分析基线：`b42d1d8fd3af14e56da03f9973ad771317b3126f`。
检查时，本地 HEAD、该仓库 origin/master 和官方
`https://github.com/mavlink/qgroundcontrol.git` master 指向同一 SHA。
这确认提交身份相同，不代表已验证稳定性。当前 checkout 无可解析版本标签，
`git describe --tags --always` 返回 `b42d1d8fd3`；不得给它虚构发布版本。
建议阶段二继续固定此 SHA，先通过原版构建及任务规划回归，再评估正式发布基线。
官方存在 `v5.1.5` 发布标签，其解引用提交为
`3a67d31f0c36bf3fe38ec52970d250a89d0aaf67`，但本次未切换或验证该发布版。

阶段一分支：`phase1/caac-architecture`。沿用已有 checkout，没有创建 worktree。

| 项目 | 仓库要求 / 当前验证 |
| --- | --- |
| 语言 | QGC C++20；参考几何模块 C++17 |
| Qt | `.github/build-config.json` 指定 6.11.1，最低 6.11.0；尚未安装成功 |
| Qt 模块 | Graphs、Location及私有接口、Positioning、Multimedia、Quick3D、Sensors、Scxml、HttpServer 等；不能用系统旧 Qt 代替 |
| CMake | 最低 3.25；工具环境安装 3.31.10 |
| Ninja | 工具环境安装 1.13.0.git.kitware.jobserver-pipe-1 |
| 编译器 | 当前 Debian GCC/G++ 14.2.0，几何模块已实际编译 |
| Python | 锁定工具环境 Python 3.10.21，uv frozen 同步；pytest 9.1.1 |
| 工作流 | just 1.55.1；configure/build/test；CTest Unit/Integration，默认排除 Flaky/Network |
| GStreamer | Linux 最低 1.20.0；配置默认 1.28.4；本机仅检查到 1.26.2 运行库，开发依赖未验证 |
| 资源 | 初始磁盘可用约 29 GiB、内存可用约 31 GiB；后续首次构建并行建议 4，按实际峰值调整 |
| Windows | CI 使用 windows-2022、`win64_msvc2022_64`、Windows/Windows-debug preset、NSIS；当前没有 Windows 构建运行证据 |

不要将 `QGC_STABLE_BUILD=ON` 当成稳定发布验证；它是构建选项。
不应为了编译通过降级 Qt、关闭校验或修改版本锁。

## 2. 参考资料取得情况

| 资料 | 状态 |
| --- | --- |
| QGC 官方源码基线 | 当前 checkout 已检查，与官方 master SHA 一致 |
| CAAC_QGC_Geometry_V1 (2).zip | 已取得、解压、阅读全部 5 个文件，并执行原有编译测试 |
| 凌动地面站 2026-10-09 22-21-47.zip | 用户已上传；下载工具拒绝超过 32 MiB 的文件，尚未读取内容 |
| 凌动地面站 v2.5.0 安装程序 (1).exe | 用户已上传；同样超过工具限制，尚未读取或运行 |
| 约 8 分 49 秒操作视频 | 工作目录未取得；无法确认是否位于未能读取的 ZIP 中 |
| 航点.ld | 工作目录未取得；格式、属性与兼容性尚未分析 |

几何参考原件位于 `/workspace/reference/CAAC_QGC_Geometry_V1`。
当前功能对照仅依据用户需求和 QGC 源码，不是已完成的凌动实测对照。
继续参考分析需要可访问的解压资料，或每份不超过 32 MiB 的独立文件。
不提取或复制凌动的专有代码、图标和其他未授权资源。

## 3. 任务规划与航点源码定位

| 源码 | 职责 / 已确认接口 |
| --- | --- |
| `src/PlanView/PlanView.qml` | 规划页根组件；持有 PlanMasterController；地图 `onMapClicked` 按编辑层和添加模式决定行为；`insertSimpleItemAfterCurrent` 调用任务控制器 |
| `src/PlanView/PlanViewRightPanel.qml`、`PlanTreeView.qml` | 规划侧栏和任务树；树中删除操作经 `removeVisualItem` |
| `src/PlanView/SimpleItemEditor.qml`、`MissionItemEditor.qml` | 简单任务项属性及编辑器入口；使用 QGC Fact 控件 |
| `src/PlanView/SimpleItemMapVisual.qml`、`MissionItemMapVisualBase.qml` | 航点视觉、选择、拖动；拖动后更新 missionItem.coordinate |
| `src/FlightMap/MapItems/MissionItemIndicator.qml`、`MissionItemIndicatorDrag.qml` | 编号标记与拖动交互；右键编辑需沿实际事件链实现，不能只改一个全屏 MouseArea |
| `src/FlightMap/FlightMap.qml`、`src/GeoMap/GeoMap.qml` | 现有地图及另一套地图引擎；中键交互应分别验证，不假设同一处理器适用于两者 |
| `src/MissionManager/PlanMasterController.h/.cc` | mission/fence/rally 管理，`saveToJson`、`saveToFile`、`loadFromFile`、车辆同步 |
| `src/MissionManager/MissionController.h/.cc` | `visualItems`、`insertSimpleMissionItem`、`removeVisualItem`、`visualItemIndexForObject`、选中序号、任务平移和旋转、任务 JSON 保存加载 |
| `src/MissionManager/SimpleMissionItem.h/.cc`、`MissionItem.h/.cc` | MAVLink 任务命令、参数、坐标、高度框架等；航点并不只是经纬高三元组 |
| `src/MissionManager/VisualMissionItem.h`、`MissionSettingsItem.h` | 视觉任务项、Home/任务设置；视觉索引和 MAVLink sequence number 不能混用 |
| `src/PlanView/TransformEditor.qml` | 已有任务变换 UI，适合复用操作布局和交互习惯 |
| `src/API/QGCCorePlugin.h/.cc` | Custom Build 插件、QML 引擎创建、plan/mission JSON 钩子；`customMapItems` 文档明确用于 FlightMap，不应假定它直接扩展规划页 |
| `custom-example/CMakeLists.txt`、`cmake/CustomOverrides.cmake`、`src/CustomPlugin.h/.cc` | 官方定制构建示例，资源/QML 模块和 CUSTOMCLASS/CUSTOMHEADER 注册方式 |
| `test/MissionManager/MissionControllerTest.cc`、`PlanMasterControllerTest.cc` | 原任务控制与保存加载回归测试入口，尚未运行 |

现有 `MissionController::rotateMission` 围绕 planned Home，使用 QGeoCoordinate
distanceTo/azimuthTo/atDistanceAndAzimuth。默认不转起飞和降落点，跳过 standalone coordinate，
复杂项仅移动参考坐标，不旋转其几何。因此它不能直接满足任意中心、所有相关航点、
复杂项和完整属性保留要求。高度和高度框架必须针对 SimpleMissionItem 的真实 setter 行为测试，
不能仅凭函数名认定保留。

没有在上述控制器接口中发现可直接复用的完整 undo/redo 事务接口；需要新增编辑事务层。
排序也不能直接重排 visualItems 后就认定完成：必须保持序号重算、DO_JUMP 引用、
速度命令、起飞/降落及复杂任务约束。

## 4. 几何模块审查

接口为 `destination`、`rotate`、`insertRelative`，数据为 Coordinate 和 Waypoint。
无 Qt、无 MAVLink 依赖，适合保持计算与 UI 分离。旋转复制输入 vector，保留高度、speed、command，
中心距小于 1 微米时保留坐标；相对插入高度来自模板而非基准点。
它已验证有限数值、经纬范围、负距离和插入索引，并规范输出经度。

正式集成前需处理：

1. 平均半径 6371008.8 m 的球面算法不是 WGS84 椭球测地线。建议 GeographicLib
   椭球 inverse/direct，并用独立已知数据验证；角度定义固定为真北顺时针，旋转正角顺时针。
   椭球极坐标旋转保证中心距离/方位角关系，不等同于保持所有航段长度的平面刚体旋转，
   必须在功能说明中明确采用哪种定义。
2. 方位角和旋转角在三角函数计算前没有归一化。极大有限角度有数值精度风险；
   极点、近对跖、跨日期变更线和长距离没有测试。
3. destination 的 asin 输入没有舍入钳位；polar 已钳位 haversine。
   需要边界测试确定实际风险，不能称已有复现故障。
4. insertRelative 未验证模板 altitude/speed/command，rotate 也未验证 speed/command；
   插入可以返回带非有限模板高度的数据。任务语义有效性应由适配层独立检查。
5. Coordinate.altitude 强制有限，而 QGeoCoordinate 的二维位置允许未定义高度。
   中心选择不应无意要求高度；必须分离二维中心与任务高度/参考框架。
6. Waypoint 没有 MAV_FRAME、param1..7、autoContinue、复杂项内容、稳定 ID、速度命令语义。
   不能把它直接当作 QGC 任务保存格式，否则会丢失属性。
7. 测试为单个 main 中的 assert；Release 定义 NDEBUG 时断言不执行。
   本次特意以 Debug 执行，1/1 通过只覆盖原有局部用例，不能证明考试精度。
8. 尚无许可证文件。用户提供代码可用于审查；正式发布前记录来源并确认许可，
   不擅自给参考代码添加开源许可。

应新增数据驱动用例：零距离/零角、±90/180/360、多轮往返、精确中心固定、
跨日期线、高纬和极点、接近对跖、无效/NaN/Inf 输入、插入边界、
高度/速度/全部 MAVLink 参数/任务顺序不变、失败时不部分修改。
精度阈值必须基于用途和独立基准确定，当前没有正式考试阈值依据。

## 5. 功能对照与差距

| 用户需求 | 当前 QGC 可复用能力 | CAAC 扩展 |
| --- | --- | --- |
| 左键空白新建 | 添加模式下地图点击插入 | CAAC 模式事件优先级，避开选中心/拖动/弹窗 |
| 中键拖图、右键属性 | 地图手势、航点选择和拖动、现有属性控件 | 两套地图的鼠标策略、右键属性弹窗 |
| 精确坐标/高度/速度、插删排序 | SimpleMissionItem/Fact、插入删除、任务树 | 坐标输入、速度命令适配、受约束排序 |
| 编号、连线和列表 | 标记、simpleFlightPathSegments、PlanTreeView | 复用已有模型，不建平行任务模型 |
| undo/redo | 未发现完整编辑事务接口 | 同一编辑入口的原子事务及状态恢复 |
| 任意中心旋转 | Home 点旋转、TransformEditor | 三种中心选择、椭球计算、属性保留和预览/提交 |
| 相对坐标插入 | 插入 API、独立几何参考 | 基准/方位角/距离/插入位置 UI 与任务适配 |
| .caacroute 与 .plan | PlanMasterController JSON、文件 IO | 版本封装、验证、原子保存、兼容转换、桌面路径 |
| 考试计时、提交、检查、日志 | 任务规划和遥测基础 | 独立考试状态机、可配置规则、评分接口；规则不是官方标准 |
| 学员/教员、训练与成绩 | PX4/MAVLink 通信基础 | 培训管理、SITL 启动和记录；本次未运行 PX4 SITL |
| Windows 安装发布 | 已有 Windows CI/NSIS/安装验证脚本 | CAAC 标识、自有资源、依赖部署与实际 Windows 回归 |

## 6. 模块结构与数据原则

首选 `custom/` overlay，通过 QGC_CUSTOM_DIR 默认机制加载 CMakeLists 和 CustomOverrides。
保留原 PX4 工厂、MAVLink 和 Vehicle，不照搬示例中禁用工厂的选项。

调用链：CAAC QML 编辑面板 → CaacRouteEditorController → 编辑事务/验证 →
QGC MissionController + SimpleMissionItem；纯 RouteGeometry 仅返回位置计算结果。
以 QGC 模型为唯一任务事实来源，几何层不持有 Vehicle、不发送 MAVLink。
计算与全量校验成功后一次提交，恢复通过统一事务入口执行，避免生成多个独立撤销记录。
断开连接、换 Vehicle、装载新计划、拖动取消必须有明确事务生命周期。

文件建议为 JSON `.caacroute`，含 fileType、schemaVersion、完整 qgcPlan、
CAAC 元数据（稳定航点 ID/旋转中心选择方式/经纬度/操作配置）。
不要只存 Waypoint 数组，也不要将 speed 字段简单等同于 QGC 的速度命令。
旋转中心为航点时引用稳定 ID，并同时保存必要坐标；排序/删除后的引用必须验证。
未知版本和无效数据拒绝加载且保留旧任务；字段类型、范围、文件大小/项数、
命令/高度框架和非有限数值检查在改变模型前完成。
保存使用 QSaveFile，Windows 桌面用 QStandardPaths::DesktopLocation，
不能拼接 `%USERPROFILE%/Desktop`，以兼容重定向桌面。
阶段二先复用 `.plan` 保存加载；`.caacroute` 的完整兼容转换作为明确后续里程碑。
不自动上传、解锁或起飞；考试编辑与飞行执行保持独立用户动作。

## 7. 拟新增和修改文件

以下是实施计划，不代表文件已经实现。

| 文件 | 计划 |
| --- | --- |
| `custom/CMakeLists.txt`、`custom/cmake/CustomOverrides.cmake` | 定制模块、原创品牌、测试接入；保留 PX4 通信 |
| `custom/src/CaacCorePlugin.h/.cc` | 插件注册、CAAC 属性/设置、plan JSON 元数据钩子 |
| `custom/src/Route/CaacRouteEditorController.h/.cc` | 当前 PlanMasterController 绑定、插删/排序/属性验证、事务入口 |
| `custom/src/Route/CaacRouteGeometry.h/.cc` | 椭球计算适配和几何结果；不包含地图/车辆 |
| `custom/src/Route/CaacRouteEditCommand.h/.cc` | 可恢复快照/命令，坐标和完整属性保留 |
| `custom/src/Route/CaacRouteFile.h/.cc` | .caacroute 版本验证、QGC JSON 封装和原子 IO |
| `custom/src/PlanView/CaacRouteEditor.qml`、`CaacWaypointDialog.qml` | 编辑入口与精确属性弹窗，复用 QGC 控件 |
| `custom/src/PlanView/CaacRotationDialog.qml`、`CaacRelativeWaypointDialog.qml` | 三类中心选择、相对坐标和插入位置 |
| `custom/test/CMakeLists.txt`、`RouteGeometryTest.cc`、`RouteEditorTest.cc`、`RouteFileTest.cc` | 计算/事务/文件测试，Qt Test 或仓库 UnitTest；明确接入 CTest |
| `src/PlanView/PlanView.qml`、`src/PlanView/CMakeLists.txt` | 若资源覆盖无法可靠替换当前静态 QML 模块：加入默认不启用的最小 CAAC 面板/插件入口 |
| `src/FlightMap/MapItems/MissionItemIndicatorDrag.qml`、`src/PlanView/MissionItemMapVisualBase.qml` | 必要时暴露右键和拖动完成/取消事件；默认行为保持原样 |
| `src/MissionManager/MissionController.h/.cc` | 仅在公开接口不足时增加经过测试的批量编辑/受约束移动接口；不直接操作私有模型 |
| `test/MissionManager/MissionControllerTest.cc`、`PlanMasterControllerTest.cc` | 最小扩展接口的任务序号、保存加载、PX4 相关回归 |

现有 custom-example 用资源添加覆盖部分 QML，但静态 QML 模块中的 PlanView 替换
是否可靠需要实测；不先承诺零核心修改。首先做最小 Custom Build 编译探针，
验证模块加载和现有地图交互，然后选择 overlay 或小型通用入口。
不为考试功能复制完整 PlanView，也不新建独立 HTML 演示。

阶段二验收：离线 PX4 规划状态下创建/插删/拖动/精确属性修改，
保存重载 `.plan` 后字段与任务序号一致；原规划页和飞行页回归；
Windows x64 Debug/Release 构建、相关 Unit 测试和真实 QGC 界面鼠标操作。
后续几何、考试、教学、发布按独立分支和提交推进。

## 8. 实际编译与测试记录

几何模块（原件未改动，Debug 确保 assert 生效）：

```sh
QGC=/workspace/CAAC-QGC-GroundStation
"$QGC/tools/.venv/bin/cmake" -S /workspace/reference/CAAC_QGC_Geometry_V1 \
  -B /workspace/build/geometry -DCMAKE_BUILD_TYPE=Debug
"$QGC/tools/.venv/bin/cmake" --build /workspace/build/geometry --parallel 2
"$QGC/tools/.venv/bin/ctest" --test-dir /workspace/build/geometry --output-on-failure
```

结果：静态库和 test_route_geometry 编译成功；route_geometry **1/1 通过**。

QGC 开发工具：

```sh
cd /workspace/CAAC-QGC-GroundStation
export UV_CACHE_DIR=/workspace/.cache/uv
export UV_PYTHON_INSTALL_DIR=/workspace/.local/python
python3 tools/setup/install_python.py dev
cd tools
.venv/bin/python -m pytest tests/ -q
```

结果：**1021 通过、14 跳过**。首次运行因未指定 uv Python 下载路径失败 1 项；
修正运行环境后完整重跑通过，没有修改测试或断言。跳过项没有算作通过。

原版 QGC（尚未进行 CAAC 源码修改）：

```sh
cd /workspace/CAAC-QGC-GroundStation
tools/.venv/bin/cmake -S . -B /workspace/build/qgc-baseline -G Ninja \
  -DCMAKE_MAKE_PROGRAM=/workspace/CAAC-QGC-GroundStation/tools/.venv/bin/ninja \
  -DCMAKE_BUILD_TYPE=Debug -DQGC_BUILD_INSTALLER=OFF -DQGC_USE_CACHE=OFF
```

结果：C/C++ 编译器探测成功；配置在根 CMakeLists.txt:243 失败：
`Qt6 6.11.0+ not found`。**没有进入应用编译，应用 Unit/Integration、启动、
GUI/PX4 通信、Windows 编译和安装包验证均未执行，不能报告通过。**
`QGC_BUILD_INSTALLER=OFF` 只用于 Linux 开发构建，不构成最终发布验证。

## 9. 当前阻塞与下一步

1. Qt 下载：实际 HTTPS 请求到 download.qt.io 被代理拒绝，CONNECT 403。
   已保存该域名到环境配置草稿，保留平台 package_managers 预设。
   草稿保存后的复查仍为 403，因此尚未生效；不能称 SDK 已安装。
   在环境设置审阅并保存网络配置后重试，再根据实际重定向添加必要镜像域名，
   不开放任意全网、不跳过 TLS 或 checksum。
2. 系统依赖：账户为 agent，没有 sudo；apt-get update 在申请提权执行后仍因
   /var/lib/apt 权限失败。此为操作系统权限限制，不是源码错误，也不是自动审批拒绝。
   需要平台提供可安装依赖的基础镜像/管理员安装路径，或预装仓库 Debian 构建依赖。
   不能靠再次批准相同命令取得 root。
3. 参考大附件：工具传输上限 32 MiB；需可读取的解压或较小文件。
4. Windows：需原生 Windows x64 开发/CI 环境验证 MSVC、Qt、GStreamer、NSIS、安装升级卸载。
   Linux 几何测试不能替代 Windows 应用测试。

当前只完成了工具开发环境和可取得资料的阶段一分析；整体 QGC 环境尚未就绪。
待上述基础依赖可用，继续安装 Qt → 原版 configure/build → CTest → 可验证启动；
之后才实施阶段二。报告提交不包含任何已完成航点编辑器的声明。
