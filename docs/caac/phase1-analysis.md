# CAAC 教学考试地面站：阶段一源码与架构分析

首次分析：2026-10-09；最近更新：2026-10-10（Asia/Shanghai）。目标平台：Windows 64 位。
本次交付为源码分析、设计和当前机器验证记录；没有实现 CAAC 功能。

**更新说明：** 环境与资料表已同步最新状态；第8节保留首轮历史日志。
后续 Qt/系统依赖处理见 [cloud-validation.md](cloud-validation.md)，
本轮问题处理和验证结果见 [phase1-issues.md](phase1-issues.md)，
视频画面与用户确认见 [reference-review.md](reference-review.md)。

## 1. 基线与开发环境

仓库：`zhaozhangyu396-creator/CAAC-QGC-GroundStation`。
分析基线：`b42d1d8fd3af14e56da03f9973ad771317b3126f`。
检查时，本地 HEAD、该仓库 origin/master 和官方
`https://github.com/mavlink/qgroundcontrol.git` master 指向同一 SHA。
这确认提交身份相同，不代表已验证稳定性。首轮checkout无版本标签，
`git describe --tags --always`返回裸SHA，打包版本回退0.0.0。
本轮从官方取得开发标记 `v5.2.0-dev`，校验解引用提交
`995174437e2d6bf0a9923e32b09db11925b3f9b6`且是基线祖先；
源码基线描述为 `v5.2.0-dev-142-gb42d1d8fd3`，属于开发版，不是官方稳定发布版。
开发基线固定此 SHA，已通过原版 Linux 构建、启动与任务规划回归。
这是已验证的开发提交，不是已验证的 Windows 发布版本。
官方存在 `v5.1.5` 发布标签，其解引用提交为
`3a67d31f0c36bf3fe38ec52970d250a89d0aaf67`，但本次未切换或验证该发布版。

阶段一分支：`phase1/caac-architecture`。沿用已有 checkout，没有创建 worktree。

| 项目 | 仓库要求 / 当前验证 |
| --- | --- |
| 语言 | QGC C++20；参考几何模块 C++17 |
| Qt | `.github/build-config.json` 指定 6.11.1，最低 6.11.0；已安装，恢复后版本查询与启动通过 |
| Qt 模块 | Graphs、Location及私有接口、Positioning、Multimedia、Quick3D、Sensors、Scxml、HttpServer 等；不能用系统旧 Qt 代替 |
| CMake | 最低 3.25；工具环境安装 3.31.10 |
| Ninja | 工具环境安装 1.13.0.git.kitware.jobserver-pipe-1 |
| 编译器 | 当前 Debian GCC/G++ 14.2.0，几何模块已实际编译 |
| Python | 锁定工具环境 Python 3.10.21，uv frozen 同步；pytest 9.1.1 |
| 工作流 | just 1.55.1；configure/build/test；CTest Unit/Integration，默认排除 Flaky/Network |
| GStreamer | Linux 最低 1.20.0；本机 1.26.2 开发依赖、编译和测试通过；Windows 配置指定 1.28.4 |
| 资源 | 初始磁盘可用约 29 GiB、内存可用约 31 GiB；后续首次构建并行建议 4，按实际峰值调整 |
| Windows | windows-2022、MSVC19.44.35229.0、Qt6.11.1、GStreamer1.28.4；原版x64 Debug编译与10项portable测试通过；安装包另记 |

不要将 `QGC_STABLE_BUILD=ON` 当成稳定发布验证；它是构建选项。
不应为了编译通过降级 Qt、关闭校验或修改版本锁。

## 2. 参考资料取得情况

