本仓库修改自：https://github.com/lgs2007m/uboot-ipq60xx-build

u-boot-2016 源代码基于：https://github.com/gl-inet/uboot-ipq60xx

**eMMC** 机型 U-Boot 仓库：https://github.com/chenxin527/uboot-ipq60xx-emmc-build

**NAND** 机型 U-Boot 仓库：https://github.com/chenxin527/uboot-ipq60xx-nand-build

## 适配设备

本项目已适配以下 IPQ60xx **NOR + eMMC** 机型：

- 飞利浦 LY1800
- 双渔 Y6010

以下机型可硬改为 **NOR + eMMC**（未测试）：
- 京东云太乙（RE-CS-07）
- 京东云亚瑟（RE-SS-01）
- 京东云雅典娜（RE-CS-02）
- 连我 NN6000 V1
- 连我 NN6000 V2
- 红米 AX5 JDCloud（RA50）

## 机型说明

NOR + eMMC 机型典型分区布局如下：

NOR 分区表（双渔 Y6010）：
```
IPQ6018# smeminfo
flash_type:             0x6
flash_index:            0x0
flash_chip_select:      0x0
flash_block_size:       0x10000
flash_density:          0x1000000
flash_secondary_type:   0x5
partition table offset: 0x0
No.: Name             Attributes            Start             Size
  0: 0:SBL1           0x0000ffff              0x0          0xc0000
  1: 0:MIBIB          0x001040ff          0xc0000          0x10000
  2: 0:BOOTCONFIG     0x001040ff          0xd0000          0x20000
  3: 0:BOOTCONFIG1    0x001040ff          0xf0000          0x20000
  4: 0:QSEE           0x0000ffff         0x110000         0x1a0000
  5: 0:QSEE_1         0x0000ffff         0x2b0000         0x1a0000
  6: 0:DEVCFG         0x0000ffff         0x450000          0x10000
  7: 0:DEVCFG_1       0x0000ffff         0x460000          0x10000
  8: 0:RPM            0x0000ffff         0x470000          0x40000
  9: 0:RPM_1          0x0000ffff         0x4b0000          0x40000
 10: 0:CDT            0x0000ffff         0x4f0000          0x10000
 11: 0:CDT_1          0x0000ffff         0x500000          0x10000
 12: 0:APPSBLENV      0x0000ffff         0x510000          0x10000
 13: 0:APPSBL         0x0000ffff         0x520000          0xa0000
 14: 0:APPSBL_1       0x0000ffff         0x5c0000          0xa0000
 15: 0:ART            0x0000ffff         0x660000          0x40000
```

eMMC 分区表（双渔 Y6010）：
```
IPQ6018# mmc part

Partition Map for MMC device 0  --   Partition Type: EFI

Part    Start LBA       End LBA         Name
        Attributes
        Type GUID
        Partition GUID
  1     0x00000022      0x00003021      "0:HLOS"
        attrs:  0x0000000000000000
        type:   b51f2982-3ebe-46de-8721-ee641e1f9997
        guid:   9798feca-7799-1058-4b69-854f5c698859
  2     0x00003022      0x00006021      "0:HLOS_1"
        attrs:  0x0000000000000000
        type:   a71da577-7f81-4626-b4a2-e377f9174525
        guid:   f3dbbfd8-8edc-fde3-91dd-a653b785d4ea
  3     0x00006022      0x00024021      "rootfs"
        attrs:  0x0000000000000000
        type:   98d2248d-7140-449f-a954-39d67bd6c3b4
        guid:   de1de159-cc70-db2d-c79e-41068975925c
  4     0x00024022      0x00026021      "0:WIFIFW"
        attrs:  0x0000000000000000
        type:   5911fd72-35be-424e-975d-69c957ad3a43
        guid:   55cb8126-e549-80c8-4d69-113675a44df1
  5     0x00026022      0x00044021      "rootfs_1"
        attrs:  0x0000000000000000
        type:   5647b280-dc2a-485d-9913-cf53ac40fa32
        guid:   d77ecb94-9116-eaea-f0da-ddaf5c296cf7
  6     0x00044022      0x00046021      "0:WIFIFW_1"
        attrs:  0x0000000000000000
        type:   a640a4e3-6aeb-4d83-81a0-dfeae6b7d1a5
        guid:   62e4e062-910d-8bfd-3fdd-af2aad4da687
  7     0x00046022      0x000be021      "rootfs_data"
        attrs:  0x0000000000000000
        type:   ab1760da-a8bb-4d6f-98d2-9ad3ab9009cd
        guid:   20ee14f5-5383-ce48-4d7e-a207d8ba7776
  8     0x000be022      0x008be021      "app"
        attrs:  0x0000000000000000
        type:   ac176067-a1bc-4c8f-98d1-9bd3ab9011cd
        guid:   7845e0f3-80b3-ae1c-866c-57d9fe68f690
  9     0x008be022      0x0e2be021      "data1"
        attrs:  0x0000000000000000
        type:   ad1760ff-a2bd-4e8f-98d3-9cd3ab9022cd
        guid:   6103e817-6117-d089-5243-eb51f9165c88
 10     0x0e2be022      0x0e2d79b9      "data2"
        attrs:  0x0000000000000000
        type:   ae1760cc-a3bf-536d-98d4-9dd3ab9033cd
        guid:   39c6eebe-f67b-a61a-afd7-5d610d3ea40e
 11     0x0e2d79ba      0x0e6d79b9      "swap"
        attrs:  0x0000000000000000
        type:   af1760cc-a3bf-536d-98d4-9dd3ab9044cd
        guid:   134e417f-570b-b302-14eb-46bb7f2f4716
```

