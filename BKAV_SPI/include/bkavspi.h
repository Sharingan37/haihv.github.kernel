#define SPIMD3 0x0248 // thanh ghi set spi mode for LC898201
#define MODE0 0x00  // mode 0
#define MODE3 0x01  // mode 3

int spi_config(unsigned char index);
void RamRead32A( unsigned long address, unsigned long* outData);
void RamWrite32A( unsigned long address,unsigned long inData);
void RegWriteA(unsigned long address, char inData);
void RegReadA(unsigned long address,unsigned char* outData);
