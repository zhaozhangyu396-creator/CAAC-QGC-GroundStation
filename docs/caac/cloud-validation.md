# 云环境补充验证记录

日期：2026-10-09（Asia/Shanghai）。本记录更新阶段一报告中的历史环境阻塞。
当前机器为 Debian Linux；最终产品目标仍为 Windows 64 位。

## 已解决的安装阻塞

用户更新网络设置后，Qt 官方下载入口恢复可访问；通过仓库的
`tools/setup/install_qt.py install --from-config` 安装 Qt 6.11.1 和配置指定的模块。
没有降低 Qt 版本，没有关闭 TLS、安装包校验或软件包签名检查。
只将已安装 aqt 的官方安装器临时目录和缓存目录改到 `/workspace/.cache/aqt`。

当前账户仍没有管理员权限。通过 apt 下载经过签名索引验证的 Debian 软件包，
使用 `dpkg-deb -x` 解压到 `/workspace/sysroot`，提供头文件、链接库和运行工具。
这不是系统级 apt 安装，不修改系统包数据库，也不需要用户提供管理员密码。
系统已安装的运行库与 GStreamer 插件通过本地符号链接复用。

首次应用编译在最终链接时因 `eglGetDisplay` 失败。原因是缺少 `libopengl-dev`，
使 CMake 未建立 `OpenGL::EGL` 目标；补齐此开发包后重新配置，
已找到 GLVND OpenGL/EGL，并重新编译原有视频缓冲路径。
本次修复仅涉及环境依赖，没有修改 QGC 视频代码或关闭视频功能。

## 保留目录与运行方式

| 目录 | 用途 |
| --- | --- |
| `/workspace/CAAC-QGC-GroundStation` | 现有 checkout，分支 `phase1/caac-architecture` |
| `/workspace/Qt/6.11.1/gcc_64` | 官方 Qt SDK |
| `/workspace/sysroot` | 本地解压的 Debian 构建与运行依赖 |
| `/workspace/qgc-cloud` | 安装、激活、配置与验证辅助脚本 |
| `/workspace/build/qgc-baseline` | 原版 QGC Debug 构建及 CTest 输出 |
| `/workspace/reference` | 几何模块、视频元数据和采样帧 |

配置与编译命令：

```sh
bash /workspace/qgc-cloud/install.sh
source /workspace/qgc-cloud/activate.sh
bash /workspace/qgc-cloud/configure.sh
cmake --build /workspace/build/qgc-baseline --parallel 4
```

配置固定 Debug，保留 PX4、MAVLink 和视频能力；使用本地依赖路径，
开启原有测试，关闭 Linux 安装包生成和编译缓存。
首次构建并行 4，适配当前 4 CPU 配额；复用已有构建目录。
每个云任务本身已隔离，继续使用现有 checkout，不创建额外 worktree。

启动与测试命令：

```sh
source /workspace/qgc-cloud/activate.sh
cd /workspace/CAAC-QGC-GroundStation
python tools/run_tests.py --build-dir /workspace/build/qgc-baseline \
  --verify-only --timeout 120 --verbose
cd /workspace/build/qgc-baseline
CI=1 python /workspace/CAAC-QGC-GroundStation/.github/scripts/cmake_helper.py ctest \
  --junit-output qgc-cloud-junit.xml --ctest-output qgc-cloud-ctest.txt \
  --jobs 4 --build-type Debug --include-labels 'Unit|Integration' \
  --exclude-labels 'Flaky|Network'
```

使用仓库 runner 和 CI helper；无显示器时由它们启动 Xvfb。
启动检查实际加载 QML 根窗口并等待视频后端初始化，不能用进程存在代替通过。
没有需要保留运行的后台服务；每次运行检查时重新启动所需显示进程。
CTest 使用 `CI=1` 启用仓库已有的 CI 等待时间：例如 `TestTimeout::mediumMs()`
从本地 5 秒提高到 CI 10 秒；没有修改断言或关闭失败用例。

## 实际结果

- Qt 6.11.1 安装与版本查询：通过。
- 本地系统依赖下载、解压、CMake 配置：通过。
- GStreamer libav 插件检查及 5 帧测试图像到 fakesink 的 CPU 管线：通过。
- Xvfb 启动：通过。
- 参考几何模块原有 Debug CTest：1/1 通过；不是完整地理精度验证。
- Python 工具原有完整测试：首轮记录为 1021 通过、14 跳过，未重复扩展无关测试。
- 原版 QGC 应用编译：通过，产物 `/workspace/build/qgc-baseline/Debug/QGroundControl`。
- QGC 启动检查：通过，已加载 QML 根窗口并初始化视频后端。
- 首次 CTest：428 项选中，427 项通过、1 项失败；失败项为 `LinkManagerTest`，
  内部两项 DNS 等待断言超时。云端无效域名查询约 5 秒，越过本地 5 秒等待边界。
- 按仓库 CI 等待配置单独复测 `LinkManagerTest`：1/1 通过，约 33 秒。
- 使用 CI 配置的完整 CTest：**428/428 测试项通过，失败/禁用/跳过测试项均为 0**，
  耗时约 307 秒；这是 CTest 项统计，不能解读为内部所有平台用例均通过。
- 完整安装脚本复用：通过，已重新配置并完成增量编译；随后再次启动检查通过。
- 对首轮有内部跳过的 8 个测试类单独保留完整输出复核：8/8 类通过，
  仍有 34 项 QtTest 内部用例跳过；没有将这些项计为行为通过。

首次 CTest 的原始结果另存为构建目录中的 `qgc-cloud-initial-junit.xml`、
`qgc-cloud-initial-ctest.txt` 和 `qgc-cloud-initial-LastTest.log`，不覆盖失败记录。
首次运行 QtTest 内部有 34 行跳过，涉及 Windows/GPU 专用路径、缺少蓝牙硬件、
虚拟摇杆限制、协议不适用项以及原版 ArduSub MockLink 参数不匹配等。
这些跳过没有算成已验证的行为；CTest 测试类通过也不代表类内所有平台分支均执行。
复核输出与原因保存在 `qgc-cloud-platform-junit.xml`；完整套件输出保存在
`qgc-cloud-junit.xml` 和 `qgc-cloud-ctest.txt`。
原有默认输出截断会省略部分成功类的内部日志，不能据此认定无跳过。
完整套件注册 431 项，默认过滤排除了 3 项 Network 测试：
`QGCCachedFileDownloadTest`、`QGCFileDownloadTest`、`QGCNetworkHelperTest`。

本轮仓库修改仅有 3 份文档：更新 `phase1-analysis.md`，新增本文件与
`reference-review.md`。应用源码、测试、构建版本声明和锁文件未修改。
辅助脚本保留在 checkout 外，并在环境安装脚本中完整保存。

## 限制与配置保存

这台 Linux 机器不能提供原生 Windows MSVC 编译、安装包、升级卸载或实机飞行验证。
Xvfb/软件图形路径不等同于物理 GPU 视频性能验证。
当前阶段没有实现新的 CAAC 航点编辑功能，也没有运行 PX4 SITL 或真实飞控。
新增视频与 `.ld` 分析见 [reference-review.md](reference-review.md)。

完整安装脚本与启动说明已由配置工具确认保存到环境草稿，返回 `status=saved`、
`requires_publish=true`。用户需在环境设置审阅、保存并发布。
保存时只更新这两个字段，保留用户当前的网络配置及仓库成员设置。
草稿保存、当前实例验证、发布以及新任务恢复是不同状态；不能相互替代。
尚未验证发布后的新任务恢复，阶段一文档提交与分支目前保留在本地，没有推送。
