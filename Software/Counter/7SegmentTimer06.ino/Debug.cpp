#include "Debug.h"
#include "LCD.h"
#include "Arduino.h"



int slow = 1;
int fast = 3;
const int wait = 7;
const int repeats = 1000;

void update(LEDLCD LCD)
{
	//timerAlarmDisable(timer1);
	//timerAlarmDisable(timer2);
	//timerAlarmDisable(timer3);

	LCD.writeLEDs();

	//timerAlarmEnable(timer1);
	//timerAlarmEnable(timer2);
	//timerAlarmEnable(timer3);
}

void anidigitdemo(LEDLCD LCD)
{
	float Zahl;
	for (uint16_t i = 0; i < 10300; i++)
	{

		Zahl = (float)i / 100;
		Serial.println(Zahl);
		LCD.anishowcontinuous(Zahl, slow, fast);
		update(LCD);
		delay(wait);
	}
}

void showdemo(LEDLCD LCD)
{
	LCD.show(1.337);
	for (int i = 0; i < repeats; i++)
		update(LCD), delay(wait);
	LCD.show(13.37);
	for (int i = 0; i < repeats; i++)
		update(LCD), delay(wait);
	LCD.show(133.7);
	for (int i = 0; i < repeats; i++)
		update(LCD), delay(wait);
	LCD.show(1337);
	for (int i = 0; i < repeats; i++)
		update(LCD), delay(wait);
}


void anishowdemo(LEDLCD LCD)
{
	LCD.anishow(1.337, slow, fast);
	for (int i = 0; i < repeats; i++)
		update(LCD), delay(wait);
	LCD.anishow(13.37, slow, fast);
	for (int i = 0; i < repeats; i++)
		update(LCD), delay(wait);
	LCD.anishow(133.7, slow, fast);
	for (int i = 0; i < repeats; i++)
		update(LCD), delay(wait);
	LCD.anishow(1337, slow, fast);
	for (int i = 0; i < repeats; i++)
		update(LCD), delay(wait);
}