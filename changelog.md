## 📋 更新日志

### 📅 [xx.xx.xx](https://github.com/chenxin527/uboot-ipq60xx-nor-build/releases/tag/)

#### ✨ 功能更新

- 添加对飞利浦 LY1800 和双渔 Y6010 的支持
- 调整相关代码，适配 NOR + eMMC
- 添加 flashupdate 命令
- SPI Web 刷写命令改用 flashupdate（减少闪存磨损）
- 支持刷写 eMMC 的 GPT 分区表或镜像，以及 SPI-NOR 的 MIBIB 分区表或镜像
- 雅典娜支持通过 Screen 键启动 HTTP Server

#### 🐛 BUG 修复

- 修复无法启动超过 30MB 的 uImage 的问题

#### 📢 其他更新

- 修改 bootcmd 为 bootipq（[点击此处](https://github.com/chenxin527/uboot-ipq60xx-nor-build/commit/b1682e34c931779f1f46ff4d210777e9b1256f1a) 查看详细说明）
- 修改 smeminfo 命令的输出内容

### 📅 [25.11.13](https://github.com/chenxin527/uboot-ipq60xx-nor-build/releases/tag/25.11.13-22.43.36)

#### ✨ 功能更新

- 添加对连我 NN6000 V1 & V2 的支持
- 支持通过 WPS 键启动 HTTP Server
- Ctrl+C 中断 web failsafe mode 后不会重启，而是进入 U-Boot 命令行模式

#### 🌐 网页更新

- 将标签页图标替换为 Unicode 图标

#### 📢 其他更新

- 为每个设备编译单独的 U-Boot
- 填充 U-Boot 至 640KB (655360 Bytes)
- 将通过网页刷写的 U-Boot 大小限制调回 640KB

### 📅 [25.11.03](https://github.com/chenxin527/uboot-ipq60xx-nor-build/releases/tag/25.11.03-17.23.47)

#### 🌐 网页更新

- 调整网页布局
- 删除主区域页脚，修改侧边栏页脚样式
- 为侧边栏添加图标和鼠标悬停动画
- 修改提示框加粗文字的字体大小

#### 📢 其他更新

- 填充 U-Boot 至 1024KB（1048576 Bytes）

### 📅 [25.10.25](https://github.com/chenxin527/uboot-ipq60xx-nor-build/releases/tag/25.10.25-15.18.56)

#### ✨ 功能更新

- 添加对京东云太乙的支持

#### 🌐 网页更新

- 修改部分网页提示信息

### 📅 [25.10.12](https://github.com/chenxin527/uboot-ipq60xx-nor-build/releases/tag/25.10.12-14.44.11)

#### ✨ 功能更新

- 支持启动 uImage

#### 🌐 网页更新

- 添加标签页图标
- 修改部分网页的提示信息

#### 📢 其他更新

- 修改更新固件时的串口输出日志

### 📅 [25.09.15](https://github.com/chenxin527/uboot-ipq60xx-nor-build/releases/tag/25.09.15-23.49.18)

#### ✨ 功能更新

- 支持刷写内核大小为 12MB 的固件
- 支持刷写更大的 U-Boot 文件（不超过 1024KB）

#### 🌐 网页更新

- 将网页翻译为简体中文
- 添加侧边导航栏，切换为深色背景
