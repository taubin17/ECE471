#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define LENGTH 3


int main(int argc, char **argv) {

        int spi_fd;
	int spi_mode = SPI_MODE_0;
	int result = 0;
	int value = 0;
	int lsb_mode = 0;

	struct spi_ioc_transfer spi;

	//int testing = 0x00;
	//int increment = 0x10;

	unsigned char data_out_0[LENGTH];
	unsigned char data_in[LENGTH];
	
	double vin = 0;

	/* Open SPI device */
	
	spi_fd = open("/dev/spidev0.0", O_RDWR);
	if (spi_fd < 0) printf("Error opening file! \n%s", strerror(errno));


	/* Set SPI Mode_0 */

	result = ioctl(spi_fd, SPI_IOC_WR_MODE, &spi_mode);
	if (result < 0) printf("Error using IOCTL! \n%s", strerror(errno));

	// Set the bit mode to least signifcant bit first
	result = ioctl(spi_fd, SPI_IOC_WR_LSB_FIRST, &lsb_mode);
	if (result < 0) printf("Error using IOCTL! \n%s", strerror(errno));

	/* Loop forever printing the CH0 and CH1 Voltages 	*/

	// Clear output buffer
	memset(&spi, 0, sizeof(struct spi_ioc_transfer));
	
	spi.tx_buf = (unsigned long)&data_out_0;
	spi.rx_buf = (unsigned long)&data_in;
	spi.len = LENGTH;
	spi.delay_usecs = 0;
	spi.speed_hz = 10000;
	spi.bits_per_word = 8;
	spi.cs_change = 0;
	
	while (1)
	{
		// Send data telling MCP3008 we want channel 0
		data_out_0[0] = 0x01;
		data_out_0[1] = 0x80;
		data_out_0[2] = 0x00;
		
		// Send our message
		result = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi);	
		if (result < 0) printf("Error using IOCTL! \n%s", strerror(errno));
		
		// Mask upper 14 bits to recieve 10 bit result	
		value = (data_in[2] + ((data_in[1] & 0x03) << 8));

		// Calculate vin as function of value, and print vin
		vin = (value * 3.3) / 1024;
		printf("Channel zero = %lf\n", vin);

		
		// Send data telling MCP3008 we want channel 1
		data_out_0[0] = 0x01;
		data_out_0[1] = 0x90;
		data_out_0[2] = 0x00;

		// Send our message
		result = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi);
		if (result < 0) printf("Error using IOCTL! \n%s", strerror(errno));
		
		// Mask upper 14 bits to recieve 10 bit result
		value = (data_in[2] + ((data_in[1] & 0x03) << 8));

		// Calculate vin as function of value, and print vin
		vin = (value * 3.3) / 1024;
		printf("Channel one = %lf\n", vin);
	
		// Sleep 1 second
		usleep(1000000);

	}
	
	// Cleanup descriptors
	close(spi_fd);

	return 0;
}
