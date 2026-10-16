# 地面站离线永久激活

首次启动只显示激活窗口。激活成功后才初始化主界面、视频、遥控器和飞控连接。授权与当前电脑绑定；复制授权文件到另一台电脑不能激活。重启时重新验证签名，不保存可直接修改的“已激活”标记。

## 签发工具

显式构建独立工具，默认构建和安装包均不包含此工具：

```sh
cmake --build build --config Release --target CaacLicenseTool
```

Windows Release 工作流另外生成 `CaacLicenseTool-Windows-x64` 构建产物，内含可完整解压使用的工具 ZIP、Qt/VC 运行库和中文说明。CI 使用临时测试密钥检查签发、拒绝覆盖及错误机器码；临时私钥随即删除，不进入压缩包。正式签发私钥仍由签发人单独保管。

未授权的 Windows 构建与安装后启动验证，检查真实激活窗口持续显示、关闭后正常退出且不产生授权文件；不绕过授权进入主界面。已授权启动与重启流程由本地真实签发验证及原生 UI 回归覆盖。

首次配置发行商密钥时，在自己的离线电脑执行一次（Windows 使用 `CaacLicenseTool.exe`）：

```sh
CaacLicenseTool keygen --private-key /secure/issuer-private.hex --public-key public-key.hex
```

把生成的公钥文件放入 `custom/activation/public-key.hex`，然后重新构建软件。私钥只由签发人保管，另行备份，不得提交到仓库、打入安装包或发给使用者。当前仓库内的公钥与本次单独交付的签发私钥配对；不要重新生成密钥覆盖它，否则新软件将不再接受旧密钥签发的激活码。

用户把激活窗口显示的机器码发给签发人。签发人执行：

```sh
CaacLicenseTool issue --private-key /secure/issuer-private.hex --machine CAAC-01234567-89ABCDEF-01234567-89ABCDEF --output activation.txt
```

把 `activation.txt` 中的激活码交给用户粘贴即可。签发过程无需网络，签发文件和密钥文件不会覆盖已有文件。

## 授权保存与更新

授权保存在 Qt `GenericDataLocation` 下的 `CAAC/GroundStation/activation.license`。它独立于软件偏好设置，正常升级及重置偏好设置后仍有效。机器码来自操作系统机器标识的单向摘要，不显示或上传原始机器标识。重装系统导致机器标识变化后，需要重新签发。

永久授权不检查日期，也无需联网。修改激活码、换机或更换验码公钥都会使验证失败。没有配置有效公钥或无法获取机器标识时，软件保持锁定。

## 签名格式与依赖

使用标准 Ed25519 签名，Monocypher 4.0.2 的 SHA-512 Ed25519 实现；下载内容由 SHA-256 固定。上游依赖采用 `BSD-2-Clause OR CC0-1.0`，本项目按 CC0 使用。上游许可证：<https://github.com/LoupVaillant/Monocypher/blob/4.0.2/LICENCE.md>。

机器码包含 128 位摘要。激活码前缀为 `CAAC1.`，后接机器摘要与 64 字节签名的 Base64URL 编码。签名涵盖产品标识、协议版本、永久授权类型和机器摘要。签名私钥不参与客户端运行。