| 资料 | 状态 |
| --- | --- |
| QGC 官方源码基线 | 当前 checkout 已检查，与官方 master SHA 一致 |
| CAAC_QGC_Geometry_V1 (2).zip | 已取得、解压、阅读全部 5 个文件，并执行原有编译测试 |
| 凌动地面站 2026-10-09 22-21-47.zip | 用户已上传；下载工具拒绝超过 32 MiB 的文件，尚未读取内容 |
| 凌动地面站 v2.5.0 安装程序 (1).exe | 用户已上传；同样超过工具限制，尚未读取或运行 |
| 约 8 分 49 秒操作视频 | 10 段均已取得，完成60帧采样；四轮关键行为确认含多步undo、无redo、仅.ld及可选桌面 |
| 航点.ld | 已取得并检查编码特征；未解析出航点，不能声称已支持导入 |

几何参考原件位于 `/workspace/reference/CAAC_QGC_Geometry_V1`。
当前对照依据用户需求、QGC源码、视频画面采样与明确标记的用户确认。
安装程序/大ZIP仍需可访问的解压资料或每份不超过32 MiB的独立文件；
这些资料不再阻塞已有视频的行为分析。
不提取或复制凌动的专有代码、图标和其他未授权资源。

## 3. 任务规划与航点源码定位

| 源码 | 职责 / 已确认接口 |
| --- | --- |
| `src/PlanView/PlanView.qml` | 规划页根组件；持有 PlanMasterController；地图 `onMapClicked` 按编辑层和添加模式决定行为；`insertSimpleItemAfterCurrent` 调用任务控制器 |
| `src/PlanView/PlanViewRightPanel.qml`、`PlanTreeView.qml` | 规划侧栏和任务树；树中删除操作经 `removeVisualItem` |
| `src/PlanView/SimpleItemEditor.qml`、`MissionItemEditor.qml` | 简单任务项属性及编辑器入口；使用 QGC Fact 控件 |
| `src/PlanView/SimpleItemMapVisual.qml`、`MissionItemMapVisualBase.qml` | 航点视觉、选择、拖动；拖动后更新 missionItem.coordinate |
| `src/FlightMap/MapItems/MissionItemIndicator.qml`、`MissionItemIndicatorDrag.qml` | 编号标记与拖动交互；右键编辑需沿实际事件链实现，不能只改一个全屏 MouseArea |
| `src/FlightMap/FlightMap.qml`、`src/GeoMap/GeoMap.qml` | 现有地图及另一套地图引擎；双击加点/左键拖图应分别验证，不假设同一处理器适用于两者 |
| `src/MissionManager/PlanMasterController.h/.cc` | mission/fence/rally 管理，`saveToJson`、`saveToFile`、`loadFromFile`、车辆同步 |
| `src/MissionManager/MissionController.h/.cc` | `visualItems`、`insertSimpleMissionItem`、`removeVisualItem`、`visualItemIndexForObject`、选中序号、任务平移和旋转、任务 JSON 保存加载 |
| `src/MissionManager/SimpleMissionItem.h/.cc`、`MissionItem.h/.cc` | MAVLink 任务命令、参数、坐标、高度框架等；航点并不只是经纬高三元组 |
| `src/MissionManager/VisualMissionItem.h`、`MissionSettingsItem.h` | 视觉任务项、Home/任务设置；视觉索引和 MAVLink sequence number 不能混用 |
| `src/PlanView/TransformEditor.qml` | 已有任务变换 UI，适合复用操作布局和交互习惯 |
| `src/API/QGCCorePlugin.h/.cc` | Custom Build 插件、QML 引擎创建、plan/mission JSON 钩子；`customMapItems` 文档明确用于 FlightMap，不应假定它直接扩展规划页 |
| `custom-example/CMakeLists.txt`、`cmake/CustomOverrides.cmake`、`src/CustomPlugin.h/.cc` | 官方定制构建示例，资源/QML 模块和 CUSTOMCLASS/CUSTOMHEADER 注册方式 |
| `test/MissionManager/MissionControllerTest.cc`、`PlanMasterControllerTest.cc` | 原任务控制与保存加载回归入口，两类均已实际运行通过 |

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

