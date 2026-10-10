# 阶段二：QGC 原生航点编辑器

日期：2026-10-10（Asia/Shanghai）。分支：`phase2/caac-waypoint-editor`。
源码沿用阶段一验证的 QGC 开发基线；这是开发验证版本，不是正式 Windows 发布版。
阶段二的原生航点编辑实现和自动验证已完成；最终实现提交为 `dcc1a55915`。
Windows x64 安装包、校验和及实际验证记录见下文。人工操作及实机验收仍属后续工作。

## 本轮续接验证

实现提交：`9bcb7fb286`；Windows 验证配置提交：`ed59ccfe50`；
Windows 原生 UI 测试环境修复：`58424cf2c7`；调用栈诊断：`c23cf73ee7` / `c706c845c9`；
对应 Qt PDB 符号诊断：`eee025c6eb`。
空滚动区域的尺寸修复及回归：`5ea303c92b`。
避开底部覆盖控件并检查鼠标抓取对象的 UI 测试调整：`01939dea28`。
空航线 / 等高航点的图表范围修复：`0f18e1517f`。
未知海拔下的缺失地形提示线保护及回归：`dcc1a55915`。
以下内容记录暂停后继续开发的结果；后面的“暂停保存记录”保留历史失败和续接依据。

| 检查 | 当前结果 |
| --- | --- |
| Linux Debug 增量编译 | 通过 |
| CAAC 模型及原生 UI 专项 | `dcc1a55915` 2/2 通过，2.95 秒；新增未知海拔、提示线断点、单位缩放与航线数据不变检查 |
| 完整 Unit / Integration 回归 | 最终 `dcc1a55915` 425/425 通过，515.02 秒（341 Unit、84 Integration） |
| 原生程序启动检查 | 通过；使用仓库 `run_tests.py --verify-only`，退出码 0 |
| 最新相关文件 lint | 最终 26 个修改文件全部适用检查通过 |
| 与 Windows 配置及 Qt 安装相关的 Python 测试 | 77/77 通过 |
| Windows x64 Release | 最终 `dcc1a55915` 编译、Portable 10/10、启动、安装/升级、移除 SDK PATH 后启动、卸载清理全部通过 |
| Windows x64 Debug | 最终 `dcc1a55915` 编译、模型及原生 UI 2/2 通过，43.69 秒；严格日志检查通过 |

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
- [调用栈诊断头文件顺序修正后的 Debug 复测](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38040690410)
- [补充对应 Qt PDB 的 Debug 诊断](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38041690429)
- [本轮 Windows Release](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38037038524)
- [空滚动区域修复后的 Windows Debug](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38042520212)
- [空滚动区域修复后的 Windows Release](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38042641594)
- [避开底部覆盖控件后的 Windows Debug UI 复测](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38043745780)
- [图表范围修复后的 Windows Debug](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38045068279)
- [图表范围修复后的 Windows Release，随后被最终版本构建取代](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38045176986)
- [未知海拔保护后的 Windows Debug](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38045921605)
- [未知海拔保护后的 Windows Release](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38046017262)

第一次 Debug 诊断及 Release 针对 `ed59ccfe50a554a2f35754068ccf2986b27b4097`。
Debug 诊断已通过模型测试，但无界面插件未找到字体，UI 触发
`qnumeric.h:523` 的浮点转整数断言并退出，错误码 `0xc0000409`。
未将此运行记为通过。`58424cf2c781dcc31745fcc1717fc60b4fa72ea5` 将该项 UI 测试
改用 Windows 原生平台，继续使用软件渲染和原来的全部断言，并在发送鼠标前检查视口。
原生窗口复测消除了缺字体警告，但相同数值断言仍出现，不能将缺字体认定为根因。
`c23cf73ee7` 为该测试增加 Windows 调用栈和步骤日志，并保持原有断言。
该次 x64 编译因格式化后 `dbghelp.h` 位于 Win32 声明之前而失败，未进入 UI 测试；
`c706c845c9` 将依赖头文件分成有明确注释的独立包含块，格式化复查通过，继续复测。
该次 x64 编译及模型测试通过（8.73 秒）；UI 仍异常退出（10.14 秒）。
步骤日志仅输出 `start UI`，确认失败发生在 MainWindow 初始化期间，尚未进入规划页或执行航点操作。
调用栈经过 Qt Quick、Windows 平台插件和 COM 回调；未安装完整 Qt PDB 时，
DbgHelp 返回的最近导出函数名及大偏移不能作为真实函数名或根因证据。
`eee025c6eb` 从同版本官方 Qt 包安装 `qtbase` / `qtdeclarative` 调试符号到独立目录，
以保持运行 SDK 不变，并将符号目录交给诊断 handler；配置 lint 与相关 Python 测试 38/38 通过。
该次符号安装因包装脚本的 `--archives` 需要单个字符串参数而失败，未执行 UI；
已改为 `--archives "qtbase qtdeclarative"`。符号是可选诊断依赖，下载失败不阻止实际 UI 测试。
不将原规划页零范围图表假设当作本次原因。
上述诊断提交仅修改测试源与测试环境，至 `eee025c6eb` 的应用源码与 `ed59ccfe50` 一致。
源码和上述修复已推送；Linux 验证完成，阶段二验收仍需修复并通过 Windows UI 验证。

