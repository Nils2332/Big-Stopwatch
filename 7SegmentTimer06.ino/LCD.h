//Nils Schröder
//30.09.2018

#pragma once


#ifndef LCD_H
#define LCD_H

#include "Arduino.h"
#include "esp32_digital_led_lib.h"
#include "stdlib.h"


#define outputPin 32		// LED Output pin

const uint8_t points = 22;
const uint8_t digits = 4;
const uint8_t LEDCount = digits*points;


struct cRGB { uint8_t g; uint8_t r; uint8_t b; };
struct cHSV { double H; double S; double V; };



class LEDLCD
{
	cRGB value;
	cHSV HSVvalue;
	cRGB dataout;
	//TODO from Remove digital_LED

	float lastnumber = 0;
	uint8_t digitvalue[digits] = { 0 };
	uint8_t lastdigitvalue[digits] = { 0 };
	uint16_t animationstep[digits] = { 0 };
	uint8_t toupdate = 0;
	uint8_t animation = 0;
	uint8_t timescale = 1;
	float number;
	uint8_t decimaldot;

	//uint8_t animationsteps = 7;

	const uint8_t aninumbers[10][21] =
	{
		{ 2, 1, 0,17,16,15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 0, 0, 0 },		//0
		{ 3, 4, 5, 6, 7, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },		//1
		{ 0, 1, 2, 3, 4, 5,20,19,18,14,13,12,11,10, 9, 0, 0, 0, 0, 0, 0 },		//2
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,20,19,18, 0, 0, 0, 0, 0, 0 },		//3
		{ 17,16,15,18,19,20, 3, 4, 5, 6, 7, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0 },		//4
		{ 2, 1, 0,17,16,15,18,19,20, 6, 7, 8, 9,10,11, 0, 0, 0, 0, 0, 0 },		//5
		{ 2, 1, 0,17,16,15,14,13,12,11,10, 9, 8, 7, 6,20,19,18, 0, 0, 0 },		//6
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },		//7
		{ 18,19,20, 5, 4, 3, 2, 1, 0,17,16,15,14,13,12,11,10, 9, 8, 7, 6 },		//8
		{ 20,19,18,15,16,17, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11, 0, 0, 0 },		//9
	};

	uint8_t lightpoints[digits][10][21];

	const uint8_t aninumberlenght[10] = { 18, 6,15,15,12,15,18, 9,21,18 };

	const float anitime[10] = { 21 / 18, 21 / 6, 21 / 15, 21 / 15, 21 / 12, 21 / 15, 21 / 18, 21 / 9, 1, 21 / 18 };


public:

	cRGB color[5];
	cHSV hsv[5];

	LEDLCD(uint8_t x);

	void update();

	void setcolorrgb(uint8_t number, uint8_t r, uint8_t g, uint8_t b);
	void setcolorhsv(uint8_t number, double h, double s, double v);
	void setLEDRGB(uint16_t number, uint8_t r, uint8_t g, uint8_t b);
	void setLEDHSV(uint16_t number, double h, double s, double v);

	void show(float numberin);
	void anishow(float numberin, uint8_t timescale);
	void anishowcontinuous(float numberin, uint8_t timescale);

	void clearALL();

	void setZero();

	void colorWheel(uint8_t timescale, uint8_t devider);

	void fadeout(uint8_t timescale, uint8_t devider);

private:

	void printdigits(uint8_t dot);
	void anidigits(uint8_t dot, uint8_t step);
	void anidigits2(uint8_t dot, uint8_t step);

	void clear(uint8_t digit);

	void getdigits(float number);

	void print(uint16_t LEDnumber, cRGB color);

};

#endif // LCD_H