正式集成决策：V1仅作参考，不直接作为任务模型或正式计算后端。
复用QGC已有的 `QGCGeoMath` / GeographicLib 2.7，并在CAAC适配层验证输入和保留完整任务属性。
独立审查已量化球面误差、复现非有限模板属性和极大角度故障，详见
[phase1-issues.md](phase1-issues.md)。下列风险不因原有1项测试通过而消失：

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
| 航点新建 | 添加模式下地图点击插入 | 用户最终确认采用凌动习惯：空白处左键双击，无需添加模式；替代原单击要求 |
| 拖图、右键属性 | 地图手势、航点选择和拖动、现有属性控件 | 左键长按空白拖图、左键按住航点直接拖点、右键编辑；替代原中键要求 |
| 精确坐标/高度/速度、插删排序 | SimpleMissionItem/Fact、插入删除、任务树 | 坐标输入、速度命令适配、受约束排序 |
| 编号、连线和列表 | 标记、simpleFlightPathSegments、PlanTreeView | 复用已有模型，不建平行任务模型 |
| undo/redo | 未发现完整编辑事务接口 | 同一入口的原子事务和恢复；凌动有多步undo、无redo，CAAC仍保留原要求的两者 |
| 任意中心旋转及两点定向 | Home 点旋转、TransformEditor、WGS84计算接口 | 正角顺时针；任意中心固定，其余起飞/降落点移动；两点目标为真北顺时针方向 |
| 相对坐标插入 | 插入 API、独立几何参考 | 真北顺时针方位角或夹角、距离、选中点之后插入；前段朝北时夹角90°向西、180°继续朝北 |
| .caacroute 与 .plan | PlanMasterController JSON、文件 IO | 版本封装、验证、原子保存、兼容转换、桌面路径 |
| 考试计时、提交、检查、日志 | 任务规划和遥测基础 | 独立考试状态机、可配置规则、评分接口；规则不是官方标准 |
| 学员/教员、训练与成绩 | PX4/MAVLink 通信基础 | 培训管理、SITL 启动和记录；本次未运行 PX4 SITL |
| Windows 安装发布 | 已有 Windows CI/NSIS/安装验证脚本 | CAAC 标识、自有资源、依赖部署与实际 Windows 回归 |

## 6. 模块结构与数据原则

首选 `custom/` overlay，通过 QGC_CUSTOM_DIR 默认机制加载 CMakeLists 和 CustomOverrides。
保留原 PX4 工厂、MAVLink 和 Vehicle，不照搬示例中禁用工厂的选项。
Qt 6.11.1静态QML模块的URL拦截探针已在Debug/Release运行通过：
使用现有CustomPlugin的同类拦截方式，将 `/qml/...` 指向存在的 `/Custom/qml/...` 资源，
不存在覆盖资源时使用原资源。此结果确认接入机制，不代表完整PlanView已经接入或回归。

调用链：CAAC QML 编辑面板 → CaacRouteEditorController → 编辑事务/验证 →
QGC MissionController + SimpleMissionItem；纯 RouteGeometry 仅返回位置计算结果。
以 QGC 模型为唯一任务事实来源，几何层不持有 Vehicle、不发送 MAVLink。
计算与全量校验成功后一次提交，恢复通过统一事务入口执行，避免生成多个独立撤销记录。
断开连接、换 Vehicle、装载新计划、拖动取消必须有明确事务生命周期。

