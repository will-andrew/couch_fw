/* 
 * File:   sx1272.h
 * Author: will
 *
 * Created on 22 May 2017, 10:15 AM
 */

#ifndef SX1272_H
#define	SX1272_H

#include <stdint.h>

#define REG_OP_MODE 0x01
#define REG_FRF_MSB 0x06
#define REG_FRF_MID 0x07
#define REG_FRF_LSB 0x08
#define REG_PA_CONF 0x09
#define REG_PA_RAMP 0x0A
#define REG_LNA 0x0C
#define REG_FIFO_PTR 0x0D
#define REG_FIFO_TX_BASE 0x0E
#define REG_FIFO_RX_BASE 0x0F
#define REG_FIFO_RX_CURR 0x10
#define REG_IRQ_MASK 0x11
#define REG_IRQ_FLAGS 0x12
#define REG_RX_NB 0x13
#define REG_RX_HEAD_CNT_MSB 0x14
#define REG_RX_HEAD_CNT_LSB 0x15
#define REG_RX_PKT_CNT_MSB 0x16
#define REG_RX_PKT_CNT_LSB 0x17
#define REG_MODEM_STAT 0x18
#define REG_PKT_SNR 0x19
#define REG_PKT_RSSI 0x1A
#define REG_RSSI 0x1B
#define REG_HOP_CH 0x1C
#define REG_MODEM_CONF 0x1D
#define REG_MODEM_CONF2 0x1E
#define REG_SYMB_TMT 0x1F
#define REG_PREAMB_LEN_MSB 0x20
#define REG_PREAMB_LEN_LSB 0x21
#define REG_PAYLD_LENGTH 0x22
#define REG_MAX_PAYLD_LENGTH 0x23
#define REG_HOP_PERIOD 0x24
#define REG_PA_DEC 0x5A
#define REG_DIO_MAPPING1 0x40
#define REG_DIO_MAPPING2 0x41

#define MODE_SLEEP 0x80
#define MODE_STANDBY 0x81
#define MODE_FSTX 0x82
#define MODE_TX 0x83
#define MODE_FSRX 0x84
#define MODE_RXSINGLE 0x86
#define MODE_RXCONT 0x85

#define IRQ_RXTIMEOUT 0x80
#define IRQ_RXDONE 0x40
#define IRQ_PAYLOADCRCERR 0x20
#define IRQ_VALIDHEADER 0x10
#define IRQ_TXDONE 0x08
#define IRQ_CADDONE 0x04
#define IRQ_FHSSCHANGECHANNEL 0x02
#define IRQ_CADDETECTED 0x01

extern int8_t sx1272_power;

void spi_transact(uint8_t *write, uint8_t write_len, uint8_t *write2, uint8_t write2_len, uint8_t *read, uint8_t read_len);
uint8_t sx1272_read_reg(uint8_t addr);
void sx1272_write_reg(uint8_t addr, uint8_t data);
void sx1272_write_fifo(uint8_t *data, uint8_t len);
void sx1272_read_fifo(uint8_t *data, uint8_t len);
void sx1272_transmit_bytes(uint8_t *data, uint8_t len);
int sx1272_receive_bytes(uint8_t *data, uint8_t maxlen); // Returns number of bytes
void sx1272_init(void);
void sx1272_setup_receive(void);
void sx1272_set_power(int8_t power);
void sx1272_sleep(void);

#endif	/* SX1272_H */