本机主窗口几何诊断定位到 `QGCFlickableScrollIndicator.qml` 的独立缺陷：
空列表 / 空滚动区域的内容尺寸为零时，手动除法生成无限宽或高，即使控件不可见也保留该值。
`5ea303c92b` 仅将四处尺寸 / 位置计算改用 Qt 原生 `visibleArea` 比例，消除这些无限值。
新增 UI 检查覆盖空内容、正常双向滚动（位置及半视口长度）、清空后的有限尺寸和隐藏状态。
模型及完整航点 UI 流程仍为 2/2 通过；保留 Windows 辅助功能和全部原有断言。
这次已改变应用源码，因此重新执行完整 Linux 回归及 Windows Debug / Release；
尚不将旧安装包的通过结果当作 `5ea303c92b` 的验证结果。

`5ea303c92b` 的 Windows 原生 UI 不再触发数值断言，已实际执行双击加点、属性校验、
弹窗目标固定和直接拖点；空内容与正常滚动比例检查也通过。
该次 UI 在拖图处失败（32.75 秒），原测试点位于地图高度 75% 处，可能落入底部图表覆盖区域。
`01939dea28` 将取点移到地图高度 40%、宽度 20% 处，并明确检查鼠标抓取对象为 CAAC 地图 MouseArea，
仍要求地图中心移动超过 1 米且不增加航点。此次仅修改测试，应用功能源码与 `5ea303c92b` 一致。
本机专项重跑 2/2 通过；当时 Windows 全流程复测尚未完成，未认定阶段二已验收。
该次 Windows 日志还出现 `QPainterPath::moveTo` 无效坐标警告；未添加忽略规则或宣称无警告。

`01939dea28` 的 Windows UI 已完成全部操作断言（含拖图、排序、取消保存、保存和加载），
但严格日志检查因 6 条 `QPainterPath::moveTo` 警告判为失败（24.03 秒），没有标记通过。
`0f18e1517f` 保证原 TerrainStatus 图表的轴范围有限且非零：
空航线的距离轴采用一个显示用米的最小范围，等高航点的高度轴至少保留一个显示用米的跨度，
再转换为当前单位；这是显示范围，航线数据仍由原任务模型提供。
新增检查覆盖空航线、单点、等高点及清空后的有效轴范围；本机专项 2/2 通过。
Windows handler 只为首条相同绘制警告增加符号调用栈，原警告仍转发给严格检查，未加入忽略规则。
该次 Linux 完整回归 425/425 通过（523.46 秒），启动验证通过；Windows Debug 模型与原生 UI
2/2 通过（模型 17.63 秒、UI 28.03 秒），严格日志检查也通过。
这证明坐标轴范围修复消除了实际流程中的绘制警告；没有屏蔽警告或改变断言。
该 Debug 任务后续打包被最终版本任务取消；仅将已经完成的专项测试记为通过，
不将整项被取消的任务记为成功。中间 Release 任务也被最终版本构建取代。

随后补充未知海拔边界测试，修复前实际失败于缺失地形提示线的首点不有限。
Qt 6.11.1 `PointRenderer::updateLineSeries` 支持内部无效点断线，但初始 `moveTo` 不检查首点。
`dcc1a55915` 将未知海拔时的黄色缺失提示线基准回落到图表已有的零基准。
测试确认内部断点保留、未知飞行高度不产生飞行曲线、已知负海拔和单位缩放仍正确，
完整计划 JSON 未改变；修复后本机专项 2/2 通过（2.82 秒）。
此项是追加的边界保护，不将它认定为前次 Windows 警告的已证实原因。
最终源码的完整 Linux 回归已通过（425/425，515.02 秒），原生启动检查退出码 0；
保存截图的专项复跑也为 2/2 通过（2.95 秒）。Windows Debug 专项 2/2 通过（43.69 秒），
模型 17.86 秒，原生 UI 25.77 秒；严格日志检查通过。Release x64 任务全部通过。

### 最终 Windows x64 产物

实现提交：`dcc1a5591591ce695b337829ae937fec9b26c9cc`。
Release x64 任务：`114195726548`；Debug x64 任务：`114195440383`。
采用 Qt 6.11.1、MSVC 19.44.35229.0、GStreamer 1.28.4，保留 QGC 原生运行库及打包机制。

