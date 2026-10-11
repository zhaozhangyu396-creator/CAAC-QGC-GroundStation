# 本批界面改版与 Windows 验证记录

应用源码：`5fc662e165e2d694edab2b46213fdcde1ca12dc8`。分支：`phase3/caac-route-geometry`。
报告日期：2026-10-11（Asia/Shanghai）。报告与后续截图规格提交不改变本次安装包的应用源码身份。

## 已实现的范围

- 用户指出的飞行、航线旋转、高度基准、速度、航线偏移、设置菜单和任务统计等误译修正。
- 编号航点卡片、左键拖动实时距离/方位角/夹角提示、右键相对上一航点的位置编辑。
- 航点编号与 QGC 地图任务编号保持一致；速度指令改变任务编号时，选择列表同步更新。
- 相对坐标编辑器移入右侧，分基础/高级设置，提供添加按钮及输入框外的空格键加点。
- 保留 QGC 任务参数入口；CAAC 版本隐藏用户要求移除的计划信息分组。
- Windows 安装版验证在排除 Qt/GStreamer 开发 SDK 的 PATH 下进行。

右键测试在切换原生设置面板后重新居中航点并等待地图投影更新，
检查右键按下实际交给航点控件，继续验证弹窗、输入拒绝/接受、拖动和撤销。
该调整针对自动测试的屏幕坐标前提，不将其描述为已经证实的产品右键缺陷修复。

完整四入口布局、全局高级设置、模板生成、整条航线拖拽仍待实现，
详见 [功能布局清单](lingdong-ui-implementation.md)。高级设置截图仅确认字段显示规则；最新要求移除“停止转弯”选项，保留其余四种方式，并补充停留朝向选择“指定”时显示指定角度。
本次不构成 PX4 SITL、实机、正式考试规则、全界面专业翻译或阶段六发布验收。

## 修改文件

相对上一报告提交 `44bee8912e47eb674121396f67147680d43e98d6` 的已提交文件如下。
当前报告和新增高级设置规格为后续文档变更，另行提交。

- `.github/scripts/tests/test_windows_installed_path.py`
- `.github/scripts/tests/test_workflow_security_policy.py`
- `.github/scripts/windows_installed_path.py`
- `.github/workflows/windows.yml`
- `custom/CMakeLists.txt`
- `custom/src/CaacCorePlugin.h`
- `custom/src/PlanView/CaacGeometryDialog.qml`
- `custom/src/PlanView/CaacRelativeEditor.qml`
- `custom/src/PlanView/CaacRouteEditor.qml`
- `custom/src/PlanView/CaacWaypointDialog.qml`
- `custom/src/Route/CaacRouteEditorController.cc`
- `custom/src/Route/CaacRouteEditorController.h`
- `custom/test/CaacRouteEditorTest.cc`
- `custom/test/CaacRouteEditorUITest.cc`
- `docs/caac/lingdong-ui-implementation.md`
- `docs/caac/phase3-route-geometry.md`
- `src/API/QGCCorePlugin.h`
- `src/MissionManager/MissionController.cc`
- `src/PlanView/MissionItemEditor.qml`
- `src/PlanView/PlanTreeView.qml`
- `src/PlanView/PlanView.qml`
- `src/PlanView/PlanViewRightPanel.qml`
- `test/MissionManager/MissionControllerTreeTest.cc`
- `test/MissionManager/MissionControllerTreeTest.h`
- `translations/qgc_json_zh_CN.ts`
- `translations/qgc_source_zh_CN.ts`

## 实际编译与测试

