//Nils Schröder
//30.09.2018

#include "LCD.h"
#include "Arduino.h"
//#include "WS2812.h"
#include "esp32_digital_led_lib.h"
#include "math.h"
#include "stdlib.h"



strand_t pStrand = { .rmtChannel = 0,.gpioNum = outputPin,.ledType = LED_WS2812B_V3,.brightLimit = 32,.numPixels = LEDCount,
.pixels = nullptr,._stateVars = nullptr };


cRGB HSVToRGB(double H, double S, double V)
{
	double r = 0, g = 0, b = 0;

	if (V == 0)
	{
		r = V;
		g = V;
		b = V;
	}
	else
	{
		int i;
		double f, p, q, t;

		if (H == 360)
			H = 0;
		else
			H = H / 60;

		i = (int)trunc(H);
		f = H - i;

		p = V * (1.0 - S);
		q = V * (1.0 - (S * f));
		t = V * (1.0 - (S * (1.0 - f)));

		switch (i)
		{
		case 0:
			r = V;
			g = t;
			b = p;
			break;

		case 1:
			r = q;
			g = V;
			b = p;
			break;

		case 2:
			r = p;
			g = V;
			b = t;
			break;

		case 3:
			r = p;
			g = q;
			b = V;
			break;

		case 4:
			r = t;
			g = p;
			b = V;
			break;

		default:
			r = V;
			g = p;
			b = q;
			break;
		}

	}

	cRGB rgb;
	rgb.r = r * 255;
	rgb.g = g * 255;
	rgb.b = b * 255;

	return rgb;
}

cHSV RGBToHSV(uint8_t r, uint8_t g, uint8_t b) {
	double delta, min;
	double h = 0, s, v;

	min = min(min(r, g), b);
	v = max(max(r, g), b);
	delta = v - min;

	if (v == 0.0)
		s = 0;
	else
		s = delta / v;

	if (s == 0)
		h = 0.0;

	else
	{
		if (r == v)
			h = (r - b) / delta;
		else if (g == v)
			h = 2 + (b - r) / delta;
		else if (b == v)
			h = 4 + (r - g) / delta;

		h *= 60;

		if (h < 0.0)
			h = h + 360;

	}

	cHSV HSV;
	HSV.H = h;
	HSV.S = s;
	HSV.V = v;

	return HSV;
}

////edit next 3 function for different adresseble LED-libraries////
LEDLCD::LEDLCD(uint8_t x)
{
	setcolorhsv(0, 240, 1, 1);
	setcolorhsv(1, 120, 1, 1);
	setcolorhsv(2, 0, 1, 1);
	setcolorhsv(3, 240, 1, 1);
	setcolorhsv(4, 240, 1, 1);

	for (uint8_t n = 0; n < digits; n++)
	{
		for (uint8_t i = 0; i < 10; i++)
		{
			for (uint8_t j = 0; j < 21; j++)
			{
				lightpoints[n][i][j] = aninumbers[i][j] + (n*points);
			}
		}
	}

}

void LEDLCD::init()
{
	pinMode(outputPin, OUTPUT);
	digitalWrite(outputPin, LOW);

	if (digitalLeds_initStrands(&pStrand, 1)) {
		Serial.println("Init FAILURE: halting");
		while (true) {};
	}

	digitalLeds_resetPixels(&pStrand);
}

//function to call on interrupt or interval
void LEDLCD::update()
{
	if (animation)
	{
		if (animation == 1)
			anidigits(anistep);

		if (animation == 2)
			anidigits2(anistep);

		if (animation == 3)
			colorWheel(timescale, multiplier, anistep);

		if (animation == 4)
			fadeout(timescale, multiplier, anistep);
	}
}

void LEDLCD::writeLEDs()
{
	//1.25ns per Bit -->  1.25*24*22*digits
	//--> ~2,7ms
	if(toupdate)
		digitalLeds_updatePixels(&pStrand), toupdate = 0;
		//LED.sync(),toupdate = 0;
}

void LEDLCD::print(uint16_t LEDnumber, cRGB color)
{
	//LED.set_crgb_at(lightpoints[i][digitvalue[i]][j], color);		//light_WS2812\WS2812.h"
	pStrand.pixels[LEDnumber] = pixelFromRGB(color.r, color.g, color.b);	//esp32_digital_led_lib.h
}

