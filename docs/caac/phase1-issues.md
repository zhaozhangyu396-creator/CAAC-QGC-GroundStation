# 阶段一问题处理与验收记录

日期：2026-10-10（Asia/Shanghai）。分支：`phase1/caac-architecture`。
原版源码基线：`b42d1d8fd3af14e56da03f9973ad771317b3126f`。
本轮交付分析文档、Windows CI元数据修复与checkout外的验证探针，
没有修改QGC应用源码或实现CAAC编辑功能。
完整模块结构和具体文件计划见 [phase1-analysis.md](phase1-analysis.md)，
参考证据见 [reference-review.md](reference-review.md)。

## 问题状态

| 问题 | 处理与证据 | 状态 |
| --- | --- | --- |
| Qt下载403 | 用户调整网络后，官方Qt6.11.1按仓库配置安装；保留TLS/包校验 | 已解决 |
| 没有管理员权限安装系统依赖 | 签名apt索引、下载包、解压至workspace sysroot；不改系统包数据库 | 已解决 |
| EGL最终链接失败 | 补齐libopengl-dev后重新配置/链接；没有关闭视频 | 已解决 |
| DNS测试等待边界 | 使用仓库CI=1的既有等待配置；原失败记录保留，完整428项复测通过 | 已解决 |
| 新启动环境能否复用 | 分支/文档提交/SDK/脚本恢复；重新配置、增量编译、启动及6项重点回归通过 | 已验证 |
| QGC版本与开发基线 | 固定已验证开发SHA；官方v5.2.0-dev标记已校验且可达，不冒充稳定发布版 | 已确定 |
| 地图/任务接口定位 | PlanView、PlanViewRightPanel、MissionController、PlanMasterController、SimpleMissionItem/Fact接口已定位 | 已完成分析 |
| 定制静态QML接入可行性 | 相同URI/资源前缀的URL拦截与回退探针，Debug/Release各2项通过 | 机制已验证；完整规划页接入留阶段二 |
| 几何V1能否直接集成 | 球面误差和非法属性/极大角风险已复现，决定复用QGC已有WGS84后端 | 已完成审查与方案选择；适配层留阶段三 |
| 参考行为与原需求冲突 | 四轮答复明确鼠标、夹角/旋转边界、多步undo/无redo、仅.ld和可选桌面 | 本轮问题均已确认；CAAC仍保留原要求的undo/redo和自有文件格式 |
| Windows工作流不可见/404 | 用户启用后可触发；原版x64 Configure/Build与10项portable测试成功 | 原版编译验证通过；完整打包另记 |
| Windows打包版本0.0.0 | 首轮三条编译成功后打包失败；CI补齐官方标记，ARM64-cross完整复测成功 | 版本修复已验证；x64/ARM64复测仍运行 |
| 大ZIP/EXE工具32MiB限制 | 文件未读取，无本地可读解压内容 | 资料限制仍在；不阻塞自有实现方案 |
| 航点.ld无法获取字段 | 原件已检查：分段Base64与高熵数据，未确认编码/加密规则 | 不能声称兼容导入 |

## 本次实际编译与测试

原版QGC配置与增量编译、恢复后的启动：通过。
Qt版本6.11.1，CMake3.31.10，GCC14.2.0，Ninja1.13；保持PX4、MAVLink和视频功能。

```sh
source /workspace/qgc-cloud/activate.sh
bash /workspace/qgc-cloud/configure.sh
cmake --build /workspace/build/qgc-baseline --parallel 4
cd /workspace/CAAC-QGC-GroundStation
python tools/run_tests.py --build-dir /workspace/build/qgc-baseline \
  --verify-only --timeout 120 --verbose
cd /workspace/build/qgc-baseline
CI=1 python /workspace/CAAC-QGC-GroundStation/.github/scripts/cmake_helper.py ctest \
  --build-type Debug --jobs 2 \
  --tests-regex '^(GeoTest|MissionControllerTest|PlanMasterControllerTest|LinkManagerTest|GStreamerTest|PX4VehicleConfigUITest)$' \
  --junit-output phase1-restored-junit.xml --ctest-output phase1-restored-ctest.txt
```

