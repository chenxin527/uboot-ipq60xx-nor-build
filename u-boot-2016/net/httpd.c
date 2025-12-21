/*
 *	Copyright 1994, 1995, 2000 Neil Russell.
 *	(See License)
 *	Copyright 2000, 2001 DENX Software Engineering, Wolfgang Denk, wd@denx.de
 */

#include <common.h>
#include <command.h>
#include <net.h>
#include <asm/byteorder.h>
#include "httpd.h"

#include "../httpd/uipopt.h"
#include "../httpd/uip.h"
#include "../httpd/uip_arp.h"
#include <gl_api.h>
#include <asm/gpio.h>
#include <asm/arch-qca-common/smem.h>

// extern flash_info_t flash_info[];

static int arptimer = 0;

struct in_addr net_httpd_ip;

// start http daemon
void HttpdStart(void){
	struct uip_eth_addr eaddr;
	unsigned short int ip[2];
	ulong tmp_ip_addr = ntohl(net_ip.s_addr);
	printf( "HTTP server is starting at IP: %ld.%ld.%ld.%ld\n", ( tmp_ip_addr & 0xff000000  ) >> 24, ( tmp_ip_addr & 0x00ff0000  ) >> 16, ( tmp_ip_addr & 0x0000ff00  ) >> 8, ( tmp_ip_addr & 0x000000ff));
	eaddr.addr[0] = net_ethaddr[0];
	eaddr.addr[1] = net_ethaddr[1];
	eaddr.addr[2] = net_ethaddr[2];
	eaddr.addr[3] = net_ethaddr[3];
	eaddr.addr[4] = net_ethaddr[4];
	eaddr.addr[5] = net_ethaddr[5];
	// set MAC address
	uip_setethaddr(eaddr);

	uip_init();
	httpd_init();

	ip[0] =  htons((tmp_ip_addr & 0xFFFF0000) >> 16);
	ip[1] = htons(tmp_ip_addr & 0x0000FFFF);

	uip_sethostaddr(ip);

	printf("done set host addr 0x%x 0x%x\n", uip_hostaddr[0], uip_hostaddr[1]);
	// set network mask (255.255.255.0 -> local network)
	ip[0] = htons((0xFFFFFF00 & 0xFFFF0000) >> 16);
	ip[1] = htons(0xFFFFFF00 & 0x0000FFFF);

	net_netmask.s_addr = 0x00FFFFFF;
	uip_setnetmask(ip);

	// should we also set default router ip address?
	//uip_setdraddr();
	do_http_progress(WEBFAILSAFE_PROGRESS_START);
	webfailsafe_is_running = 1;
}

void HttpdStop(void){
	webfailsafe_is_running = 0;
	webfailsafe_ready_for_upgrade = 0;
	webfailsafe_upgrade_type = WEBFAILSAFE_UPGRADE_TYPE_FIRMWARE;
	do_http_progress(WEBFAILSAFE_PROGRESS_UPGRADE_FAILED);
}
void HttpdDone(void){
	webfailsafe_is_running = 0;
	webfailsafe_ready_for_upgrade = 0;
	webfailsafe_upgrade_type = WEBFAILSAFE_UPGRADE_TYPE_FIRMWARE;
	do_http_progress(WEBFAILSAFE_PROGRESS_UPGRADE_READY);
}

#ifdef CONFIG_MD5
#include <u-boot/md5.h>

void printChecksumMd5(int address, unsigned int size)
{
	void *buf = (void*)(address);
	int i = 0;
	u8 output[16];
	md5_wd(buf, size, output, CHUNKSZ_MD5);
	printf("md5 for %08x ... %08x ==> ", address, address + size);
	for (i = 0; i < 16; i++)
		printf("%02x", output[i] & 0xFF);
	printf("\n");
}
#else
void printChecksumMd5(int address, unsigned int size)
{
}
#endif

