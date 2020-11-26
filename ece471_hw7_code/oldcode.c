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

	unsigned char data_out_0[LENGTH] = {0x1, 0x90, 0x0};
	unsigned char data_out_1[LENGTH] = {0x1, 0x0, 0x0};
	unsigned char data_in[LENGTH];
	
	double vin = 0;

	/* Open SPI device */
	
	while (1)
	{

	spi_fd = open("/dev/spidev0.0", O_RDWR);
	if (spi_fd < 0) printf("Error opening file! \n%s", strerror(errno));


	/* Set SPI Mode_0 */

	result = ioctl(spi_fd, SPI_IOC_WR_MODE, &spi_mode);
	if (result < 0) printf("Error using IOCTL! \n%s", strerror(errno));



	/* Loop forever printing the CH0 and CH1 Voltages 	*/


	memset(&spi, 0, sizeof(struct spi_ioc_transfer));


	// Clear out spi structure
	//memset(&spi, 0, sizeof(struct spi_ioc_transfer));

	spi.tx_buf = (unsigned long)&data_out_0;
	spi.rx_buf = (unsigned long)&data_in;
	spi.len = LENGTH;
	spi.delay_usecs = 0;
	spi.speed_hz = 10000;
	spi.bits_per_word = 8;
	spi.cs_change = 0;
	

	// Loop forever
		result = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi);
		//printf("IOCTL Called\n");
		
		printf("Data transfer to MCP3008: %02x%02x%02x\n",data_out_0[0], data_out_0[1], data_out_0[2]);
		// Read in value from device
		

		// Mask upper 14 bits to recieve 10 bit result	
		value = (spi.rx_buf & 0x0003FF);

		// Calculate vin as function of value, and print vin
		vin = (value * 3.3) / 1024;
		printf("%llu --- %u --- %d --- %d --- %lf\n", spi.rx_buf, spi.len, result, value, vin);

		usleep(1000000);

	}
	
	/* Once per second.					*/

	/* Use the SPI_IOC_MESSAGE(1) ioctl() as described in the class notes */

	close(spi_fd);
	return 0;
}