实际结果：重点CTest **6/6通过，约64.10秒**。
本轮未修改应用源码，没有再次重复此前已通过的完整428项或Python工具全套；
历史全套结果和内部跳过限制见 [cloud-validation.md](cloud-validation.md)。
编译日志：`/tmp/phase1-restored-configure.log`、`/tmp/phase1-restored-build.log`；
启动日志：`/tmp/phase1-restored-rebuilt-boot.log`；
JUnit：`/workspace/build/qgc-baseline/phase1-restored-junit.xml`。
补齐官方Git标记后再次重新配置、增量编译和启动：通过；
日志为 `/tmp/phase1-version-{configure,build,boot}.log`。

独立探针目录：`/workspace/phase1-checks`，包含CMake、C++/QML源文件、运行脚本与README。
执行 `bash /workspace/phase1-checks/run.sh`，Debug与Release分别**4/4通过**。
检查采用运行时require，Release检查不会因NDEBUG消失。
两种探针链接原版QGC Debug GeographicLib库，不能算作QGC Release构建验证。

| 探针 | 已验证内容 | 限制 |
| --- | --- | --- |
| Geometry.Wgs84KnownAnswers | 公开已知向量、赤道解析弧长、日期线/极点/极大角 | 是计算后端验证，不是CAAC适配层实现 |
| Geometry.ReferenceInvariants | V1局部旋转往返、精确中心固定、现有属性保留、非法插入索引 | 不含完整MAVLink字段，也不消除V1的其他风险 |
| CustomBuild.BaselineQml | 原静态模块加载、缺少覆盖时回退 | 最小Qt探针，不是完整QGC PlanView |
| CustomBuild.InterceptedQml | 存在覆盖资源时加载定制QML | 未测试实际地图、鼠标事件或控制器绑定 |

日志：`/tmp/phase1-checks-final.log`；JUnit位于
`/workspace/build/phase1-checks-debug/phase1-checks-junit.xml`及对应release目录。
可复现资料包：`/workspace/phase1-validation.zip`，包含探针源文件/运行脚本、
JUnit及本次Linux构建/启动日志；不包含凌动资源或修改后的V1代码。

## 几何风险的量化结果

下表是相同起点、方位角、距离下V1球面端点与WGS84端点的测地距离差，单位米。

| 起点与操作 | 差异 |
| --- | ---: |
| 赤道向北1000米 | 5.583020413 |
| 赤道向东1000米 | 1.118849497 |
| 北纬30°向北1000米 | 3.080729386 |
| 北纬30°向东1000米 | 1.957636229 |
| 北纬80°向北5000米 | 20.909712318 |
| 北纬30°、东经179.999°向东1000米 | 1.957636227 |

诊断还实际复现：insertRelative接受模板中的NaN高度/速度；
destination接受极大有限方位角后输出非有限经纬度。
V1测试通过和成功复现缺陷是不同结论，原件没有被修复。
正式实现需要输入归一化、二维中心独立于高度、完整任务属性验证和失败原子性。

任意中心两点定向的例子：中心(80°,114°)，A(80.01°,114.01°)，
B(80.02°,114.05°)，目标90°。直接将目标与原A→B的方位差作为统一旋转角，
最终A→B仍偏差约 **0.042379847°**；必须求解并复核变换后的方位约束。
诊断CSV：`/workspace/phase1-checks/geometry-findings.csv`。
计算库已知向量检查的10微米门槛和局部往返1毫米门槛是工程验证值，
不是自行认定的CAAC官方考试精度。

## Windows原版验证

已成功触发现有Windows工作流，参数build_type=Debug、debug_runner=false，
ref=master，head_sha与本报告原版基线一致。
运行：[37961938331](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/37961938331)。
工作流原有矩阵包含amd64、arm64、arm64-cross；本项目优先验收amd64。
Debug运行不执行Release的发布步骤。

