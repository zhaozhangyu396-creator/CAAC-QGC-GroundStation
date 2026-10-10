# 阶段二：QGC 原生航点编辑器

日期：2026-10-10（Asia/Shanghai）。分支：`phase2/caac-waypoint-editor`。
源码沿用阶段一验证的 QGC 开发基线；这是开发验证版本，不是正式 Windows 发布版。

## 本轮续接验证

实现提交：`9bcb7fb286`；Windows 验证配置提交：`ed59ccfe50`；
Windows 原生 UI 测试环境修复：`58424cf2c7`；调用栈诊断：`c23cf73ee7`。
以下内容记录暂停后继续开发的结果；后面的“暂停保存记录”保留历史失败和续接依据。

| 检查 | 当前结果 |
| --- | --- |
| Linux Debug 增量编译 | 通过 |
| CAAC 模型及原生 UI 专项 | 2/2 通过；新增视口检查后再次运行通过，3.63 秒 |
| 完整 Unit / Integration 回归 | 425/425 通过，481.07 秒；341 个 Unit、84 个 Integration |
| 原生程序启动检查 | 通过；使用仓库 `run_tests.py --verify-only`，退出码 0 |
| 最新相关文件 lint | 全部适用检查通过；相对于阶段一检查全部 23 个修改文件 |
| 与 Windows 配置相关的 Python 测试 | 38/38 通过 |
| Windows x64 Release | 编译、Portable 10/10、安装/升级、启动、卸载全部通过；安装包已取回并校验 |
| Windows x64 Debug | 模型测试通过；原生 UI 仍触发 Qt 数值断言；正在执行调用栈诊断，未通过验收 |

专项 UI 测试通过真实 MainWindow 验证双击加点、单击不加点、右键属性、
不部分提交无效属性、弹窗目标固定、直接拖点保留高度、左键拖图不加点、
上下排序、删除、取消保存，以及经原工具栏保存后清空再加载的完整 JSON 一致性。
保存路径包含中文文件名，用于检查原生文件通路。

双击测试改用原生 `mouseDClick` 后，发现拖点模拟需要等待标记回到视口，
并在按键仍按下时等待坐标移动，再释放按键；保留实际移动距离大于 1 米的断言。
没有屏蔽断言或将失败改为跳过。
窄侧栏的四个按钮已改为两列排列，并去除布局高度反馈引起的绑定循环；
QML 严格日志检查通过后才记录专项通过。

第一轮四进程 Unit 回归为 340/341：`QGCArchiveModelTest` 的 26 个 QtTest 项均通过，
但进程退出超时。将该项单独运行后通过（0.38 秒）；没有改动其测试或超时配置。
随后以两进程重新执行完整回归，425/425 全部通过，其中归档测试也正常通过。

另有一个既有 CI 安全策略测试失败：依赖审查工作流已使用
`gradle/actions/wrapper-validation@v6`，断言仍要求 `@v4`。
已核对阶段一提交，这两个文件与该提交完全一致；此项不属于航点编辑修改。
包含此文件的检查为 52 通过、1 失败；与本轮 Windows 改动相关的 38 项另行全部通过。

旧 Windows Debug 已编译并通过模型及 Portable 测试，UI 进程异常退出；
旧 Release 的 x64 编译失败原因是完整测试框架引用仅 Debug 提供的 `MockLink.h`。
已将完整 CAAC 测试限制为 Debug；Release 保留 Portable、启动及安装回归。
Debug 上传还引用了只在 Release 校验步骤中产生的空路径，已改为确定的安装包路径，
并一起上传校验和。当前产物没有 CI 生成的 `.build.json`；
另行保存了包含真实提交、任务和产物标识的本地验证记录，不能将其视为 CI 构建清单。
新 Debug 任务保留 stderr 和详细测试诊断。

- [本轮 Windows Debug 首次诊断](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38036924141)
- [Windows 原生窗口 Debug 复测](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38038153729)
- [Windows Debug 调用栈诊断](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38039945765)
- [本轮 Windows Release](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38037038524)

第一次 Debug 诊断及 Release 针对 `ed59ccfe50a554a2f35754068ccf2986b27b4097`。
Debug 诊断已通过模型测试，但无界面插件未找到字体，UI 触发
`qnumeric.h:523` 的浮点转整数断言并退出，错误码 `0xc0000409`。
未将此运行记为通过。`58424cf2c781dcc31745fcc1717fc60b4fa72ea5` 将该项 UI 测试
改用 Windows 原生平台，继续使用软件渲染和原来的全部断言，并在发送鼠标前检查视口。
原生窗口复测消除了缺字体警告，但相同数值断言仍出现，不能将缺字体认定为根因。
`c23cf73ee7` 为该测试增加 Windows 调用栈和步骤日志，并保持原有断言。
这些提交仅修改测试源与测试环境，Release 的应用源码与前次提交一致。
源码和上述修复已推送；Linux 验证完成，阶段二验收仍需修复并通过 Windows UI 验证。

Release 的三个 Windows 架构任务均已成功；当前交付优先使用 x64 产物。
x64 安装验证包括升级、移除 Qt/GStreamer SDK 搜索路径后启动已安装程序、卸载并检查清理。
安装包保存到 `/workspace/caac-phase2-windows-x64-release/QGroundControl-installer-AMD64.exe`，
大小 135124572 字节，SHA256：
`26ee6f1fa33168213393c2d83dcaad57e040202bcf0924280925c286db3d93ae`。
本地计算与 CI 校验和文件一致；验证记录为同目录的 `validation-provenance.json`。
它是航点编辑验证版；尚未实现阶段三至六，也未完成实机飞行及 Windows 人工操作验收。

专项截图保存为 `/workspace/caac-phase2-editor.png`。
自动测试截图中的底图显示 `Tile Fetch Error` 占位图，原因是仓库既有
`UnitTestTileGenerator::generateTile` 使用 `:/res/notile.png` 替代网络地图，
`UnitTestList.cc` 在测试入口安装此生成器；本轮没有替换测试底图或用其代替正式地图功能。
已验证坐标变换、航点连线和编辑，尚不以该截图认定在线底图下载正常。
同一 Bing 瓦片的只读 HTTP/HTTPS 探针均返回 200、`image/jpeg`、21621 字节，
这只证明下载端点可访问，不等同于 GUI 已完成在线地图验证。

已按 cloud-environment-onboarding:setup 保存 `install_script` 和 `start_skill` 草稿：
保留原 Qt/sysroot 安装流程，增加经过官方 SHA256 校验的 Go 安装及 lint 激活脚本，
并更新为本轮通过的两进程回归命令。新 Go 解压和已安装工具复用均实际验证。
草稿保存成功，尚未发布新的环境快照；不将保存视为发布或新任务恢复验证。

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

### 暂停保存记录

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
