#include "system.h"

#include <xc.h>
//#include <string,h>

#include "TCPIP Stack/TCPIP.h"

#include "message.h"
#include "time.h"
#include "pwm.h"
#include "pins.h"
#include "temperature.h"
#include "encoder.h"
#include "ppm.h"
#include "util.h"

#define RECV_PACKET(recv, pktstruct, buf) do { \
			if ((recv) < sizeof(pktstruct)) { break; } \
			UDPGetArray((buf) + 1, sizeof(pktstruct) - 1); \
		} while (0)

static int send_flag = 0;
static UDP_SOCKET udp_port = INVALID_UDP_SOCKET;

void set_stream_hz(uint8_t hz);

void udpcomm_init(void)
{
	udp_port = UDPOpenEx((DWORD)0, UDP_OPEN_SERVER, CONTROLLER_PORT, 0);
}

static void ping_response(void)
{
	struct pkt_stat_pingresp pkt;

	pkt.header.type = PKT_TYPE_PINGRESP;

	UDPIsPutReady(udp_port);
	UDPPutArray((uint8_t *)&pkt, sizeof(pkt));
	UDPFlush();
}

void udpcomm_task(void)
{
	static uint8_t recvbuf[16] = {0};
	int recv;

	while ((recv = UDPIsGetReady(udp_port)) > 0) {
		uint8_t type = 0;
		UDPGet(&type);

		switch (type) {
			case PKT_TYPE_STREAM: {
				RECV_PACKET(recv, struct pkt_cmd_stream, recvbuf);
				struct pkt_cmd_stream *pkt = (struct pkt_cmd_stream *)recvbuf;
				set_stream_hz(pkt->frequency);
				printf("Begin stream %u Hz\n", pkt->frequency);
				break;
			}
			case PKT_TYPE_PING: {
				RECV_PACKET(recv, struct pkt_cmd_ping, recvbuf);
				ping_response();
				break;
			}
			case PKT_TYPE_WHEEL: {
				RECV_PACKET(recv, struct pkt_cmd_wheel, recvbuf);
				struct pkt_cmd_wheel *pkt = (struct pkt_cmd_wheel *)recvbuf;
				pwm_set_values(pkt->pwms);
				break;
			}
			case PKT_TYPE_TIME: {
				RECV_PACKET(recv, struct pkt_cmd_time, recvbuf);
				struct pkt_cmd_time *pkt = (struct pkt_cmd_time *)recvbuf;
				time_set(&(pkt->time));
				break;
			}
			default: {
				UDPDiscard();
				break;
			}
		}
	}

	if (send_flag) {
		static struct pkt_stat_stream pkt;
		pkt.header.type = PKT_TYPE_STAT;	
		time_get(&pkt.timestamp);

		encoder_readall(pkt.disps);

		pkt.vbat = __builtin_muluu(sum_samples(adc1_buf.vbat), 4265) / 2048; // code = v / 853 * 33 / 33 * 40960, v = 853 / 40960 * codes
		pkt.flags = 0;
		pkt.flags |= (adc1_buf.estop[0] > 500) ? BIT(FLAG_ESTOP) : 0;
		pkt.flags |= (encoder_failed) ? BIT(FLAG_ENCODERFAIL) : 0;
		
		memcpy(pkt.mtemps, mtemps, sizeof(mtemps));
		
		ppm_pause_interrupts();
		for (int i = 0; i < 4; i++) {
			pkt.ctemps[i] = (int16_t)ppm_vals_hld[i][PPM_I_TEMPERATURE] / 5 - 40;
			pkt.currents[i] = __builtin_mulsu((int16_t)ppm_vals_hld[i][PPM_I_CURRENT] - 1024, 3300) / 512; // code = a * 1024 * 50 / 1000 / 3.3, 15.5 codes per amp
			pkt.voltages[i] = __builtin_divud(__builtin_muluu(ppm_vals_hld[i][PPM_I_VOLTAGE], 100), 12); // code = v / 853 * 33 / 3.3 * 1024, 12 codes per volt
			pkt.error_codes[i] = (int16_t)(ppm_vals_hld[i][PPM_I_ERROR] - 100) / 100;
			pkt.drive_fb[i] = (int16_t)ppm_vals_hld[i][PPM_I_DRIVE] - 600;
		}
		ppm_restart_interrupts();

		UDPIsPutReady(udp_port);
		UDPPutArray((uint8_t *)&pkt, sizeof(pkt));
		UDPFlush();

		send_flag = 0;
	}
}

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void)
{
	static int count = 0;
	count++;
	if (count >= 5) {
		count = 0;
		send_flag = 1;
	}

	IFS0bits.T3IF = 0;
}

void set_stream_hz(uint8_t hz)
{
	T3CONbits.TON = 0;
	TMR3 = 0;

	if (hz > 0) {
		PR3 = FCY / 256 / 5 / hz;
		T3CONbits.TON = 1;
	}
}
