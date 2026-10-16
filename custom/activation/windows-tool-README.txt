地面站永久激活码签发工具（Windows x64）

完整解压压缩包，保留 EXE 与所有 DLL 在同一目录。
在此目录打开 PowerShell，执行：

.\CaacLicenseTool.exe issue --private-key "D:\安全备份\issuer-private.hex" --machine "用户提供的完整机器码" --output "activation.txt"

将 activation.txt 内的完整激活码发给对应用户。每次使用新的输出文件名；已有文件不会覆盖。
签发人私钥须单独保存，压缩包内不含私钥。public-key.hex 是验码公钥，可公开。
本工具和私钥仅供软件所有者签发授权，无需发给普通用户。
当前安装包已配置对应公钥，请使用单独交付的配对私钥，不要重新 keygen 替换密钥。
换电脑或重装系统导致机器码改变后，需要重新签发。授权永久有效，无需联网。
