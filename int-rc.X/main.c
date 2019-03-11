#include "system.h"

#include <xc.h>
#include <libpic30.h>

#include "pins.h"
#include "message.h"
#include "udpcomm.h"
#include "time.h"
#include "temperature.h"
#include "pwm.h"
#include "ppm.h"
#include "encoder.h"

#define THIS_IS_STACK_APPLICATION
#include "TCPIP Stack/TCPIP.h"

void tcpip_config_init(void);

int main(void)
{
	system_init();                                                           
	
	ESTOP_ENGAGE = 1;
	
	time_init();
	ppm_init();
	encoder_init();
	
	ClrWdt();
	__delay_ms(500);
	ClrWdt();
	
	pwm_init();
	
	// Initialize stack-related hardware components that may be
	// required by the UART configuration routines
    TickInit();
	#if defined(STACK_USE_MPFS2)
	MPFSInit();
	#endif

	// Initialize Stack and application related NV variables into AppConfig.
	tcpip_config_init();

	// Initialize core stack layers (MAC, ARP, TCP, UDP) and
	// application modules (HTTP, SNMP, etc.)
    StackInit();

	udpcomm_init(); // Open UDP socket for messages
	
	LED_STAT2 = 1; 
	
	printf("Couch initialisation complete\n");
	printf("RCON=0x%4x\n", RCON);

	uint32_t old_seconds = 0;
    while (1) {
        // This task performs normal stack task including checking
        // for incoming packet, type of packet and calling
        // appropriate stack entity to process it.
        StackTask();

		// Send/receive messages
		udpcomm_task();

        // This tasks invokes each of the core stack application tasks
        StackApplications();

		ClrWdt();
		
		if (time_get_seconds() != old_seconds) {
			old_seconds = time_get_seconds();
			
			read_thermistor_temps();

			LED_STAT1 ^= 1;
		}
	}

	return 1;
}

APP_CONFIG AppConfig; // Config struct for TCP IP stack
void tcpip_config_init(void)
{
	// Start out zeroing all AppConfig bytes to ensure all fields are
	// deterministic for checksum generation
	memset((void*)&AppConfig, 0x00, sizeof(AppConfig));

	AppConfig.Flags.bIsDHCPEnabled = TRUE;
	AppConfig.Flags.bInConfigMode = TRUE;

	AppConfig.MyMACAddr.v[0] = MY_DEFAULT_MAC_BYTE1;
	AppConfig.MyMACAddr.v[1] = MY_DEFAULT_MAC_BYTE2;
	AppConfig.MyMACAddr.v[2] = MY_DEFAULT_MAC_BYTE3;
	AppConfig.MyMACAddr.v[3] = MY_DEFAULT_MAC_BYTE4;
	AppConfig.MyMACAddr.v[4] = MY_DEFAULT_MAC_BYTE5;
	AppConfig.MyMACAddr.v[5] = MY_DEFAULT_MAC_BYTE6;
	AppConfig.MyIPAddr.Val = MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2<<8ul | MY_DEFAULT_IP_ADDR_BYTE3<<16ul | MY_DEFAULT_IP_ADDR_BYTE4<<24ul;
	AppConfig.DefaultIPAddr.Val = AppConfig.MyIPAddr.Val;
	AppConfig.MyMask.Val = MY_DEFAULT_MASK_BYTE1 | MY_DEFAULT_MASK_BYTE2<<8ul | MY_DEFAULT_MASK_BYTE3<<16ul | MY_DEFAULT_MASK_BYTE4<<24ul;
	AppConfig.DefaultMask.Val = AppConfig.MyMask.Val;
	AppConfig.MyGateway.Val = MY_DEFAULT_GATE_BYTE1 | MY_DEFAULT_GATE_BYTE2<<8ul | MY_DEFAULT_GATE_BYTE3<<16ul | MY_DEFAULT_GATE_BYTE4<<24ul;
	AppConfig.PrimaryDNSServer.Val = MY_DEFAULT_PRIMARY_DNS_BYTE1 | MY_DEFAULT_PRIMARY_DNS_BYTE2<<8ul  | MY_DEFAULT_PRIMARY_DNS_BYTE3<<16ul  | MY_DEFAULT_PRIMARY_DNS_BYTE4<<24ul;
	AppConfig.SecondaryDNSServer.Val = MY_DEFAULT_SECONDARY_DNS_BYTE1 | MY_DEFAULT_SECONDARY_DNS_BYTE2<<8ul  | MY_DEFAULT_SECONDARY_DNS_BYTE3<<16ul  | MY_DEFAULT_SECONDARY_DNS_BYTE4<<24ul;
}

