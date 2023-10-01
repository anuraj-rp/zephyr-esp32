#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>



#define SPI_OP SPI_OP_MODE_MASTER | SPI_MODE_CPOL | SPI_MODE_CPHA | SPI_WORD_SET(8) | SPI_LINES_SINGLE

/* Don't use DT for direct SPI control. Use it for device on SPI 
*  https://github.com/zephyrproject-rtos/zephyr/discussions/44490	
*/
//static const struct spi_dt_spec spi_spec_dev = SPI_DT_SPEC_GET(SPI_DEV, SPI_OP, 0);

const struct device *spi_dev_p = DEVICE_DT_GET(DT_NODELABEL(spi2));


/* Below is not idiomatic Zephyr. Probably move to device tree */
struct spi_config spi_cfg = {
	.frequency = 100000,
	.operation = SPI_OP,
	.slave = 0,
	.cs = {
		.delay = 0,
		.gpio = {
			.dt_flags = GPIO_ACTIVE_LOW,
			.pin = 10,
			.port = 0
		}
	}
};

/* Slightly better way to config spi below */
// const struct spi_config spi_cfg = {
//     .frequency = DT_PROP(DT_NODELABEL(spi2), clock_frequency),
//     .operation = SPI_OP,
//     .cs = SPI_CS_CONTROL_INIT(DT_NODELABEL(spi2), 10),
// };

uint32_t msg = 0xAA55AA55;

struct spi_buf spi_buffer_tx = {.buf = &msg, .len = 4};

struct spi_buf_set tx_buffer = {.buffers = &spi_buffer_tx, .count = 1};

//struct spi_dt_spec spi_com = SPI_DT_SPEC_GET(DT_NODELABEL(spi2), SPI_OP, 0);

int main(void)
{
	printf("Hello World! %s\n", CONFIG_BOARD);
	//if(!spi_is_ready(&spi_spec_dev))
	if(!device_is_ready(spi_dev_p))
	{
		printf("SPI init failed\n");
		return;
	}

	uint8_t ret;
	while(1)
	{
		ret = spi_write(spi_dev_p, &spi_cfg, &tx_buffer);
		//ret = spi_write_dt(&spi_com, &tx_buffer);
		if(ret)
		{
			printf("spi write failed");
			break;
		}
	}
    return 0;
}