#include <common.h>
#include <cli.h>
#include <gl_api.h>
#include <asm/gpio.h>
#include <fdtdec.h>

DECLARE_GLOBAL_DATA_PTR;

void led_toggle(const char *gpio_name)
{

	int node, value;
	unsigned int gpio;
	node = fdt_path_offset(gd->fdt_blob, gpio_name);
	if (node < 0) {
		printf("Could not find %s node in fdt\n", gpio_name);
		return;
	}
	gpio = fdtdec_get_uint(gd->fdt_blob, node, "gpio", 0);
	if (gpio < 0) {
		printf("Could not find %s node's gpio in fdt\n", gpio_name);
		return;
	}

	value = gpio_get_value(gpio);
	value = !value;
	gpio_set_value(gpio, value);
}

void led_on(const char *gpio_name)
{
	int node;
	unsigned int gpio;
	node = fdt_path_offset(gd->fdt_blob, gpio_name);
	if (node < 0) {
		printf("Could not find %s node in fdt\n", gpio_name);
		return;
	}
	gpio = fdtdec_get_uint(gd->fdt_blob, node, "gpio", 0);
	if (gpio < 0) {
		printf("Could not find %s node's gpio in fdt\n", gpio_name);
		return;
	}

	gpio_set_value(gpio, 1);
}

void led_off(const char *gpio_name)
{
	int node;
	unsigned int gpio;
	node = fdt_path_offset(gd->fdt_blob, gpio_name);
	if (node < 0) {
		printf("Could not find %s node in fdt\n", gpio_name);
		return;
	}
	gpio = fdtdec_get_uint(gd->fdt_blob, node, "gpio", 0);
	if (gpio < 0) {
		printf("Could not find %s node's gpio in fdt\n", gpio_name);
		return;
	}

	gpio_set_value(gpio, 0);
}

bool button_is_press(const char *gpio_name, int value)
{
	int node;
	unsigned int gpio;
	node = fdt_path_offset(gd->fdt_blob, gpio_name);
	if (node < 0) {
		printf("Could not find %s node in fdt\n", gpio_name);
		return false;
	}
	gpio = fdtdec_get_uint(gd->fdt_blob, node, "gpio", 0);
	if (gpio < 0) {
		printf("Could not find %s node's gpio in fdt\n", gpio_name);
		return false;
	}

	if(gpio_get_value(gpio) == value)
	{
		mdelay(10);
		if(gpio_get_value(gpio) == value)
			return true;
		else
			return false;
	}
	else
		return false;
}

void check_button_is_press(void)
{
	int counter = 0;
	char *button_name = NULL;

	// 检测哪个按键被按下
    if (button_is_press("reset_key", GL_RESET_BUTTON_IS_PRESS)) {
        button_name = "RESET";
    }
#ifdef HAS_WPS_KEY
	else if (button_is_press("wps_key", GL_WPS_BUTTON_IS_PRESS)) {
        button_name = "WPS";
    }
#endif
#ifdef HAS_SCREEN_KEY
	else if (button_is_press("screen_key", GL_SCREEN_BUTTON_IS_PRESS)) {
        button_name = "SCREEN";
    }
#endif

	// 如果任一按键被按下
	while (button_name != NULL) {
		// 重新检测按键状态
        int still_pressed = 0;

        if (strcmp(button_name, "RESET") == 0) {
            still_pressed = button_is_press("reset_key", GL_RESET_BUTTON_IS_PRESS);
        }
#ifdef HAS_WPS_KEY
		else if (strcmp(button_name, "WPS") == 0) {
            still_pressed = button_is_press("wps_key", GL_WPS_BUTTON_IS_PRESS);
        }
#endif
#ifdef HAS_SCREEN_KEY
		else if (strcmp(button_name, "SCREEN") == 0) {
            still_pressed = button_is_press("screen_key", GL_SCREEN_BUTTON_IS_PRESS);
        }
#endif

        if (!still_pressed) {
            break;  // 按键已释放
        }

		if (counter == 0)
			printf("%s button is pressed for: %2d ", button_name, counter);

		// LED 闪烁
		led_off("power_led");
		mdelay(350);
		led_on("power_led");
		mdelay(350);

		counter++;

		// how long the button is pressed?
		printf("\b\b\b%2d ", counter);

		if(counter >= 5){
			led_off("power_led");
			led_on("blink_led");
			printf("\n");
			run_command("httpd 192.168.1.1", 0);
			cli_loop();
			break;
		}
	}

	if (counter != 0)
		printf("\n");

	return;
}

// 只检查文件的开头几个特殊 Magic Num
int check_fw_type(void *address) {
	u32 *header_magic1 = (u32 *)(address);
	u32 *header_magic2 = (u32 *)(address + 0x4);

	switch (*header_magic1) {
		case HEADER_MAGIC_CDT:
			return FW_TYPE_CDT;
		case HEADER_MAGIC_ELF:
			if (*((u32 *)(address + 0xc0000)) == HEADER_MAGIC_MBN1 &&
				*((u32 *)(address + 0xc0004)) == HEADER_MAGIC_MBN2
			)
				return FW_TYPE_NOR;
			else
				return FW_TYPE_ELF;
		case HEADER_MAGIC_FIT:
			if (*((u32 *)(address + 0x5C)) == HEADER_MAGIC_JDCLOUD)
				return FW_TYPE_JDCLOUD;
			else if (*((u32 *)(address + 0x600000)) == HEADER_MAGIC_SQUASHFS)
				return FW_TYPE_FACTORY_KERNEL6M;
			else if (*((u32 *)(address + 0xC00000)) == HEADER_MAGIC_SQUASHFS)
				return FW_TYPE_FACTORY_KERNEL12M;
			else
				return FW_TYPE_FIT;
		case HEADER_MAGIC_MBN1:
			if (*header_magic2 == HEADER_MAGIC_MBN2)
				return FW_TYPE_MIBIB;
		case HEADER_MAGIC_UBI:
			return FW_TYPE_UBI;
		default:
			if (*((u16 *)(address + 0x1FE)) == HEADER_MAGIC_EMMC)
				return FW_TYPE_EMMC;
			else
				return FW_TYPE_UNKNOWN;
	}
}

void print_fw_type(int fw_type) {
	printf("* The upload file type: ");
	switch (fw_type) {
		case FW_TYPE_CDT:
			printf("CDT *");
			break;
		case FW_TYPE_ELF:
			printf("ELF *");
			break;
		case FW_TYPE_EMMC:
			printf("EMMC IMAGE *");
			break;
		case FW_TYPE_FACTORY_KERNEL6M:
			printf("FACTORY FIRMWARE (KERNEL SIZE: 6MB) *");
			break;
		case FW_TYPE_FACTORY_KERNEL12M:
			printf("FACTORY FIRMWARE (KERNEL SIZE: 12MB) *");
			break;
		case FW_TYPE_FIT:
			printf("FIT IMAGE *");
			break;
		case FW_TYPE_MIBIB:
			printf("MIBIB *");
			break;
		case FW_TYPE_NOR:
			printf("SPI-NOR IMGAGE *");
			break;
		case FW_TYPE_JDCLOUD:
			printf("JDCLOUD OFFICIAL FIRMWARE *");
			break;
		case FW_TYPE_UBI:
			printf("UBI FIRMWARE *");
			break;
		case FW_TYPE_UNKNOWN:
		default:
			printf("UNKNOWN *");
	}
	return;
}
