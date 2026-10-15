# 飞虎无人机地面站系统 Windows 验证版

应用源提交：`c6f0b42ad7ad234d611f95ea5791b14ca1c64eec`，版本：`v5.2.0-dev-238-gc6f0b42ad7`，分支：`phase3/caac-route-geometry`。
安装包和本地、Windows 检查均对应此应用源提交；后续文档提交不改变该安装包。

## 下载与校验

- [Windows 64 位安装包](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38735430290/artifacts/11973229986)：下载 ZIP，解压并运行 `QGroundControl-installer-AMD64.exe`。
- EXE SHA-256：`99809a59e11d7627b69fe1b24a162b234780656af9a1e4d6d1fe93913d942612`。
- ZIP SHA-256：`7445e71e443395e2ae1e551fdffc06d8e7e12962aa502d81aa928c62cb3e34dd`。
- 默认创建中文桌面快捷方式“飞虎无人机地面站系统”。

## 本轮改动

- 齿轮菜单设置项和统计字段增加统一边框；校准、UOM 等操作按钮按文字收窄、加粗并居中；连接页采用已确认的宽圆角连接按钮和独立刷新图标。设置开关显示 ON／OFF，标题竖条统一为实心深色。
- 飞行中拖动航点询问是否更新；确认后等待暂停、上传成功、恢复原目标，再续飞，失败保持暂停。保留已确认的循环进度和有效速度。
- 航点右键可设任务结束降落点，空白处右键可清除本地航点；经纬度换算、起飞点／航点属性同排度分秒、功能分组横线均已加入。
- 移除参数访问密码；顶部“航线飞行”和传感器小窗口、图标间距、地图竖向工具条位置、字体等按反馈更新。
- 模拟任务详情显示有效航点速度、爬升方式和剩余圈数。USB 模式开关保留低档返航、中档姿态、高档航线任务。
- 航点拖动采用实际指针事件，避免投影反馈反复触发吸附，保留近重合辅助线及连续拖动回归。

详细操作见 [规划控件与飞行中航点更新](planning-controls-live-update.md)、[设置与规划交互](settings-interaction-update.md)、[模拟与飞行控制](simulation-and-flight-controls.md)。

## 验证记录

- 应用源提交：`c6f0b42ad7ad234d611f95ea5791b14ca1c64eec`；界面版本：`v5.2.0-dev-238-gc6f0b42ad7`。
- Linux Debug 构建、启动、修改范围静态检查通过；界面／任务回归 15/15、完整串行 Unit 346/346 通过，CTest 测试类无失败或跳过。
- Qt 内部按后端条件跳过 12 个既有数据用例；本批 CAAC 用例全部执行。
- 当前源文本中文审计 7345 条，零错误。
- Windows x64 RelWithDebInfo 原生界面／任务 15/15、Portable 10/10，以及 Release Portable 10/10 通过。
- Release 的中文安装脚本、SHA-256、初装、改变目录升级、设置保留、中文桌面快捷方式、移除 Qt／GStreamer 构建 SDK 后独立启动、卸载清理均通过。
- [Windows 功能回归](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38735481164) · [安装与升级验证](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38735430290)。

现场 Windows 卡死尚未复现并穷尽定位，实体 USB 遥控器和实际飞行尚未验收。自动化通过不代表现场卡死已完全排除。姿态模拟为训练模型；UOM 正式上报尚未接通，RID 广播及飞控安全参数仍需硬件验证。