## 编译方法

### 本地编译

1. 配置编译环境

```bash
# 编译环境：Ubuntu
# mbn 脚本使用 python3 运行，请安装并切换到 python3
sudo apt update
sudo apt install -y python3
sudo apt install -y build-essential device-tree-compiler
```

2. 克隆此仓库

```bash
git clone https://github.com/chenxin527/uboot-ipq60xx-nor-build.git
```

3. 编译你需要的设备

```
用法: ./build.sh [选项]

选项:
  help                    显示此帮助信息
  setup_env               仅设置编译环境
  check_file_size <文件>  检查并调整文件大小至 640KB (655360 Bytes)
  clean_cache             清理编译过程中产生的缓存
  build_ly1800            编译 Philips LY1800
  build_y6010             编译 SY Y6010
  build_re-cs-02          编译 JDCloud AX6600 (Athena)
  build_re-cs-07          编译 JDCloud ER1
  build_re-ss-01          编译 JDCloud AX1800 Pro (Arthur)
  build_nn6000-v1         编译 Link NN6000 V1
  build_nn6000-v2         编译 Link NN6000 V2
  build_ax5-jdcloud       编译 Redmi AX5 JDCloud
  build_all               编译所有支持的设备
```

### 云编译

Fork 本仓库后使用 GitHub Actions 云编译。

## 文件说明

编译生成文件所在目录：bin/

日志文件：log-\${编译时间}.txt

U-Boot 文件：uboot-ipq60xx-nor-\${设备型号}-\${版本号}.bin

U-Boot 截图示例（[点击此处](./screenshots.md) 查看所有网页截图）：

![uboot-index-page](./screenshots/uboot-index-page.png)

## 功能介绍

### 网址说明
| 功能        | 网址                            | 备注                                |
| :---------- | :----------------------------- | :--------------------------------- |
| 更新固件     | http://192.168.1.1             | 支持内核大小为 6MB 和 12MB 的固件更新 |
| 更新 ART    | http://192.168.1.1/art.html    | ART 包含路由器网卡 MAC 及无线校准数据 |
| 更新 CDT    | http://192.168.1.1/cdt.html    | CDT 文件不得小于 10 KB（10240 Bytes） |
| 更新 IMG    | http://192.168.1.1/img.html    | 支持刷写 eMMC 的 GPT 分区表或镜像，以及 SPI-NOR 的 MIBIB 分区表或镜像 |
| 更新 U-Boot | http://192.168.1.1/uboot.html  | U-Boot 大小不能超过 640 KB（655360 Bytes）|
| 启动 uImage | http://192.168.1.1/uimage.html | Initramfs uImage，可直接上传至内存并启动 |

> [!NOTE]
>
> 因 U-Boot HTTP 服务器限制，不支持上传 10KB（10240 Bytes）以下的文件。若要上传的文件不足 10KB，请使用十六进制编辑器在文件末尾填充空数据（0x0），但不要超过其所在分区大小。此 U-Boot 支持上传的所有文件中，只有 CDT 文件有效数据不足 10KB，特此说明。

