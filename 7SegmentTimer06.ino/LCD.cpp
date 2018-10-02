//Nils Schröder
//30.09.2018

#include "LCD.h"
#include "Arduino.h"
//#include "esp32_digital_led_lib\esp32_digital_led_lib.h"
#include "esp32_digital_led_lib.h"
//#include "nRF905.h"
#include "math.h"
#include "stdlib.h"


cRGB value;
cHSV HSVvalue;
cRGB dataout;


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

	pinMode(outputPin, OUTPUT);
	digitalWrite(outputPin, LOW);

	if (digitalLeds_initStrands(&pStrand, 1)) {
		Serial.println("Init FAILURE: halting");
		while (true) {};
	}

	digitalLeds_resetPixels(&pStrand);

}


void LEDLCD::update()
{
	if(toupdate)
		digitalLeds_updatePixels(&pStrand);
	toupdate = 0;
}


void LEDLCD::show(float numberin)
{
	getdigits(numberin);

	if (0 <= numberin && numberin < 10)
	{
		printdigits(1);
	}

	if (10 <= numberin && numberin< 100)
	{
		if (lastnumber < 10)
		{
			clearALL();
		}
		printdigits(2);
	}

	if (100 <= numberin && numberin< 1000)
	{
		if (lastnumber < 100)
		{
			clearALL();
		}
		printdigits(3);
	}

	if (1000 <= numberin && numberin < 10000)
	{
		if (lastnumber < 1000)
		{
			clearALL();
		}
		printdigits(0);
	}

	toupdate = 1;
}

void LEDLCD::anishow(float numberin)
{
	clearALL();
	update();
	getdigits(numberin);

	if (0 <= numberin && numberin < 10)
	{
		anidigits(1);
	}

	if (10 <= numberin && numberin< 100)
	{
		anidigits(2);
	}

	if (100 <= numberin && numberin< 1000)
	{
		anidigits(3);
	}

	if (1000 <= numberin && numberin < 10000)
	{
		anidigits(0);
	}
}

void LEDLCD::printdigits(uint8_t dot)
{
	for (uint8_t i = 0; i < digits; i++)
	{
		//if (digitvalue[i] != lastdigitvalue[i])
		//{
		clear(i);
		for (uint8_t j = 0; j < aninumberlenght[digitvalue[i]]; j++)
		{
			//LED.set_crgb_at(lightpoints[i][digitvalue[i]][j], color[0]);
			pStrand.pixels[lightpoints[i][digitvalue[i]][j]] = pixelFromRGB(color[0].r, color[0].g, color[0].b);
		}
		//}
	}

	if (dot > 0)
	{
		//LED.set_crgb_at(22 * dot - 1, color[1]);
		pStrand.pixels[22 * dot - 1] = pixelFromRGB(color[1].r, color[1].g, color[1].b);
	}
}