void LEDLCD::setcolorrgb(uint8_t number, uint8_t r, uint8_t g, uint8_t b)
{
	hsv[number] = RGBToHSV(r, g, b);

	color[number].r = r;
	color[number].g = g;
	color[number].b = b;
}
void LEDLCD::setcolorhsv(uint8_t number, double h, double s, double v)
{
	color[number] = HSVToRGB(h, s, v);

	hsv[number].H = h;
	hsv[number].S = s;
	hsv[number].V = v;
}


void LEDLCD::show(float numberin)
{
	clearALL();
	getdigits(numberin);
	printdigits();
	animation = 0;
}
void LEDLCD::printdigits()
{
	for (uint8_t i = 0; i < digits; i++)
	{
		clear(i);
		for (uint8_t j = 0; j < aninumberlenght[digitvalue[i]]; j++)
		{
			print(lightpoints[i][digitvalue[i]][j], color[0]);
		}
	}

	if (decimaldot > 0 && decimaldot != digits)
		print(22 * decimaldot - 1, color[1]);

	toupdate = 1;
}

void LEDLCD::anishow(float numberin, uint16_t timescalein, uint8_t multiplierin)
{
	clearALL();
	update();

	timescale = timescalein;
	multiplier = multiplierin;
	getdigits(numberin);
	animation = 1;
	anistep = 0;

	anidigits(0);
}
void LEDLCD::anidigits(uint32_t step)	//decimal Dot, nextdot = refreshrate*timescale
{
	anistep = step;
	uint8_t a_digit, a_led;

	for (uint8_t i = 0; i < multiplier; i++)
	{
		a_digit = anistep / (22 * timescale);
		a_led = (anistep % (22 * timescale)) / timescale;

		if (a_digit == (digits - 1) && a_led == 21)
		{
			anistep = 0;
			decimaldot = 0;
			animation = 0;
		}
		else
		{
			if (a_led < aninumberlenght[digitvalue[a_digit]])
				print(lightpoints[a_digit][digitvalue[a_digit]][a_led], color[0]);

			if (decimaldot - 1 == a_digit && a_led == 21)
				print(22 * decimaldot - 1, color[1]);

			anistep++;
			toupdate = 1;
		}
	}
}

void LEDLCD::anishowcontinuous(float numberin, uint16_t timescalein, uint8_t multiplierin)
{
	if (numberin != lastnumber)
	{
		animation = 2;
		getdigits(numberin);
		timescale = timescalein;
		multiplier = multiplierin;

		anidigits2(0);

		lastnumber = numberin;
		for (uint8_t i = 0; i < digits; i++)
			lastdigitvalue[i] = digitvalue[i];
	}
}
void LEDLCD::anidigits2(uint32_t step)
{
	anistep = step;
	
	if (anistep == timescale-1)
	{
		anistep = 0;

		for (uint8_t k = 0; k < multiplier; k++)
		{
			uint8_t n = decimaldot + 1;
			if (n > digits)
				n = digits;

			//zerorotation (2nd Digit after Decimalpoint)
			for (uint8_t i = (decimaldot + 1); i < digits; i++)
			{
				//Serial.println(i);
				clear(i);
				animationstep[i] = (animationstep[i] + 1) % (aninumberlenght[0]*3);

				print(lightpoints[i][0][animationstep[i] / 3], color[0]);
			}

			//digitanimation
			for (uint8_t i = 0; i < n; i++)
			{
				uint16_t x;
				x = (decimaldot + 1 - i)*(decimaldot + 1 - i);
				if (lastdigitvalue[i] != digitvalue[i]) animationstep[i] = 0, clear(i);
				else
				{
					if (animationstep[i] < (aninumberlenght[digitvalue[i]] - 1)*(x))
						animationstep[i]++;
				}
				print(lightpoints[i][digitvalue[i]][animationstep[i] / x], color[0]);

			}

			//decimaldot
			if (decimaldot < digits)
				print((22 * decimaldot) - 1, color[1]);

			toupdate = 1;
		}
	}
	anistep++;
}

void LEDLCD::clearALL()
{
	for (uint8_t i = 0; i < digits; i++)
		clear(i), digitvalue[i] = 10;
}

