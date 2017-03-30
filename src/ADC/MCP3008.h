#ifndef MCP3008_h
#define MCP3008_h
#include "../concurrent.h"

#ifdef __arm__
#include <wiringPi.h>
#include <wiringPiSPI.h>
#endif

#define CS_PIN 12
#define CLOCK_PIN 9
#define MOSI_PIN 11
#define MISO_PIN 10

typedef struct MCP3008 {
	int clockpin;
	int mosipin;
	int misopin;
	int cspin;
} MCP3008;


int initMCP3008(MCP3008 ** mcp,int clockpin, int mosipin, int misopin, int cspin);
int softwareReadADC(MCP3008 * mcp, int adcnum);
int testMCP3008(int chanel);

#endif