> [!TIP]
>
> uImage (U-Boot Image) 即所谓 “内存固件”。在 USB 9008 救砖模式下，利用 “启动 uImage” 功能可上传并启动临时 OpenWrt 固件，在临时固件中可使用预置的各种工具进行备份分区、救砖恢复等操作。
>
> [点击此处](http://example.com) 获取 USB 9008 救砖教程及相关文件。
>
> [点击此处](https://github.com/chenxin527/OWRT-BUILDS/releases?q=UIMAGE-ONLY-IPQ60XX&expanded=true) 获取经测试可正常使用的 uImage。

### 固件说明

双渔 Y6010 / 飞利浦 LY1800 除了支持刷写各自的专用固件，还支持刷写京东云亚瑟的固件。

因为亚瑟的固件未包含 NOR 的驱动，无法从 NOR 中读取 ART 分区，所以若要刷写亚瑟的固件，需将 ART 分区复制到 eMMC 上。

方法：将亚瑟的分区表刷到 eMMC 上，再将双渔 Y6010 / 飞利浦 LY1800 的原机 ART 刷到 eMMC 的 ART 分区即可。

此外，刷写亚瑟固件后无法直接在 SSH 下操作 U-Boot 环境变量，需刷写专用固件或通过 TTL 在串口控制台中修改。

### 进 Web 刷机界面

所有机型都支持通过 RESET 键进入 U-Boot Web 刷机界面。

以下有 WPS 键的机型还支持通过 WPS 键进入 U-Boot Web 刷机界面：

- 京东云亚瑟（原厂叫 JOY 键）
- 京东云雅典娜（原厂叫 JOY 键）
- 连我 NN6000 V1（原厂叫 Reboot 键）
- 连我 NN6000 V2（原厂叫 Reboot 键）

此外，京东云雅典娜还支持通过 SCREEN 键进入 U-Boot Web 刷机界面。

### 其他

U-Boot 下不区分 LAN / WAN，任意网口均可进入 Web 刷机界面。

按住 RESET / WPS / SCREEN 键后上电，等待 LED 闪烁 5 次后即可进入 U-Boot Web 刷机界面。

## 注意事项

### 连我 NN6000 V1 的 U-Boot 未测试

连我 NN6000 V1 的 U-Boot 未测试过，因为没有机器。

V1 和 V2 的 U-Boot 只是网口配置不同，其他都一样。若发现 V1 的 U-Boot 不能正常使用，可刷写 V2 的 U-Boot 测试，看看每个网口是否能正常进 Web。每换一个网口都要断电并重新按 RESET / WPS 键启动 HTTP Server，不要在 HTTP Server 已经启动的时候换网口，这样是进不了 Web 的。

### bootipq 失败后执行 httpd 出错

bootipq 失败后执行 httpd 会出现以下错误：

```
HTTP server is ready!

Data will be downloaded at 0x50000000 in RAM
Upgrade type: firmware
Upload file size: 57282710 bytes
Loading: #######################################
         .......................................
         #######################

done!
data abort
pc : [<4a448310>]          lr : [<4a462c0f>]
reloc pc : [<4a448310>]    lr : [<4a462c0f>]
sp : 4a27f844  ip : 000031b5     fp : 00000a01
r10: 4a487c5c  r9 : 4a27fea0     r8 : 4a487c74
r7 : 4a487c6c  r6 : 0000d250     r5 : 4a48767e  r4 : 0000ba05
r3 : 000005a6  r2 : 4a462c0f     r1 : 000005a6  r0 : 00000000
Flags: nzCv  IRQs off  FIQs off  Mode SVC_32
Resetting CPU ...
```

这是因为 bootipq 命令修改了运行环境，导致执行 httpd 命令刷写固件失败。

若遇到 bootipq 失败的情况，请断电重启路由器以重置运行环境，在 bootipq 执行前按 RESET 键重新进入 Web 界面刷写固件，或者打断 U-Boot 自动启动流程后在串口控制台手动执行相关命令。

bootipq 常见失败原因：U-Boot 无法正常读取 0:HLOS 分区；0:HLOS 分区中存储的不是正确的固件内核数据。