void LEDLCD::clear(uint8_t digit)
{
	value.r = 0;
	value.g = 0;
	value.b = 0;
	for (uint8_t i = 0; i < points; i++)
	{
		uint16_t pos = 22 * digit + i;
		print(pos, value);
	}
	toupdate = 1;
}

void LEDLCD::setZero()
{
	animation = 0;
	lastnumber = 0;
	for (uint8_t i = 0; i < digits; i++)
	{
		digitvalue[i] = 0;
		clear(i);

		for (uint8_t j = 0; j < aninumberlenght[digitvalue[i]]; j++)
			print(lightpoints[i][digitvalue[i]][j], color[0]);
	}
	toupdate = 1;
}

void LEDLCD::getdigits(float numberin)
{
	number = numberin;

	if (0 <= number && number < 10)
	{
		digitvalue[0] = (int)number;
		digitvalue[1] = ((int)(number * 10) % 10);
		digitvalue[2] = ((int)(number * 100) % 10);
		digitvalue[3] = ((int)(number * 1000) % 10);
		decimaldot = 1;
	}

	if (10 <= number && number < 100)
	{
		digitvalue[0] = (int)(number / 10) % 10;
		digitvalue[1] = (int)number % 10;
		digitvalue[2] = ((int)(number * 10) % 10);
		digitvalue[3] = ((int)(number * 100) % 10);
		decimaldot = 2;
	}

	if (100 <= number && number < 1000)
	{
		digitvalue[0] = (int)(number / 100) % 10;
		digitvalue[1] = (int)(number / 10) % 10;
		digitvalue[2] = (int)number % 10;
		digitvalue[3] = ((int)(number * 10) % 10);
		decimaldot = 3;
	}

	if (1000 <= number && number < 10000)
	{
		digitvalue[0] = (int)(number / 1000) % 10;
		digitvalue[1] = (int)(number / 100) % 10;
		digitvalue[2] = (int)(number / 10) % 10;
		digitvalue[3] = (int)number % 10;
		decimaldot = 4;
	}
}

void LEDLCD::setLEDRGB(uint16_t number, uint8_t r, uint8_t g, uint8_t b)
{

}

void LEDLCD::setLEDHSV(uint16_t number, double h, double s, double v)
{

}

void LEDLCD::colorWheel(uint16_t timescalein, uint8_t multiplierin, uint32_t step)
{
	if (step == 0)
	{
		color[2] = color[0];
		hsv[2] = hsv[0];
		anistep = step;
		timescale = timescalein;
		multiplier = multiplierin;
		animation = 3;
	}


	if (anistep%timescale == 0)
	{
		for (uint8_t i = 0; i < multiplier; i++)
			setcolorhsv(0, (uint16_t)(hsv[0].H + 1) % 360, hsv[0].S, hsv[0].V);
		show(number);
		animation = 3;

		for (uint8_t i = 1; i < multiplier; i++)
			anistep++;
	}


	anistep++;
	Serial.println(anistep);

	if (anistep / timescale >= 360)
	{
		color[0] = color[2];
		hsv[0] = hsv[2];
		show(number);
		Serial.println("exit1");
	}
	toupdate = 1;

}

void LEDLCD::fadeout(uint16_t timescalein, uint8_t multiplierin, uint32_t step)
{
	if (step == 0)
	{
		color[2] = color[0];
		hsv[2] = hsv[0];
		color[3] = color[1];
		hsv[3] = hsv[1];
		anistep = step;
		timescale = timescalein;
		multiplier = multiplierin;
		animation = 4;
	}


	anistep++;

	//Serial.println(anistep);

	if (anistep%timescale == 0)
	{
		for (uint8_t i = 0; i < multiplier; i++)
		{
			setcolorhsv(0, hsv[0].H, hsv[0].S, hsv[0].V - ((float)1 / (timescale * 100)));
			setcolorhsv(1, hsv[1].H, hsv[1].S, hsv[1].V - ((float)1 / (timescale * 100)));
		}
		show(number);
		animation = 4;
	}

	if (hsv[0].V <= 0.02)
	{
		color[0] = color[2];
		hsv[0] = hsv[2];
		color[1] = color[3];
		hsv[1] = hsv[3];
		clearALL();
		animation = 0;
		Serial.println("exit2");
	}
	toupdate = 1;
}