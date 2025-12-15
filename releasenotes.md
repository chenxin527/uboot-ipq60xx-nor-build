## 📋 更新日志

[点击此处](https://github.com/chenxin527/uboot-ipq60xx-nor-build/blob/main/changelog.md) 查看完整更新日志。

### ✨ 功能更新

- 添加对飞利浦 LY1800 和双渔 Y6010 的支持
- 调整相关代码，适配 NOR + eMMC
- 添加 flashupdate 命令
- SPI Web 刷写命令改用 flashupdate（减少闪存磨损）
- 支持刷写 eMMC 的 GPT 分区表或镜像，以及 SPI-NOR 的 MIBIB 分区表或镜像
- 雅典娜支持通过 Screen 键启动 HTTP Server

### 🐛 BUG 修复

- 修复无法启动超过 30MB 的 uImage 的问题

### 📢 其他更新

- 修改 bootcmd 为 bootipq（[点击此处](https://github.com/chenxin527/uboot-ipq60xx-nor-build/commit/b1682e34c931779f1f46ff4d210777e9b1256f1a) 查看详细说明）
- 修改 smeminfo 命令的输出内容
- 针对 SPI-NOR 修改 CDT 大小限制为 64KB (65536 Bytes)
- 检查上传的文件是否正确，不正确则返回 fail.html，并在串口控制台打印相关信息

## 📡 支持设备

原生 **NOR + eMMC** 机型：

- 双渔 Y6010
- 飞利浦 LY1800

可硬改为 **NOR + eMMC** 的机型（未测试）：
- 京东云太乙（RE-CS-07）
- 京东云亚瑟（RE-SS-01）
- 京东云雅典娜（RE-CS-02）
- 连我 NN6000 V1
- 连我 NN6000 V2
- 红米 AX5 JDCloud（RA50）

## 📸 网页截图

[点击此处](https://github.com/chenxin527/uboot-ipq60xx-nor-build/blob/5bda947116b50186274a11367528f257d4f52c56/screenshots.md) 查看所有网页截图。

![uboot-index-page](https://github.com/chenxin527/uboot-ipq60xx-nor-build/blob/42af5d1c143ea42dd02023db5bef45f56677674f/screenshots/uboot-index-page.png)
