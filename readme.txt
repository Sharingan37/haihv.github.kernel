BKAV SPI:

Directory and file description:
/BKAV_SPI/bkav_spi: This directory contains the SPIdev kernel module
       driver files.
/BKAV_SPI/driver_spi: This directory contains the Function kernel module
       driver files.
/BKAV_SPI/include: This directory contain all include files that are needed
       for compiling driver.

/BKAV_SPI/load_driver: This directory load spi driver.
/Test_Driver: This directory is example test spi driver.

Usage:
- Change directory to the spi driver directory.
	cd /BKAV_SPI/bkav_spi/
- Compile the kernel module spi driver.
        make clean
	make
- Change directory to the function driver directory.
	cd /BKAV_SPI/driver_spi/
- Compile the kernel module function driver.
        make clean 
	make
 - Load the kernel module driver using the provided script.
       chmod 777 load_driver.sh
       sudo ./load_driver.sh
- Run the provided test function driver.
        cd /Test_Driver
	make
	./examples
