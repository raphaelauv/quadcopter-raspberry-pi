#ifndef MCP3008_h
#define MCP3008_h

#include "../log.h"

#ifdef __arm__
#include <wiringPi.h>
#include <wiringPiSPI.h>
#endif

#define DEFAULT_CHANNEL_ADCNUM 0

typedef struct MCP3008 {
	int clockpin;
	int mosipin;
	int misopin;
	int cspin;
} MCP3008;

#define FLAG_SOFTWARE_MODE 1
#define FLAG_HARDWARE_MODE 2

//SOFTWARE
#define CS_PIN 12
#define CLOCK_PIN 9
#define MOSI_PIN 11
#define MISO_PIN 10

int initMCP3008(MCP3008 ** mcp,int clockpin, int mosipin, int misopin, int cspin);
int softwareReadADC(MCP3008 * mcp, int adcnum);

//HARDWARE
//#define BATTERY_DECALAGE 0.5
#define CONVERTION_TO_VOLT 0.01
#define BATTERY_HIGH_LIMIT 1200
#define BATTERY_LOW_LIMIT 1000



int initHardwareADC(int adcnum);
int hardwareReadADC(int adcnum);

#define EXPO_MOYEN_VAL 125
float getFiltredValue(int adcnum,MCP3008 * mcp);
int testMCP3008(int chanel,int modeFlag);

#endif
