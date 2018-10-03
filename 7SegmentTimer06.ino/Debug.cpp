#include "Debug.h"



int slow = 1;
int fast = 3;
const int wait = 7;
const int repeats = 1000;

void anidigitdemo()
{
	float Zahl;
	for (uint16_t i = 0; i < 10300; i++)
	{

		Zahl = (float)i / 100;
		Serial.println(Zahl);
		LCD1.anishowcontinuous(Zahl, slow, fast);
		update();
		delay(wait);
	}
}

void showdemo()
{
	LCD1.show(1.337);
	for (int i = 0; i < repeats; i++)
		update(), delay(wait);
	LCD1.show(13.37);
	for (int i = 0; i < repeats; i++)
		update(), delay(wait);
	LCD1.show(133.7);
	for (int i = 0; i < repeats; i++)
		update(), delay(wait);
	LCD1.show(1337);
	for (int i = 0; i < repeats; i++)
		update(), delay(wait);
}


void anishowdemo()
{
	LCD1.anishow(1.337, slow, fast);
	for (int i = 0; i < repeats; i++)
		update(), delay(wait);
	LCD1.anishow(13.37, slow, fast);
	for (int i = 0; i < repeats; i++)
		update(), delay(wait);
	LCD1.anishow(133.7, slow, fast);
	for (int i = 0; i < repeats; i++)
		update(), delay(wait);
	LCD1.anishow(1337, slow, fast);
	for (int i = 0; i < repeats; i++)
		update(), delay(wait);
}