安装包：`/workspace/caac-phase2-windows-x64-dcc1a55915/QGroundControl-installer-AMD64.exe`。
大小：135140342 字节。SHA256：
`6591217328993f6da8a7199462499780727f72ee48150e14f83ac04c928e1e70`。
本地计算与 CI 校验和文件一致；同目录保存 `使用说明.txt`、校验和及 `validation-provenance.json`。
产物 ID：`11666649773`，名称 `caac-validation-amd64-Release`。
本地验证记录区分实现提交、工作流、具体 x64 任务、归档摘要和安装包摘要。

Release Portable 10/10 通过（0.89 秒）。x64 安装、升级、清除 Qt/GStreamer SDK PATH 后
启动已安装程序、卸载并检查清理均通过。这些验证在 Windows 原生 runner 上实际运行。
当前产品仍使用 QGC 名称；正式品牌、签名和发布按阶段六完成。

源码已推送到阶段二分支。完整阶段二验证资料另存于
`/workspace/caac-phase2-validation-dcc1a55915/`，并打包为
`/workspace/caac-phase2-validation-final.zip`；安装程序独立提供。

### 历史安装包

中间 x64 安装包已单独保留，源提交 `5ea303c92b1d5b41b795af647427d4398ec3857a`。
路径：`/workspace/caac-phase2-windows-x64-5ea303c92b/QGroundControl-installer-AMD64.exe`。
大小 135134891 字节，SHA256：
`4a469295d46c0d540502ed67f3fe64d77035b6a86260ba1c83f4f068bf5fa82b`。
本地计算与 CI 校验和一致；同目录保存校验和及 `validation-provenance.json`。
Release Portable 为 10/10 通过，0.97 秒；x64 安装、升级、清除 SDK PATH 后启动及卸载步骤均通过。

早期提交 `ed59ccfe50` 的三个 Windows 架构任务均成功。
本段记录该早期验证安装包，最终版本见前文。
x64 安装验证包括升级、移除 Qt/GStreamer SDK 搜索路径后启动已安装程序、卸载并检查清理。
安装包保存到 `/workspace/caac-phase2-windows-x64-release/QGroundControl-installer-AMD64.exe`，
大小 135124572 字节，SHA256：
`26ee6f1fa33168213393c2d83dcaad57e040202bcf0924280925c286db3d93ae`。
本地计算与 CI 校验和文件一致；验证记录为同目录的 `validation-provenance.json`。
它是航点编辑验证版；尚未实现阶段三至六，也未完成实机飞行及 Windows 人工操作验收。

最终源码的专项截图保存为 `/workspace/caac-phase2-editor-dcc1a55915.png`；
旧专项截图为 `/workspace/caac-phase2-editor.png`。
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

下次继续先读取本报告和 `AGENTS.md`，检查阶段二分支及保留的验证记录。
阶段三再实现旋转、相对坐标和撤销/恢复；几何接口与 QGC 模型分离，并保留任务属性及顺序。

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
| `src/QmlControls/QGCFlickableScrollIndicator.qml` | 空列表 / 空滚动区域使用 Qt 原生可视比例，避免除零产生无限尺寸 |
| `src/PlanView/TerrainStatus.qml` | 空航线 / 等高航点保留有限、非零的图表轴范围 |
| `src/QmlControls/TerrainProfile.cc` | 未知海拔时缺失地形提示线保留有限显示基准；内部断线及飞行数据语义保持 |
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

完整回归及启动命令：

```sh
cd /workspace/build/qgc-baseline
CI=1 python /workspace/CAAC-QGC-GroundStation/.github/scripts/cmake_helper.py ctest \
  --build-type Debug --jobs 2 --include-labels 'Unit|Integration' \
  --exclude-labels 'Flaky|Network|StockUI' \
  --junit-output phase2-final-regression-junit.xml --ctest-output phase2-final-regression-output.txt
cd /workspace/CAAC-QGC-GroundStation
python tools/run_tests.py -B /workspace/build/qgc-baseline -t Debug --verify-only --headless
```

`StockUI` 仅标记原版的单击加点 / 添加模式交互测试；定制版用上述原生 CAAC UI 测试验证新习惯，
其他 UI 回归继续执行。保留 `Flaky|Network` 的仓库既有筛选，不将这些未运行测试计入通过数量。
Linux 应用为 Debug，现有 Qt SDK 的库为 Release；Windows Debug 同时验证 Qt Debug 图形路径。
Windows 构建及原生安装验证由本分支 `.github/workflows/windows.yml` 执行。
手动触发 `build_type=Debug` 和 `build_type=Release` 时，两次之间等待第一项 Detect Changes 完成。

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

### 当时记录的续接步骤（历史）

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
新增面板使用可翻译字符串，中文译文尚待补齐。Windows 人工操作、硬件图形驱动、
PX4 SITL 和实机飞行尚未在本阶段验收。
没有使用凌动专有代码、资源或 V1 几何模块的未确认许可证代码。
