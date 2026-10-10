# 阶段二：QGC 原生航点编辑器

日期：2026-10-10（Asia/Shanghai）。分支：`phase2/caac-waypoint-editor`。
源码沿用阶段一验证的 QGC 开发基线；这是开发验证版本，不是正式 Windows 发布版。

## 实现与操作

`custom/` 通过官方 Custom Build 机制加载 `CaacCorePlugin` 和 `CAAC.Plan` QML 模块。
插件保留 QGC 原有选项、PX4/ArduPilot 工厂、MAVLink、Vehicle、视频及任务模型。
新增界面显示在规划页任务树上方；原任务树、编号、连线、起飞/降落工具和文件工具继续使用。

| 操作 | 行为 |
| --- | --- |
| 地图空白处左键双击 | 在当前选中任务项之后插入普通航点，无需添加模式 |
| 空白处左键按住拖动 | 平移地图；拖动不创建航点 |
| 航点左键按住拖动 | 直接拖动可见的简单航点，不要求先选中；保留高度 |
| 航点右键单击 / Edit | 打开精确经纬度、高度、速度属性弹窗 |
| 属性 OK / Cancel | OK 全量检查后提交；Cancel 保留原值；经纬度不会在打开弹窗时舍入 |
| Up / Down | 普通航点跨相邻普通航点调整顺序，速度/相机附属命令随任务项保留 |
| Delete | 删除当前普通航点；任务序号与列表经 MissionController 重新计算 |
| 原 Plan File 工具 | 使用完整 QGC `.plan` 保存、选择桌面路径及加载，保留 fence/rally 等原生内容 |

高度字段始终用米，保留当前高度框架；弹窗同时显示该框架。
速度字段用米/秒，通过 QGC SpeedSection/Fact 提交，不另建 speed 数据副本。
更复杂的任务项继续由 QGC 原编辑器处理。重叠标记继续使用 QGC 的分组选择。
正式规划页使用 FlightMap；实验性 GeoMap 没有在本轮接入 CAAC 编辑事件。

包含 `DO_JUMP` 的计划拒绝 CAAC 插入、删除和排序，防止破坏目标序号；
增加或取消附属速度命令也会改变序号，因此采用相同的限制。
排序不得跨起飞、降落、动作或复杂任务项。限制会在面板显示说明，失败前不会修改模型。
精确属性修改检查任务项仍属于当前计划；车辆传输期间暂停修改。
这些编辑不会自动向飞控上传航线、修改 Home、解锁或起飞。

## 修改文件

| 文件 | 职责 |
| --- | --- |
| `custom/CMakeLists.txt`、`custom/cmake/CustomOverrides.cmake` | 定制模块构建，保留原功能配置 |
| `custom/src/CaacCorePlugin.h/.cc` | 注册插件，提供规划页面板与直接编辑选项 |
| `custom/src/Route/CaacRouteEditorController.h/.cc` | 绑定唯一的 QGC 任务模型，校验、属性提交、插删排序 |
| `custom/src/PlanView/CaacRouteEditor.qml` | 原生编辑入口、地图鼠标操作 |
| `custom/src/PlanView/CaacWaypointDialog.qml` | 可取消的精确属性输入 |
| `src/API/QGCCorePlugin.h` | 默认关闭的通用规划页扩展接口 |
| `src/PlanView/PlanView.qml`、`PlanViewRightPanel.qml` | 面板 Loader、定制编辑时避免原单击加点 |
| `src/FlightMap/FlightMap.qml` | 定制模式事件信号，暂停原鼠标处理；原触摸逻辑保留 |
| `src/PlanView/MissionItemMapVisualBase.qml`、`src/FlightMap/MapItems/MissionItemIndicatorDrag.qml` | 可见航点的直接拖动和右键事件 |
| `src/MissionManager/MissionController.h/.cc` | 经校验的普通航点移动接口，同步任务树和序号 |
| `custom/test/CMakeLists.txt`、`CaacRouteEditorTest.cc`、`CaacRouteEditorUITest.cc` | 模型、文件、失败路径及完整 MainWindow 鼠标测试 |
| `test/CMakeLists.txt` | 定制测试注册 |
| `test/QmlUITests/CMakeLists.txt` | 原单击/添加模式测试明确标为 StockUI；保留全部断言，CAAC 使用专项 UI 测试 |
| `.github/workflows/windows.yml` | x64 Debug/Release 专项测试、验证安装包下载；本仓库不执行 AWS 发布 |

## 编译与测试

云环境复用已验证 Qt 6.11.1、GStreamer 和本地 sysroot，无需管理员权限。

```sh
cd /workspace/CAAC-QGC-GroundStation
source /workspace/qgc-cloud/activate.sh
/workspace/qgc-cloud/configure.sh
cmake --build /workspace/build/qgc-baseline --parallel 4
cd /workspace/build/qgc-baseline
CI=1 python /workspace/CAAC-QGC-GroundStation/.github/scripts/cmake_helper.py ctest \
  --build-type Debug --jobs 1 --tests-regex '^CaacRouteEditor(Test|UITest)$' \
  --include-labels 'Unit|Integration' --exclude-labels 'Flaky|Network' \
  --junit-output junit-caac.xml --ctest-output caac-output.txt
```

