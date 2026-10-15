# 飞虎无人机地面站系统 Windows 验证版

应用源提交：`80691cac5b6e7e8858e3af26c5f878ef8f666c23`，版本：`v5.2.0-dev-233-g80691cac5b`，分支：`phase3/caac-route-geometry`。
本地与 Windows 验证、安装包均对应此应用源提交。后续文档提交不改变安装包的应用版本。

## 下载与校验

- [Windows 64 位安装包](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38669678904/artifacts/11943793808)：下载构件 ZIP，解压并运行 `QGroundControl-installer-AMD64.exe`。
- EXE SHA-256：`428d19ade25534e6286f786dc87eecc5fef04f30f349663f4292963660f7cb87`。
- ZIP SHA-256：`cd1cf8a889e8b3de8ecf66db2be0238ec1affad1bd52733256af2711fd16d9fa`。
- 默认创建中文桌面快捷方式“飞虎无人机地面站系统”。

## 本轮改动

- 航点拖动改用实际指针事件，避免地图投影反馈再次触发吸附；修正近重合辅助线的排序规则。新增开启吸附、模拟连接、五航点折线／闭合航线及连续拖动回归。
- 生成或移动 H 点后自动应用位置与可用海拔；地形返回后同步，保留手动海拔输入与真实净空报警。应用计划起飞点不等同于自动写入文件或上传飞控。
- 任务执行中阻止上传并提示先暂停；统计数据与真高报警可拖动；航点属性改为左键双击，Ctrl+X 重做。
- 完成侧栏开合、模拟置顶、设置中荧光绿装饰清理、UOM 折叠提示及 RID 跳转、连接窗口、电池 V 门限、电子围栏绘制／读写／地图预览与字体调整。
- USB 摇杆支持模拟应急手动接管及模式轴映射：低档返航、中档姿态、高档航线任务。首次连接先识别档位，输入中断超过 0.5 秒悬停。
- 任务详情显示剩余循环圈数；上锁红色、解锁绿色。窄窗口下姿态仪避让右上规划菜单文字。

详细行为见 [设置与规划交互更新](settings-interaction-update.md)；遥控器使用见 [模拟与飞行控制](simulation-and-flight-controls.md)。

## 验证记录

- 应用源提交：`80691cac5b6e7e8858e3af26c5f878ef8f666c23`；界面版本：`v5.2.0-dev-233-g80691cac5b`。
- Linux Debug 构建、启动检查及修改文件静态检查通过；界面／任务回归 15/15、完整串行 Unit 346/346 通过，CTest 类别无失败或跳过。
- Qt 内部按后端条件跳过 12 个既有数据用例；本批 CAAC 界面及功能用例全部执行。
- 中文源审计 7249 条，零错误。
- Windows x64 RelWithDebInfo 原生界面／任务回归 15/15、Portable 10/10，以及 Release Portable 10/10 通过。
- Release 验证中文安装脚本、SHA-256、初装、改变安装目录升级、设置保留、中文桌面快捷方式、移除 Qt／GStreamer 构建 SDK 后独立启动与卸载清理。
- [Windows 功能回归](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38669779819) · [Release 安装与升级验证](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/38669678904)。

拖动回归覆盖原生鼠标事件、吸附与撤销／重做。USB 回归通过 SDL 虚拟设备验证原生摇杆、MAVLink 与模拟控制链路。尚未用用户现场 Windows 环境复现此次卡死，也未进行实体 USB 遥控器验收，因此不将自动化通过表述为现场问题已经完全排除。

电池保护仍由 PX4 原生剩余电量阈值触发，V 是按电池节数及满／空电压换算的参考显示，不支持的动作禁用。姿态模拟为训练运动模型，不代表真实飞控动力学。UOM 正式数据上报尚未接通；安全参数、RID 广播及实际飞行行为尚未进行实机验收。
