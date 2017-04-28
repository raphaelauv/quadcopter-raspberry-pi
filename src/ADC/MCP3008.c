#include "MCP3008.h"

unsigned char send[3]={0x18,0x00,0x00};

int initHardwareADC(int adcnum){

	#ifdef __arm__
	if(wiringPiSPISetup (adcnum, 300000)==-1){
		logString("initHardwareADC - wiringPiSPIDataRW FAIL\n");
        return -1;
    }
	#endif

	#ifdef __arm__
	if(!wiringPiSPIDataRW (adcnum, send, 3)){
		logString("initHardwareADC - wiringPiSPIDataRW FAIL\n");
		return -1;
	}
	#endif

	send[0]=0x01;
	send[1]=0x80;
	return 0;

}

int hardwareReadADC(int adcnum){

	int value=0;
//	printf("VAL TAB : %x , %x , %x\n",send[0],send[1],send[2]);
	#ifdef __arm__
	if(!wiringPiSPIDataRW (adcnum, send, 3)){
		logString("hardwareReadADC - wiringPiSPIDataRW FAIL\n");
		return -1;
	}
	#endif
	value=((send[1] & 0x03) <<8) | send[2];
//	printf("VALUE : %d\n",value);
	send[0]=0x01;
	send[1]=0x80;
	return value;
}


float getFiltredValue(int adcnum,MCP3008 * mcp){


    int voltage=0;
    float batterie_charge=0;

    int j=0;

    int hardwareMode=0;
    if(mcp==NULL){
    	hardwareMode=1;
    }
    while(1){

    	if(hardwareMode){
    		voltage=hardwareReadADC(adcnum);
    	}else{
    		voltage=softwareReadADC(mcp,adcnum);
    	}

        batterie_charge=batterie_charge* 0.92 + (voltage+65)* 0.09853;

        if(j==EXPO_MOYEN_VAL){
            break;
        }
        j++;
    }

    return (batterie_charge * CENVERTION_TO_VOLT) - BATTERY_DECALAGE;

}

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
		//logString("MALLOC FAIL : mcp");
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

	if(mcp == NULL){
		return -1;
	}

	if ((adcnum > 7) || (adcnum < 0)){
		return -1; // Wrong adc address return -1
	}

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

int testMCP3008(int chanel,int modeFlag){

	int val;
	MCP3008 * mcp;
	int hardwareMode=0;

	if( modeFlag == FLAG_HARDWARE_MODE){
		initHardwareADC(chanel);
		hardwareMode=1;
		printf("FLAG_HARDWARE_MODE\n");
	}else if(modeFlag == FLAG_SOFTWARE_MODE){
		printf("FLAG_SOFTWARE_MODE\n");
		if (initMCP3008(&mcp, CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN)) {
			return -1;
		}
	}

	while (1) {
        usleep(4000);
		if(hardwareMode){
			val=getFiltredValue(chanel,NULL);
		}else{
			val=getFiltredValue(chanel,mcp);
		}
		printf("CHANNEL %d | VAL : %d\n", chanel, val);
	}

}