中心选航点时，该点经纬度原值保留；其余可编辑位置随变换移动，包含计划起飞/降落位置。
计划Home、起飞任务项和飞控实际Home需要明确映射；离线编辑不会隐式发出设置飞控Home的命令。
两点定向允许中心在A/B之外：WGS84上各点的子午线方向不同，不能仅计算
`目标方位角 − 原A→B方位角`后直接作为统一旋转角。
应求解变换后A→B满足目标方位角的旋转参数，复查最终结果；
重合点、无法定义方向或无解时整次操作拒绝，禁止部分修改任务。
夹角由基准点到前点的反向射线顺时针测量，支持负值；
只有起飞点、没有前段时拒绝夹角生成。方位角模式仍可使用明确的真北参照。

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
| `custom/src/PlanView/PlanViewRightPanel.qml` | 优先覆盖此小型入口，复用公开editorMap/planMasterController属性和编辑层信号；完整页面需另行验证 |
| `custom/src/PlanView/CaacRotationDialog.qml`、`CaacRelativeWaypointDialog.qml` | 三类中心、单点/两点定向、方位角/夹角及点后插入 |
| `custom/test/CMakeLists.txt`、`RouteGeometryTest.cc`、`RouteEditorTest.cc`、`RouteFileTest.cc` | 计算/事务/文件测试，Qt Test 或仓库 UnitTest；明确接入 CTest |
| `src/PlanView/PlanView.qml`、`src/PlanView/CMakeLists.txt` | 若资源覆盖无法可靠替换当前静态 QML 模块：加入默认不启用的最小 CAAC 面板/插件入口 |
| `src/FlightMap/MapItems/MissionItemIndicatorDrag.qml`、`src/PlanView/MissionItemMapVisualBase.qml` | 必要时暴露右键和拖动完成/取消事件；默认行为保持原样 |
| `src/MissionManager/MissionController.h/.cc` | 仅在公开接口不足时增加经过测试的批量编辑/受约束移动接口；不直接操作私有模型 |
| `test/MissionManager/MissionControllerTest.cc`、`PlanMasterControllerTest.cc` | 最小扩展接口的任务序号、保存加载、PX4 相关回归 |

现有 custom-example 用资源和URL拦截覆盖QML；静态模块拦截能力已用独立编译探针确认。
阶段二首先接入最小原生编辑入口，并回归完整PlanView、两套地图和鼠标事件。
如果完整页面的事件接入仍缺乏公共接口，再引入经过测试的小型通用入口；不承诺零核心修改。
不为考试功能复制完整 PlanView，也不新建独立 HTML 演示。

阶段二验收：离线 PX4 规划状态下创建/插删/拖动/精确属性修改，
保存重载 `.plan` 后字段与任务序号一致；原规划页和飞行页回归；
Windows x64 Debug/Release 构建、相关 Unit 测试和真实 QGC 界面鼠标操作。
后续几何、考试、教学、发布按独立分支和提交推进。

## 8. 首轮实际编译与测试记录（历史）

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

## 9. 当前状态与下一步

1. Qt下载、系统依赖、EGL链接、测试DNS等待配置已处理；Linux原版QGC已构建、启动和测试通过。
   新启动环境中的分支、提交、SDK、脚本和应用已核对，恢复后的重点检查6/6通过。
2. 几何风险已有可复现证据；正式计算选用QGC已有WGS84后端，V1不直接集成。
   输入校验和完整任务属性保留属于后续适配层实现，不能称已修复原件或完成编辑器。
3. 鼠标方案、夹角90°/180°/−90°、无前段拒绝、航点直接拖动和两点定向主要规则已由用户确认。
   多步undo、无redo、仅.ld和可选桌面也已确认；CAAC仍按原要求实现undo/redo与自有格式。
   明确覆盖此前冲突表述；用户确认与画面证据分别记录。
4. 用户启用Actions后，成功触发原版master的Windows Debug验证，run `37961938331`。
   三条分支编译成功，x64 portable测试10/10通过；无标签导致0.0.0打包校验失败。
   阶段一分支的Windows工作流新增官方开发标记获取、提交校验和祖先检查；
   已提交 `06138ce19e`并触发run `37965262779`复测；最终结果见问题处理报告。
   修复后的ARM64-cross整个任务及NSIS打包通过；其余复测仍在运行。
   不推送或创建发布标签，不修改应用/依赖版本或放宽打包校验。
5. 大附件仍未读取；`.ld`已分析编码但未解码。CAAC自有格式采用QGC plan JSON封装，
   不以破解`.ld`作为实现自有保存/加载的前置条件。

本轮保持阶段一：更新分析、补齐验证并提交结果；尚未修改应用编辑器。
Windows原版编译与本轮参考规则问题已按证据关闭；完整打包复测和其他遗留项单独保留。