void LEDLCD::anidigits(uint8_t dot)
{
	for (uint8_t i = 0; i < digits; i++)
	{
		//write dot to LED
		if (dot == i && i != 0)
		{
			//LED.set_crgb_at(22 * dot - 1, color[1]);
			pStrand.pixels[22 * dot - 1] = pixelFromRGB(color[1].r, color[1].g, color[1].b);
		}
		//write digits to LED
		for (uint8_t j = 0; j < aninumberlenght[digitvalue[i]]; j++)
		{
			//LED.set_crgb_at(lightpoints[i][digitvalue[i]][j], color[0]);
			pStrand.pixels[lightpoints[i][digitvalue[i]][j]] = pixelFromRGB(color[0].r, color[0].g, color[0].b);

			//LED.sync();
			digitalLeds_updatePixels(&pStrand);
			delay((int)(anitime[digitvalue[i]] * 10));
		}

	}
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

void LEDLCD::anishowcontinuous(float numberin, uint8_t step)
{
	if (numberin != lastnumber)
	{
		getdigits(numberin);

		if (0 <= numberin && numberin < 10)
		{
			anidigits2(1);
		}

		if (10 <= numberin && numberin < 100)
		{
			anidigits2(2);
		}

		if (100 <= numberin && numberin < 1000)
		{
			anidigits2(3);
		}

		if (1000 <= numberin && numberin < 10000)
		{
			anidigits2(4);
		}
	}
	lastnumber = numberin;
	for (uint8_t i = 0; i < digits; i++)
		lastdigitvalue[i] = digitvalue[i];
}

void LEDLCD::anidigits2(uint8_t dot)
{
	uint8_t n = dot + 1;
	if (n > digits)
		n = digits;

	for (uint8_t i = (dot + 1); i < digits; i++)
	{
		clear(i);
		animationstep[i] = (animationstep[i] + 1) % 36;
		//LED.set_crgb_at(lightpoints[i][0][animationstep[i] / 2], color[0]);
		pStrand.pixels[lightpoints[i][0][animationstep[i] / 2]] = pixelFromRGB(color[0].r, color[0].g, color[0].b);
	}

	for (uint8_t i = 0; i < n; i++)
	{
		if (lastdigitvalue[i] != digitvalue[i]) animationstep[i] = 0, clear(i);
		else
		{
			if (animationstep[i] < (aninumberlenght[digitvalue[i]] - 1)*(dot + 1 - i))
				animationstep[i]++;
		}
		//LED.set_crgb_at(lightpoints[i][digitvalue[i]][animationstep[i] / (dot + 1 - i)], color[0]);
		pStrand.pixels[lightpoints[i][digitvalue[i]][animationstep[i] / (dot + 1 - i)]] = pixelFromRGB(color[0].r, color[0].g, color[0].b);
	}

	if (dot < digits)
		//LED.set_crgb_at((22 * dot) - 1, color[1]);
		pStrand.pixels[(22 * dot) - 1] = pixelFromRGB(color[0].r, color[0].g, color[0].b);
}

void LEDLCD::clearALL()
{
	for (uint8_t i = 0; i < digits; i++)
		clear(i), digitvalue[i] = 10;
}

void LEDLCD::setZero()
{
	lastnumber = 0;
	for (uint8_t i = 0; i < digits; i++)
	{
		digitvalue[i] = 0;
		clear(i);
		value.r = 0;
		value.g = 0;
		value.b = 255;

		for (uint8_t j = 0; j < aninumberlenght[digitvalue[i]]; j++)
		{
			//LED.set_crgb_at(lightpoints[i][digitvalue[i]][j], value);
			pStrand.pixels[lightpoints[i][digitvalue[i]][j]] = pixelFromRGB(value.r, value.g, value.b);
		}

	}
	digitalLeds_updatePixels(&pStrand);
}

void LEDLCD::clear(uint8_t digit)
{
	value.r = 0;
	value.g = 0;
	value.b = 0;
	for (uint8_t i = 0; i < points; i++)
	{
		uint16_t pos = 22 * digit + i;
		//LED.set_crgb_at(pos, value);
		pStrand.pixels[pos] = pixelFromRGB(value.r, value.g, value.b);
	}
}

void LEDLCD::getdigits(float number)
{
	if (0 <= number && number < 10)
	{
		digitvalue[0] = (int)number;
		digitvalue[1] = ((int)(number * 10) % 10);
		digitvalue[2] = ((int)(number * 100) % 10);
		digitvalue[3] = ((int)(number * 1000) % 10);
	}

	if (10 <= number && number < 100)
	{
		digitvalue[0] = (int)(number / 10) % 10;
		digitvalue[1] = (int)number % 10;
		digitvalue[2] = ((int)(number * 10) % 10);
		digitvalue[3] = ((int)(number * 100) % 10);
	}

	if (100 <= number && number < 1000)
	{
		digitvalue[0] = (int)(number / 100) % 10;
		digitvalue[1] = (int)(number / 10) % 10;
		digitvalue[2] = (int)number % 10;
		digitvalue[3] = ((int)(number * 10) % 10);
	}

	if (1000 <= number && number < 10000)
	{
		digitvalue[0] = (int)(number / 1000) % 10;
		digitvalue[1] = (int)(number / 100) % 10;
		digitvalue[2] = (int)(number / 10) % 10;
		digitvalue[3] = (int)number % 10;
	}
}

void LEDLCD::setPinRGB(uint16_t pin, uint8_t r, uint8_t g, uint8_t b)
{

}

void LEDLCD::setPinHSV(uint16_t pin, double h, double s, double v)
{

}