新增测试检查插入位置、序号和任务树一致性、附属速度保留、无效输入不部分修改、
完整 `.plan` 重载、排序边界和旧任务项引用。UI 测试启动真实 QGC MainWindow，
实际发送鼠标事件验证单击不加点、双击加点、右键弹窗、直接拖点与左键拖图。

### 本次保存时的实际结果

用户要求暂停并保存工作；阶段二尚未完成验收。

| 检查 | 结果 |
| --- | --- |
| Linux Debug 全量及增量编译 | 通过；最后增量检查返回 `ninja: no work to do`，退出码 0 |
| `CaacRouteEditorTest` | 通过，2.02 秒；覆盖未启动模型保护、DO_JUMP 下速度命令增删的原子拒绝等新断言 |
| `CaacRouteEditorUITest` 首次运行 | 失败，32.89 秒；双击加点处预期 2 个任务项，实际 1 个 |
| UI 测试修正后的运行 | 尚未执行；已将两次 `mouseClick` 改为原生 `QTest::mouseDClick` 并成功重编译 |
| 相关文件 pre-commit 检查 | 通过；保存前再次检查全部未提交文件，包含双击测试修正 |
| 阶段二全量 Unit 回归、单独启动验证 | 尚未执行 |
| 阶段二 Windows Debug / Release | GitHub Actions 正在运行旧提交，尚无最终结果；不代表最新保存版本通过 |

QtTest 的两次默认 `mouseClick` 会在释放后推进超过双击间隔的事件时间，
不能模拟这里需要的真实双击。因此修正为 `mouseDClick`。
修正后尚未运行，不据此认定界面行为已通过；右键、拖点、拖图等后续 UI 断言仍待执行。
UI 首次运行未到截图步骤，尚无通过验收的界面截图。

Windows 验证任务使用提交 `4b4aa2e0a7e456a78bcabeedfd1d7248da03294a`，
未包含后续保护、弹窗目标固定、双击测试修正和工作流参数整理：

- [Debug 验证任务](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38024915730)
- [Release 验证任务](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38024765242)

### 下次继续的位置

1. 检出 `phase2/caac-waypoint-editor` 并拉取最新保存提交；先读取本报告和 `AGENTS.md`。
2. 若云工作区仍在，执行上述激活、编译和专项测试命令，首先复测修正后的真实鼠标交互。
   不以阶段一的测试结果替代阶段二验证；发现失败时保留日志并定位具体事件或模型状态。
3. 相关测试通过后执行要求的全量 Unit 回归和启动验证，再复核相关文件 lint。
4. 查询上述旧 Windows 任务的结果，然后对最新保存提交重新触发 Debug / Release。
   两次触发之间先等待前一任务的 Detect Changes 结束，避免共享检测工作流取消另一任务。
5. 保存实际编译、测试、安装包验证结果，完成阶段二报告后再进入阶段三。

云环境激活文件为 `/workspace/qgc-cloud/activate.sh`，配置脚本为
`/workspace/qgc-cloud/configure.sh`；构建目录名称仍是 `qgc-baseline`，
其中当前二进制已经包含 CAAC 修改，不应称为原版 QGC。

本次 lint 额外使用经过官方 SHA256 校验的 Go 1.27.2。
复用本地 pre-commit 工具时，在激活环境后设置：

```sh
export PATH="/workspace/.tools/go-1.27.2/go/bin:$PATH"
export PRE_COMMIT_HOME=/workspace/.cache/pre-commit
export npm_config_cache=/workspace/.cache/npm
export GOPATH=/workspace/.cache/go
export GOCACHE=/workspace/.cache/go-build
export GOFLAGS='-p=2 -buildvcs=false'
export PRE_COMMIT_FROM_REF=0ea5771df61ad3d72cb08c55c9ed1cd380df2270
```

`PRE_COMMIT_FROM_REF` 让区域格式检查相对于阶段一提交检查本次修改；
`GOFLAGS` 仅用于构建 lint 工具，未跳过代码检查或测试。
主要日志保留在 `/tmp/caac-phase2-build-final.log`、
`/tmp/caac-phase2-ui-rebuild.log`、`/tmp/caac-phase2-tests.log`、
`/tmp/caac-phase2-lint-final.log`、`/tmp/caac-phase2-checkpoint-lint.log`；
JUnit 和测试输出位于构建目录。
工作保存时另打包为 `/workspace/caac-phase2-validation-checkpoint.zip`，
避免验证记录仅依赖临时目录；源码以 GitHub 阶段二分支为准。

## 后续阶段与限制

本轮不实现任意中心旋转、两点定向、相对坐标和撤销/重做，按阶段三继续。
`.caacroute` 版本封装与 `.ld` 兼容导入尚未实现；当前只使用已有 `.plan` 格式。
考试计时、日志、评分、培训管理和正式品牌安装发布按后续阶段进行。
没有使用凌动专有代码、资源或 V1 几何模块的未确认许可证代码。
