# 测量、单位换算与任务初始速度

2026-10-12（北京时间），继续补齐凌动布局的实际功能。

## 本批功能

- 全局设置顶部增加 A/B 测量和单位换算入口，使用可拖动的原生窗口。
- 测量支持在地图上选取 A、B 两点，也可输入十进制度或度分秒坐标；空白坐标可切换到度分秒。
  地图显示独立的 A/B 标记和连线，窗口显示 WGS84 水平距离与 A 到 B 的真北方位角；重合点不显示虚构方位角。
  按 Esc 取消选点，切换编辑层或关闭规划视图时取消选点；清除测量不改变任务。
  测量点只保留当前界面会话，不保存为航点，也不写入 `.plan`。
- 换算支持长度 m/km/ft/NM，速度 m/s/km/h/kn/mph，角度度/弧度；实时计算，拒绝空白、非法和溢出数值。
- 全局任务初始速度使用原生 `MissionSettingsItem::SpeedSection` 和 Facts，生成 `MAV_CMD_DO_CHANGE_SPEED`。
  后续航点自己的速度命令覆盖初始速度；按原生机型规则保留地速/空速类型，不把此值当作垂直速度。
  更改指定状态和数值为同一次撤销事务，保持原生 `.plan` 保存/加载；含跳转命令的任务保留已有顺序保护。
  速度拒绝非正值、非法值、超出 Fact 范围及无法表示为 MAVLink 浮点参数的值。

## 验证

- 原生 Xcb/Mesa、125% 缩放：任务模型、中英文界面共 3/3，46.34 秒。
  UI 覆盖实际按钮、空白度分秒切换、地图选点、测量结果、非法坐标拒绝、任务保持不变、换算维度切换和非法值、初始速度保存及撤销。
- 模型检查验证生成原生速度命令、非法输入的原子拒绝、撤销/重做和 `.plan` 保存/加载。
- 最新 lupdate 源码中文审计：6625 条消息、30 个技术标识、1 条空来源、0 错误。
- 全部单元测试 344/344，43.40 秒；记录 `caac-utilities-unit.xml`。
- 本批静态检查、精确源码重建和启动检查通过；生成类型的 qmllint 四文件通过，保留原生 QGCCorePlugin 类型声明未解析的警告，未掩盖运行时错误。
- 截图 `/workspace/caac-measure-125.png` 已检查 A/B 标记及连线；地图是离线测试占位瓦片，不构成在线地图或实机验证。

初次 UI 检查发现 Repeater 对重新挂接到地图的标记执行错误的兄弟节点排序，已改用 Instantiator；未屏蔽相关警告。
空白坐标格式切换的禁用条件也已修正，未删除检查。

记录位于 `/workspace/caac-utilities-*` 和 `/workspace/build/qgc-baseline/caac-utilities-*.xml`。
完整转弯/爬升、闭合循环、任务结束、模拟应急、教学/考试与 UOM 接入仍待后续开发；本批不称为完整凌动等效软件。

## Windows 验证与安装包

应用源码 `969955fd42d0d9fe07f2df44ad3df5e0e2bef749`。
Debug 运行 `38195468118`，x64 作业 `114637084626`，8/8，0 失败、0 跳过，214 秒。
本地 JUnit `/workspace/caac-969-debug-results/junit-caac.xml`。
Release 运行 `38195637051`，x64 作业 `114637570630` 成功：Portable 10/10、编译后启动、实际 NSIS 中文审计、安装校验值、
安装与升级、排除 Qt/GStreamer 构建 SDK 路径后的安装版启动、卸载与清理均通过。
两份包内中文审计均为 6625 条消息、30 个技术标识、1 条空来源、0 错误。

[下载新版 Windows x64 Release 验证包](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38195637051/artifacts/11723707501)。
登录 GitHub 下载 ZIP，解压运行 `build/QGroundControl-installer-AMD64.exe`。
Artifact `11723707501`，名称 `caac-validation-amd64-Release`，有效期至 2026-10-26 10:40:26（北京时间）。

| 文件 | 字节数 | SHA256 |
| --- | --- | --- |
| 下载 ZIP | 135255387 | `0acebb8cef875ad191b3ffe21e9ffdc5975842de87a501c78ee455b34133d589` |
| 安装 EXE | 135264754 | `6569c99b7e9a4ac31b87833736ce0caa7d16a7a17fefc64cd710e4e2f9c09a39` |

ZIP 与 GitHub API digest 一致，EXE 与包内校验值一致；包名也与校验文件一致。
记录为 `/workspace/caac-969-package-proof.json`、`caac-969-release-validation.json` 和 `caac-969-portable/junit-portable.xml`。
此包包含此前 ROI 修复、凌动式规划/飞行面板、航段标注、窗口拖动、模板、度分秒及高级航点参数，以及本批三项功能。
先前源 `28fe957425` 的包没有本批测量、换算与任务初始速度功能；测试不代替 Windows 人工 DPI、实机飞行或 SITL 验收。
