//#include "Arduino.h"
#include "MCP3008.h"
#include "../concurrent.h"

int initMCP3008(MCP3008 ** mcp, int clockpin, int mosipin, int misopin,
		int cspin) {

	#ifdef __arm__
	if (wiringPiSetup () == -1){
		return -1;
	}
	#endif

	// define SPI outputs and inputs for bitbanging

	*mcp = (MCP3008 *) malloc(sizeof(MCP3008));
	if (*mcp == NULL) {
		logString("MALLOC FAIL : mcp");
		return EXIT_FAILURE;
	}
	(*mcp)->clockpin = clockpin;
	(*mcp)->mosipin = mosipin;
	(*mcp)->misopin = misopin;
	(*mcp)->cspin = cspin;

	#ifdef __arm__
	pinMode(cspin, OUTPUT);
	pinMode(clockpin, OUTPUT);
	pinMode(mosipin, OUTPUT);
	pinMode(misopin, INPUT);
	#endif



	return 0;
}

// read SPI data from MCP3008 chip, 8 possible adc's (0 thru 7)
int softwareReadADC(MCP3008 * mcp, int adcnum) {

	if ((adcnum > 7) || (adcnum < 0))
		return -1; // Wrong adc address return -1


	#ifdef __arm__
	digitalWrite(mcp->cspin, HIGH);
	digitalWrite(mcp->clockpin, LOW); //  # start clock low
	digitalWrite(mcp->cspin, LOW); //     # bring CS low
	#endif

	int commandout = adcnum;
	commandout |= 0x18; //  # start bit + single-ended bit
	commandout <<= 3; //    # we only need to send 5 bits here

	for (int i = 0; i < 5; i++) {
		if (commandout & 0x80){
			#ifdef __arm__
			digitalWrite(mcp->mosipin, HIGH);
			#endif
		}
		else{
			#ifdef __arm__
			digitalWrite(mcp->mosipin, LOW);
			#endif

		}
		commandout <<= 1;

		#ifdef __arm__
		digitalWrite(mcp->clockpin, HIGH);
		digitalWrite(mcp->clockpin, LOW);
		#endif


	}

	int adcout = 0;
	// read in one empty bit, one null bit and 10 ADC bits
	for (int i = 0; i < 12; i++) {

		#ifdef __arm__
		digitalWrite(mcp->clockpin, HIGH);
		digitalWrite(mcp->clockpin, LOW);
		#endif


		adcout <<= 1;

		#ifdef __arm__
		if (digitalRead(mcp->misopin)){
			adcout |= 0x1;
		}
		#endif
	}
	#ifdef __arm__
	digitalWrite(mcp->cspin, HIGH);
	#endif
	adcout >>= 1; //      # first bit is 'null' so drop it
	return adcout;
}

int testMCP3008(int chanel){

	MCP3008 * mcp;
	if(initMCP3008(&mcp,CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN)){
		return -1;
	}

	int val;
	while (1) {
		val = softwareReadADC(chanel); // read Chanel 0 from MCP3008 ADC
		printf("CHANNEL %d | VAL : %d\n",chanel, val);
	}
}