| 检查 | 结果 | 证据说明 |
| --- | --- | --- |
| Linux Debug 编译、启动 | 通过 | Qt 6.11.1 SDK，GCC 14.2；最新 SHA 重编译及启动检查 |
| 最新应用代码完整 Unit | 341/341，61.34 秒 | 提交前相同应用代码；随后对准确 SHA 重编译 |
| 准确 SHA 任务树/模型/中英文 UI | 4/4，15.06 秒 | Linux 下执行真实 QML 鼠标操作测试 |
| 最新修改文件 lint | 通过 | changed-region clang-format 及 pre-commit |
| 前一源码完整 Unit/Integration | 426/426，482.99 秒 | 源码 4f9180，不能称为最新源码完整 Integration 复测 |
| Windows x64 Debug 模型/中英文 UI | 3/3，105.64 秒 | run 38122033368，job 114418625479；JUnit 3 项全部执行，通过 |
| Windows x64 Release | 通过；Portable 10/10，1.13 秒 | run 38122068762，job 114418721007；编译/启动、安装/升级、SDK PATH 隔离、安装版启动、卸载通过 |

前一源码 `4f9180d8be` 的 Release 已通过编译、Portable 10/10（1.06 秒）、启动、
实际 NSIS 中文检查、安装/升级、排除 Qt/GStreamer SDK 后启动和卸载清理。
该源码的 Debug 中文 UI 首次右键弹窗检查失败，模型/英文 UI 通过，失败记录保留。
更早的 `71aa653289` Windows 3/3 结果不能替代最新源码验证；其旧 SDK PATH 过滤也未实现隔离。

命令从指定目录执行，未运行的检查不计为通过：

```bash
source /workspace/qgc-cloud/activate.sh
cd /workspace/CAAC-QGC-GroundStation
cmake --build /workspace/build/qgc-baseline --parallel 4
python tools/run_tests.py -B /workspace/build/qgc-baseline -t Debug --verify-only --headless
cd /workspace/build/qgc-baseline
CI=1 python /workspace/CAAC-QGC-GroundStation/.github/scripts/cmake_helper.py ctest \
  --build-type Debug --jobs 2 --include-labels Unit --exclude-labels 'Flaky|Network|StockUI'
CI=1 QGC_TEST_VERBOSE=1 python /workspace/CAAC-QGC-GroundStation/.github/scripts/cmake_helper.py ctest \
  --build-type Debug --jobs 1 \
  --tests-regex '^(MissionControllerTreeTest|Caac(RouteEditor(Test|UITest)|ChineseUITest))$' \
  --include-labels 'Unit|Integration' --exclude-labels 'Flaky|Network'
```

Windows 使用仓库 `.github/workflows/windows.yml`，手动指定本分支的 Debug/Release。
准确源码的证据目录：`/workspace/caac-ui-redesign-validation-5fc662e165`。
完整中文目录检查来自前一源码相同翻译目录：6,483 条非空消息、28 个技术标识例外、1 条空源文本、0 个目录错误，
不表示 6,483 条中文的专业语义均已经逐条人工验收。

## 安装包与人工验收

Windows x64 Release job 已成功完成，原生 Debug 模型/中英文 UI 3/3 已执行通过。
[下载 Windows x64 验证安装包](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38122068762/artifacts/11696455581)。
登录 GitHub 后下载制品 ZIP，解压运行 `build/QGroundControl-installer-AMD64.exe`。
制品有效期至 2026-10-25 15:54:22（Asia/Shanghai）。

- ZIP SHA256：`0543f212c884ca5117e5b37d4305b8400726a1b06f86ef4bae3c9043e7c0b2d5`，已与 GitHub API digest 比对。
- EXE SHA256：`9580bb99a136d9f89c04ac827d0175ea26709b4933f5402ba8659749da755b0f`，135,220,737 字节。
- 制品 ID：`11696455581`；应用源码 `5fc662e165e2d694edab2b46213fdcde1ca12dc8`。

可验证的运行限制是安装版在排除 Qt/GStreamer SDK 路径后通过启动检查，
不将其扩展为所有 Windows 版本、全部运行场景或实机飞行已经验证。
本轮新版人工操作、在线地图和高 DPI 尚未验收；历史 e4411d8348 的用户验收结果继续单独保留。
自动测试截图采用占位地图瓦片。
环境配置草稿保存与发布不同；准备好的源码和工具可在环境设置中审阅、保存并发布，不能将草稿保存当作发布成功。