首轮ARM64/ARM64-cross的Configure与Build均通过；Create Installer失败于
`cmake/install/ValidatePackageVersion.cmake:18`：缺少标签导致版本0.0.0。
首轮amd64 Configure与Build通过，实际编译1552秒；portable CTest **10/10通过，1.68秒**。
三条分支的最终工作流结论均为failure，原因均为Create Installer的0.0.0校验失败；
不能将“源码编译通过”写成“整个Windows工作流/安装包通过”。
实际x64工具链为MSVC19.44.35229.0、Qt6.11.1、GStreamer1.28.4。
首轮完整日志为 `/tmp/phase1-windows-initial-logs.zip`；
交叉编译文本日志为 `/tmp/phase1-windows-arm64-cross.log`；
x64 JUnit和portable输出下载至 `/workspace/build/windows-baseline`。

修复提交：`06138ce19e44413e02203571da3669f2441163ad`，仅新增Windows工作流9行。
对本仓库从官方获取v5.2.0-dev，核对解引用提交并检查是HEAD祖先，
保留打包版本校验；标签只进入构建checkout，不发布到远端。
不改QGC应用源码、依赖锁或版本声明，不切换至另一份应用源码。
复测：[37965262779](https://github.com/zhaozhangyu396-creator/CAAC-QGC-GroundStation/actions/runs/37965262779)，
ref=phase1/caac-architecture，head_sha=06138ce19e44413e02203571da3669f2441163ad，参数仍为Debug。
三条分支的官方标记校验步骤已实际通过；ARM64-cross的Configure、Build、
Create Installer及整个任务均success，已验证版本修复与NSIS打包。
修复后日志：`/tmp/phase1-windows-arm64-cross-fixed.log`。
x64和原生ARM64完整复测仍在运行，不能将交叉分支通过写成两者也通过。
阶段一的原版x64编译验证已由首轮结果完成；完整复测可在上述运行链接继续查看。
Release启动、安装/升级/卸载以及安装包发布未执行，属于后续发布阶段验收。

之前工作流为空、触发404与Actions设置查询403属于启用前历史记录；
用户启用后已发现现有Windows工作流，不需要再次要求启用或提供Token。

## CI修改检查与原有问题

Windows YAML检查通过：check-yaml、actionlint1.7.12、zizmor、yamllint。
actionlint的pre-commit源码安装未能运行（本机go不是所需Go编译工具）；
改用相同固定版本的官方二进制直接检查，通过；下载已核对官方SHA256。
相关现有CI脚本检查实际为**98通过、2失败**，没有称全部通过。
日志：`/tmp/phase1-ci-contract-tests.log`。

失败分别为test_workflow_security_policy的dependency-review断言仍期待
`gradle/actions/wrapper-validation@v4`，实际原版已用v6；
test_cache_contracts期待`actions/cache/restore@v5`，原版实际已用v6。
在基线b42d1d8fd3的文件快照中单独重跑，同样2项失败，
日志：`/tmp/phase1-ci-baseline-failures.log`。这些文件本轮未改，
不是Windows元数据步骤引入；本轮记录问题，未改原有断言或倒退Action版本。

## 修改范围与后续验收

本轮仓库文件：Windows工作流新增元数据获取/校验步骤；
更新phase1-analysis.md、reference-review.md、cloud-validation.md，新增本文件。
独立探针位于checkout外。
QGC应用、原有测试、构建版本、依赖锁文件、V1原件没有修改。
文档检查实际通过8项：合并标记、大文件、大小写冲突、尾部空白、
文件结尾、混合换行、markdownlint、typos；另有git diff --check通过。
markdownlint首次安装因npm默认缓存目录不可写而失败，
将npm_config_cache指向workspace缓存后安装与检查通过；未关闭包完整性校验。
文档提交沿用阶段一分支；阶段二从原生编辑入口开始，
使用独立分支/明确提交，不把本次分析交付称作航点编辑器交付。

阶段一的源码分析、模块/文件计划、原版Linux与Windows x64编译验证均有实际证据。
当前遗留：两项原有CI版本断言、完整Windows打包复测的其余任务、
尚未解析的.ld、大附件读取限制。
后续阶段仍需实际验证完整定制规划页接入、SITL/实机、GPU视频、
Release启动与安装/升级/卸载；没有把这些未来工作算作阶段一功能交付。
自有.caacroute使用完整QGC plan JSON封装，不以解码.ld作为自身保存/加载前提。
