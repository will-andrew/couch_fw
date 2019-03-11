#include <xc.h>
#include <stdint.h>
#include <stdlib.h>

#include "pins.h"
#include "softspi.h"
#include "sx1272.h"
#include "system.h"

#include <libpic30.h>

//#define FREQUENCY (915000000.0)
//#define SX_XTAL (32000000.0)
//#define FREQ_CODE (FREQUENCY / SX_XTAL * 524288.0)
//#define FREQ_MSB (FREQ_CODE >> 16)
//#define FREQ_MID ((FREQ_CODE >> 8) & 255)
//#define FREQ_LSB (FREQ_CODE & 255)
#define FREQ_MSB 228
#define FREQ_MID 192
#define FREQ_LSB 0

#define MODE_SLEEP 0x80
#define MODE_STANDBY 0x81
#define MODE_FSTX 0x82
#define MODE_TX 0x83
#define MODE_FSRX 0x84
#define MODE_RXSINGLE 0x86
#define MODE_RXCONT 0x85

int8_t sx1272_power = 13; // dBm

void spi_transact(uint8_t *write, uint8_t write_len, uint8_t *write2, uint8_t write2_len, uint8_t *read, uint8_t read_len)
{
	SPI_CS_LORA = 0;
	
	
	
	SPI_CS_LORA = 1;
}

uint8_t sx1272_read_reg(uint8_t addr)
{
	uint8_t data;
	SPI_CS_LORA = 0;
	softspi_transact_general(&addr, 1, NULL, 0, &data, 1);
	SPI_CS_LORA = 1;
	return data;
}

void sx1272_write_reg(uint8_t addr, uint8_t data)
{
	addr |= 0x80;
	SPI_CS_LORA = 0;
	softspi_transact_general(&addr, 1, &data, 1, NULL, 0);
	SPI_CS_LORA = 1;
}

void sx1272_write_fifo(uint8_t *data, uint8_t len)
{
	uint8_t addr = 0x80; // FIFO indirect reg address with write flag
	SPI_CS_LORA = 0;
	softspi_transact_general(&addr, 1, data, len, NULL, 0);
	SPI_CS_LORA = 1;
}

void sx1272_read_fifo(uint8_t *data, uint8_t len)
{
	uint8_t addr = 0x00; // FIFO indirect reg address without write flag
	SPI_CS_LORA = 0;
	softspi_transact_general(&addr, 1, NULL, 0, data, len);
	SPI_CS_LORA = 1;
}

void sx1272_load_fifo_tx_ptr(void)
{
	uint8_t txBase = sx1272_read_reg(REG_FIFO_TX_BASE);
	sx1272_write_reg(REG_FIFO_PTR, txBase);
}

void sx1272_load_fifo_rx_ptr(void)
{
	uint8_t rxBase = sx1272_read_reg(REG_FIFO_RX_BASE);
	sx1272_write_reg(REG_FIFO_PTR, rxBase);
}

void sx1272_wait_for_irq(uint8_t flag)
{
	while (1) {
		__delay_ms(1);
		uint8_t flags = sx1272_read_reg(REG_IRQ_FLAGS);
		if (flags & flag) {
			sx1272_write_reg(REG_IRQ_FLAGS, flag);
			return;
		}
	}
}

void sx1272_transmit_bytes(uint8_t *data, uint8_t len)
{
	sx1272_write_reg(REG_OP_MODE, MODE_STANDBY);
	__delay_us(60);
	
	sx1272_write_reg(REG_PAYLD_LENGTH, len);
	sx1272_load_fifo_tx_ptr();	
	sx1272_write_fifo(data, len);
	
	LORA_RX = 0;
	LORA_TX = 1;
	
	__delay_us(50);
	sx1272_write_reg(REG_OP_MODE, MODE_TX);
	
	// When packet finishes transmitting it returns to standby mode
	LATBbits.LATB3 = 1;
	sx1272_wait_for_irq(IRQ_TXDONE);
	sx1272_write_reg(REG_OP_MODE, MODE_SLEEP);
	LATBbits.LATB3 = 0;
	
	LORA_TX = 0;
	LORA_RX = 1;
}