int do_http_upgrade(const ulong size, const int upgrade_type) {
	//为了能加入更多命令，加大了buf
	char buf[576];
	ulong flash_type = SMEM_BOOT_NO_FLASH;
	qca_smem_flash_info_t *sfi = &qca_smem_flash_info;
	int fw_type = check_fw_type((void *)WEBFAILSAFE_UPLOAD_RAM_ADDRESS);

	//printf checksum if defined
	printChecksumMd5(WEBFAILSAFE_UPLOAD_RAM_ADDRESS, size);

	if (getenv_ulong("flash_type", 16, SMEM_BOOT_NO_FLASH) == SMEM_BOOT_NORPLUSEMMC ||
		(sfi->flash_type == SMEM_BOOT_SPI_FLASH && sfi->flash_secondary_type == SMEM_BOOT_MMC_FLASH)
	) {
		flash_type = SMEM_BOOT_NORPLUSEMMC;
	}

	// include/gl_api.h
	// WEBFAILSAFE_UPLOAD_RAM_ADDRESS = 0x50000000 为了可以上传更大的固件，将上传地址从 0x44000000 改为 0x50000000 避免内存 crash 重启
	// FW_TYPE_NOR                这个是 SPI-NOR 的镜像
	// FW_TYPE_EMMC	              这个是 eMMC 的 GPT 分区表或镜像，只要开头有GPT信息即可
	// FW_TYPE_JDCLOUD	          这个是 JDCloud 官方原厂固件
	// FW_TYPE_UBI	              这个是 UBI 固件，eMMC 没有 UBI 固件
	// FW_TYPE_CDT                这个是 CDT 文件
	// FW_TYPE_ELF                这个是 ELF 文件 (除了 U-Boot 外, SBL1, QSEE, RPM, DEVCFG 也是 ELF 文件)
	// FW_TYPE_FACTORY_KERNEL6M	  这个是 Factory 格式的固件 (Kernel 大小: 6MB)
	// FW_TYPE_FACTORY_KERNEL12M  这个是 Factory 格式的固件 (Kernel 大小: 12MB)
	// FW_TYPE_FIT                这个是 FIT Image，包括 Factory Image 和 FIT uImage
	// FW_TYPE_MIBIB              这个是 SPI-NOR 的 MIBIB 分区表

	switch (upgrade_type) {
		case WEBFAILSAFE_UPGRADE_TYPE_FIRMWARE:
			if (flash_type == SMEM_BOOT_NORPLUSEMMC) {
				if (fw_type == FW_TYPE_FACTORY_KERNEL6M) {
					printf("\n\n******************************\n* FACTORY FIRMWARE UPGRADING *\n* FIRMWARE KERNEL SIZE: 6MB  *\n*  DO NOT POWER OFF DEVICE!  *\n******************************\n\n");
					sprintf(buf,
						"mw 0x%lx 0x00 0x200 && "
						"mmc dev 0 && flash 0:HLOS 0x%lx 0x%lx && flash rootfs 0x%lx 0x%lx && "
						"sf probe 0 && sf read 0x%lx 0xd0000 0x150 && mw.b 0x%lx 0x00 0x1 && mw.b 0x%lx 0x00 0x1 && mw.b 0x%lx 0x00 0x1 && flashupdate 0:BOOTCONFIG 0x%lx 0x150 && flashupdate 0:BOOTCONFIG1 0x%lx 0x150",
						// mw 0x%lx 0x00 0x200 擦除内存中上传文件后面的 512 字节，防止文件不够 512 字节写入文件后其他字符到 eMMC
						// 其实测试不擦除文件后内存，写入一些其他字符也可以正常启动
						(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+size),
						// factory.bin 由 kernel + rootfs 组成，其中 kernel 固定 6MB 大小
						(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
						(unsigned long int)0x600000,
						(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+0x600000),
						(unsigned long int)(size-0x600000),
						// 这部分改两个 BOOTCONFIG，启动系统 0，即 rootfs
						(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
						(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+0x80),
						(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+0x94),
						(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+0xA8),
						(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
						(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS);
				} else if (fw_type == FW_TYPE_FACTORY_KERNEL12M) {
					printf("\n\n******************************\n* FACTORY FIRMWARE UPGRADING *\n* FIRMWARE KERNEL SIZE: 12MB *\n*  DO NOT POWER OFF DEVICE!  *\n******************************\n\n");
					sprintf(buf,
						"mw 0x%lx 0x00 0x200 && "
						"mmc dev 0 && flash 0:HLOS 0x%lx 0x%lx && flash rootfs 0x%lx 0x%lx && "
						"sf probe 0 && sf read 0x%lx 0xd0000 0x150 && mw.b 0x%lx 0x00 0x1 && mw.b 0x%lx 0x00 0x1 && mw.b 0x%lx 0x00 0x1 && flashupdate 0:BOOTCONFIG 0x%lx 0x150 && flashupdate 0:BOOTCONFIG1 0x%lx 0x150",
						// mw 0x%lx 0x00 0x200 擦除内存中上传文件后面的 512 字节，防止文件不够 512 字节写入文件后其他字符到 eMMC
						// 其实测试不擦除文件后内存，写入一些其他字符也可以正常启动
						(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+size),
						// factory.bin 由 kernel + rootfs 组成，其中 kernel 固定 12MB 大小
						(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
						(unsigned long int)0xC00000,
						(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+0xC00000),
						(unsigned long int)(size-0xC00000),
						// 这部分改两个 BOOTCONFIG，启动系统 0，即 rootfs
						(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
						(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+0x80),
						(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+0x94),
						(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+0xA8),
						(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
						(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS);
				} else if (fw_type == FW_TYPE_JDCLOUD) {
					printf("\n\n*******************************\n* ORIGINAL FIRMWARE UPGRADING *\n*   DO NOT POWER OFF DEVICE!  *\n*******************************\n\n");
					sprintf(buf,
						"mmc dev 0 && imxtract 0x%lx hlos-0cc33b23252699d495d79a843032498bfa593aba && flash 0:HLOS $fileaddr $filesize && imxtract 0x%lx rootfs-f3c50b484767661151cfb641e2622703e45020fe && flash rootfs $fileaddr $filesize && imxtract 0x%lx wififw-45b62ade000c18bfeeb23ae30e5a6811eac05e2f && flash 0:WIFIFW $fileaddr $filesize && flasherase rootfs_data && "
						"sf probe 0 && sf read 0x%lx 0xd0000 0x150 && mw.b 0x%lx 0x00 0x1 && mw.b 0x%lx 0x00 0x1 && mw.b 0x%lx 0x00 0x1 && flashupdate 0:BOOTCONFIG 0x%lx 0x150 && flashupdate 0:BOOTCONFIG1 0x%lx 0x150",
						// 官方固件本身各个固件后面有填充 0，所以不用修改上传文件后的内存
						// 执行 imxtract 时不带目标地址，则不进行复制，但会修改环境变量 $fileaddr $filesize，可以直接用
						(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
						(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
						(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
						// 这部分改两个 BOOTCONFIG，启动系统 0，即 rootfs
						(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
						(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+0x80),
						(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+0x94),
						(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS+0xA8),
						(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
						(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS);
				} else {
					printf("\n\n* The upload file is NOT supported FIRMWARE!! *\n\n");
					return (-1);
				}
			} else {
				printf("\n\n* Update FIRMWARE is NOT supported for this FLASH TYPE yet!! *\n\n");
				return (-1);
			}
			break;
		case WEBFAILSAFE_UPGRADE_TYPE_UBOOT:
			printf("\n\n****************************\n*     U-BOOT UPGRADING     *\n* DO NOT POWER OFF DEVICE! *\n****************************\n\n");
			if (flash_type == SMEM_BOOT_NORPLUSEMMC || sfi->flash_type == SMEM_BOOT_SPI_FLASH) {
				if (fw_type == FW_TYPE_ELF) {
				sprintf(buf,
					"sf probe 0 && flashupdate 0:APPSBL 0x%lx $filesize && flashupdate 0:APPSBL_1 0x%lx $filesize",
					(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS),
					(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS));
				} else {
					printf("\n\n* The upload file is NOT supported UBOOT ELF!! *\n\n");
					return (-1);
				}
			} else {
				printf("\n\n* Update U-boot is NOT supported for this FLASH TYPE yet!! *\n\n");
				return (-1);
			}
			break;
		case WEBFAILSAFE_UPGRADE_TYPE_ART:
			printf("\n\n****************************\n*      ART  UPGRADING      *\n* DO NOT POWER OFF DEVICE! *\n****************************\n\n");
			if (flash_type == SMEM_BOOT_NORPLUSEMMC || sfi->flash_type == SMEM_BOOT_SPI_FLASH) {
				sprintf(buf,
					"sf probe 0 && flashupdate 0:ART 0x%lx $filesize",
					(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS));
			} else {
				printf("\n\n* Update ART is NOT supported for this FLASH TYPE yet!! *\n\n");
				return (-1);
			}
			break;
		case WEBFAILSAFE_UPGRADE_TYPE_IMG:
			printf("\n\n****************************\n*      IMG  UPGRADING      *\n* DO NOT POWER OFF DEVICE! *\n****************************\n\n");
			if (flash_type == SMEM_BOOT_NORPLUSEMMC) {
				if (fw_type == FW_TYPE_EMMC) {
					sprintf(buf,
						"mmc dev 0 && mmc erase 0x0 0x%lx && mmc write 0x%lx 0x0 0x%lx",
						(unsigned long int)((size-1)/512+1),
						(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
						(unsigned long int)((size-1)/512+1));
				} else if (fw_type == FW_TYPE_NOR) {
					sprintf(buf,
						"sf probe 0 && sf update 0x%lx 0x0 0x%lx",
						(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
						(unsigned long int)(size));
				} else if (fw_type == FW_TYPE_MIBIB) {
					sprintf(buf,
						"sf probe 0 && sf update 0x%lx 0xc0000 0x%lx",
						(unsigned long int)WEBFAILSAFE_UPLOAD_RAM_ADDRESS,
						(unsigned long int)(size));
				} else {
					printf("\n\n* The upload file is NOT supported EMMC IMG / NOR IMG!! *\n\n");
					return (-1);
				}
			} else {
				printf("\n\n* Update IMG is NOT supported for this FLASH TYPE yet!! *\n\n");
				return (-1);
			}
			break;
		case WEBFAILSAFE_UPGRADE_TYPE_CDT:
			printf("\n\n****************************\n*      CDT  UPGRADING      *\n* DO NOT POWER OFF DEVICE! *\n****************************\n\n");
			if (flash_type == SMEM_BOOT_NORPLUSEMMC || sfi->flash_type == SMEM_BOOT_SPI_FLASH) {
				if (fw_type == FW_TYPE_CDT) {
				sprintf(buf,
					"sf probe 0 && flashupdate 0:CDT 0x%lx $filesize && flashupdate 0:CDT_1 0x%lx $filesize",
					(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS),
					(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS));
				} else {
					printf("\n\n* The upload file is NOT supported CDT!! *\n\n");
					return (-1);
				}
			} else {
				printf("\n\n* Update CDT is NOT supported for this FLASH TYPE yet!! *\n\n");
				return (-1);
			}
			break;
		case WEBFAILSAFE_UPGRADE_TYPE_UIMAGE:
			printf("\n\n****************************\n*      UIMAGE BOOTING      *\n* DO NOT POWER OFF DEVICE! *\n****************************\n\n");
			if (fw_type == FW_TYPE_FIT) {
				sprintf(buf,
					"bootm 0x%lx",
					(unsigned long int)(WEBFAILSAFE_UPLOAD_RAM_ADDRESS));
			} else {
				printf("\n\n* The upload file is NOT supported FIT uImage!! *\n\n");
				return (-1);
			}
			break;
		default:
			printf("\n\n* NOT supported WEBFAILSAFE UPGRADE TYPE!! *\n\n");
			return (-1);
	}

	printf("Executing: %s\n\n", buf);

	return (run_command(buf, 0));
}

// info about current progress of failsafe mode
int do_http_progress(const int state) {
	//unsigned char i = 0;

	/* toggle LED's here */
	switch(state){
		case WEBFAILSAFE_PROGRESS_START:

			// // blink LED fast 10 times
			// for(i = 0; i < 10; ++i){
			// 	all_led_on();
			// 	milisecdelay(25);
			// 	all_led_off();
			// 	milisecdelay(25);
			// }
			led_off("power_led");
			led_on("blink_led");
			led_off("system_led");
			printf("HTTP server is ready!\n\n");
			break;

		case WEBFAILSAFE_PROGRESS_TIMEOUT:
			//printf("Waiting for request...\n");
			break;

		case WEBFAILSAFE_PROGRESS_UPLOAD_READY:
			printf("HTTP upload is done! Upgrading...\n");
			break;

		case WEBFAILSAFE_PROGRESS_UPGRADE_READY:
			led_off("power_led");
			led_off("blink_led");
			led_on("system_led");
			printf("HTTP upgrade is done! Rebooting...\n\n");
			mdelay(3000);
			break;

		case WEBFAILSAFE_PROGRESS_UPGRADE_FAILED:
			led_on("power_led");
			led_off("blink_led");
			led_off("system_led");
			printf("## Error: HTTP upgrade failed!\n\n");
			// // blink LED fast for 4 sec
			// for(i = 0; i < 80; ++i){
			// 	all_led_on();
			// 	milisecdelay(25);
			// 	all_led_off();
			// 	milisecdelay(25);
			// }

			// wait 1 sec
			// milisecdelay(1000);

			break;
	}

	return(0);
}

void NetSendHttpd(void){

	volatile uchar *tmpbuf = net_tx_packet;
	int i;

	for(i = 0; i < 40 + UIP_LLH_LEN; i++){

		tmpbuf[i] = uip_buf[i];
	}

	for(; i < uip_len; i++){

		tmpbuf[i] = uip_appdata[i - 40 - UIP_LLH_LEN];
	}

	eth_send(net_tx_packet, uip_len);
}

void NetReceiveHttpd(volatile uchar * inpkt, int len){

	memcpy(uip_buf, (const void *)inpkt, len);
	uip_len = len;
	struct uip_eth_hdr * tmp = (struct uip_eth_hdr *)&uip_buf[0];
	if(tmp->type == htons(UIP_ETHTYPE_IP)){

		uip_arp_ipin();
		uip_input();

		if(uip_len > 0){

			uip_arp_out();
			NetSendHttpd();
		}
	} else if(tmp->type == htons(UIP_ETHTYPE_ARP)){

		uip_arp_arpin();

		if(uip_len > 0){

			NetSendHttpd();
		}
	}
}

void HttpdHandler(void){
	int i;

	for(i = 0; i < UIP_CONNS; i++){
		uip_periodic(i);

		if(uip_len > 0){
			uip_arp_out();
			NetSendHttpd();
		}
	}

	if(++arptimer == 20){
		uip_arp_timer();
		arptimer = 0;
	}
}