void sx1272_setup_receive(void)
{
	sx1272_write_reg(REG_OP_MODE, MODE_RXSINGLE);
}

void sx1272_sleep(void)
{
	sx1272_write_reg(REG_OP_MODE, MODE_SLEEP);
}

void sx1272_set_power(int8_t req_power)
{
	// Not using the boost mode
	if (req_power < -1) {
		req_power = -1;
	} else if (req_power > 13) {
		req_power = 13;
	}
	
	sx1272_power = req_power;
	
	sx1272_write_reg(REG_PA_CONF, sx1272_power + 1);
}

// Returns number of bytes, negative if checksum failed
int sx1272_receive_bytes(uint8_t *data, uint8_t max_len)
{
	uint8_t irqs = sx1272_read_reg(REG_IRQ_FLAGS);
	
	if (!(irqs & IRQ_RXDONE)) {
		sx1272_write_reg(REG_IRQ_FLAGS, 0xFF);
		return 0;
	}
	
	uint8_t nbytes = sx1272_read_reg(REG_RX_NB);
	if (nbytes > max_len) {
		nbytes = max_len;
	}
	
	// Set read address to beginning of last packet 
	sx1272_write_reg(REG_FIFO_PTR, sx1272_read_reg(REG_FIFO_RX_CURR));
	sx1272_read_fifo(data, nbytes);
	
	if (irqs & IRQ_PAYLOADCRCERR) {
		sx1272_write_reg(REG_IRQ_FLAGS, 0xFF);
		return -nbytes;
	} else {
		sx1272_write_reg(REG_IRQ_FLAGS, 0xFF);
		return nbytes;
	}
}

void sx1272_init(void)
{
	sx1272_write_reg(REG_OP_MODE, 0x00); // First go to sleep if necessary to switch modes
	sx1272_write_reg(REG_OP_MODE, MODE_SLEEP);
	sx1272_write_reg(REG_OP_MODE, MODE_STANDBY);
	sx1272_write_reg(REG_FRF_MSB, FREQ_MSB);
	sx1272_write_reg(REG_FRF_MID, FREQ_MID);
	sx1272_write_reg(REG_FRF_LSB, FREQ_LSB);
	sx1272_write_reg(REG_PA_CONF, 0x0F); // 13 dBm output, max without boost
	sx1272_write_reg(REG_PA_RAMP, 0x19);
	sx1272_write_reg(REG_LNA, 0x23);
	//sx1272_write_reg(0x24, 0x1);
	sx1272_write_reg(REG_SYMB_TMT, 0xFF);
	sx1272_write_reg(REG_PREAMB_LEN_MSB, 0x00);
	sx1272_write_reg(REG_PREAMB_LEN_LSB, 0x08);
	sx1272_write_reg(REG_MAX_PAYLD_LENGTH, 0xFF);
	sx1272_write_reg(REG_HOP_PERIOD, 0);
	sx1272_write_reg(REG_PA_DEC, 0x84); // TX power boost disabled
	//sx1272_write_reg(REG_MODEM_CONF, 0xA0);
	//sx1272_write_reg(REG_MODEM_CONF2, 0x94); // SF9
	sx1272_write_reg(REG_MODEM_CONF2, 0x94); // SF7
	sx1272_write_reg(REG_MODEM_CONF, 0x8A); // 0x8A - 500kHz-4/5
	//sx1272_write_reg(REG_MODEM_CONF, 0xA2); // 0xA2 - 500kHz-4/8
	sx1272_write_reg(REG_FIFO_TX_BASE, 0x80);
	sx1272_write_reg(REG_FIFO_RX_BASE, 0x00);
	
	LORA_TX = 0;
	LORA_RX = 1;
}