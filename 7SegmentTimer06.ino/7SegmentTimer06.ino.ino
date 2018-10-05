//Nils Schröder
//28.09.2018

#include <math.h>
#include <stdlib.h>
#include "nRF905\src\nRF905.h"
#include "LCD.h"
#include "modes.h"
#include "Debug.h"


#define T6 33
#define T5 25
#define T4 26
#define T3 27
#define T2 34
#define T1 35


#define RXADDR	0xABBAFFE0
#define TX1ADDR	0xABBAFFE1
#define TX2ADDR	0xABBAFFE2


#define Batpin1	4
#define Batpin2	2


uint8_t button[6] = { 0 };
uint8_t buttons = 0;
uint8_t button_used = 0;


double V_Bat1, V_Bat2;
double V_Button[2];

uint8_t V_Batlow = 0;
uint32_t oldtime_V = 0;

uint8_t remotereset = 0;
uint32_t remotedelay[2];
uint32_t remotecounter = 0;


LEDLCD LCD1(0);

float stattime;
float oldtime;
float record = 0;
uint8_t recordset = 0;

uint8_t remotebutton = 0;

uint8_t v_buttonshow = 0;

hw_timer_t * timer1 = NULL;
hw_timer_t * timer2 = NULL;
hw_timer_t * timer3 = NULL;

uint32_t oldtime_l = 0;
uint32_t sleepcounter = 0;
uint8_t sleeping = 0;

//timer1
void IRAM_ATTR getButtons()
{
	button[0] = !digitalRead(T1);
	button[1] = !digitalRead(T2);
	button[2] = !digitalRead(T3);
	button[3] = !digitalRead(T4);
	button[4] = !digitalRead(T5);
	button[5] = !digitalRead(T6);

	buttons = button[5] * 32 + button[4] * 16 + button[3] * 8 + button[2] * 4 + button[1] * 2 + button[0];

	button_used = 0;
	//Serial.println(buttons);
}

//timer2
void IRAM_ATTR getRadio()
{

	remotecounter++;

	if (remotecounter > 300)
		remotereset = 0;
	//timerAlarmDisable(timer1);
	//timerAlarmDisable(timer2);
	//timerAlarmDisable(timer3);

	if (digitalRead(17))
	{
		Serial.println("Get Wireless");
		uint8_t buffer[NRF905_MAX_PAYLOAD];
		nRF905_read(buffer, sizeof(buffer));

		uint8_t id;
		uint8_t command;
		uint16_t timer;

		if (buffer[0] == 130)
		{
			id = buffer[1];
			command = buffer[2];

			//Voltage
			if (command == 1)
			{
				Serial.print("Voltage: ");
				V_Button[id - 1] = ((uint16_t)(buffer[3] << 8) | (buffer[4]));
				V_Button[id - 1] = V_Button[id - 1] / 1000;

				v_buttonshow = id;

				Serial.println(V_Button[id - 1]);
			}
			//Button Pressed
			if (command == 2)
			{
				timer = ((uint16_t)(buffer[3] << 8) | (buffer[4]));
				Serial.print("Delay: ");
				Serial.println(timer);
				remotedelay[id - 1] = timer;
				remotebutton = 1 << (id - 1);
				Serial.println(buttons);
				button_used = 0;

				if (id == 1)
				{
					remotecounter = 0;
					remotereset ++;
					Serial.println(remotereset);
				}
			}

			//ACK Packet
			//uint8_t compare[NRF905_MAX_PAYLOAD] = { 65, 67, 75, 0 };
			uint8_t compare[NRF905_MAX_PAYLOAD] = { 0 };
			compare[0] = 65;
			compare[1] = 67;
			compare[2] = 75;
			
			//for (uint8_t i = 0; i < NRF905_MAX_PAYLOAD; i++)
			//	Serial.print(compare[i]);

			//Serial.println(" ");

			if (id == 1)
			{
				Serial.print("Sending ACK:");
				Serial.println(id);
				while (!nRF905_TX(TX1ADDR, compare, sizeof(buffer), NRF905_NEXTMODE_RX));
			}

			if (id == 2)
			{
				Serial.print("Sending ACK:");
				Serial.println(id);
				while (!nRF905_TX(TX2ADDR, compare, sizeof(buffer), NRF905_NEXTMODE_RX));
			}
		}
	}

	//timerAlarmEnable(timer1);
	//timerAlarmEnable(timer2);
	//timerAlarmEnable(timer3);
}

//timer3
void IRAM_ATTR updateDisplay()
{
	//timerAlarmDisable(timer1);
	//timerAlarmDisable(timer2);
	//timerAlarmDisable(timer3);

	

	//timerAlarmEnable(timer1);
	//timerAlarmEnable(timer2);
	//timerAlarmEnable(timer3);
}

void setup()
{
	analogSetAttenuation(ADC_6db);

	//init Serial
	Serial.begin(115200);

	//configure Buttons
	pinMode(T1, INPUT);
	pinMode(T2, INPUT);
	pinMode(T3, INPUT);
	pinMode(T4, INPUT);
	pinMode(T5, INPUT);
	pinMode(T6, INPUT);

	digitalWrite(T1, HIGH);
	digitalWrite(T2, HIGH);
	digitalWrite(T3, HIGH);
	digitalWrite(T4, HIGH);
	digitalWrite(T5, HIGH);
	digitalWrite(T6, HIGH);
	//end Buttons

	//radio Avaiable PIN
	pinMode(17, INPUT);

	// Start up radio
	nRF905_init();

	// Set address of this device
	nRF905_setListenAddress(RXADDR);

	// Put into receive mode
	nRF905_RX();
	nRF905_powerUp();
	nRF905_RX();


	LCD1.init();

	//Timer init
	//esp_task_wdt_fee

	timer1 = timerBegin(1, 80, true);
	timerAttachInterrupt(timer1, &getButtons, true);
	timerAlarmWrite(timer1, 5000, true);
	timerAlarmEnable(timer1);

	timer2 = timerBegin(2, 80, true);
	timerAttachInterrupt(timer2, &getRadio, true);
	timerAlarmWrite(timer2, 5000, true);
	timerAlarmEnable(timer2);

	timer3 = timerBegin(0, 80, true);
	timerAttachInterrupt(timer3, &updateDisplay, true);
	timerAlarmWrite(timer3, 10000, true);
	timerAlarmEnable(timer3);

	//End timer


	Serial.println("7-Segment-Counter-Start");

	LCD1.setcolorhsv(0, 240, 1, 0.8);
	LCD1.setcolorhsv(1, 120, 1, 1);

	LCD1.clearALL();

	uint8_t samples = 10;
	V_Bat1 = 0, V_Bat2 = 0;
	for (uint8_t i = 0; i < samples; i++)
	{
		V_Bat1 = V_Bat1 + analogRead(Batpin1);
		V_Bat2 = V_Bat2 + analogRead(Batpin2);
	}

	V_Bat1 = V_Bat1 / samples;
	V_Bat2 = V_Bat2 / samples;

	Serial.print("V1: \t");
	Serial.println(V_Bat1);
	Serial.print("V2: \t");
	Serial.println(V_Bat2);

	V_Bat1 = V_Bat1 * 0.0016414;
	V_Bat2 = V_Bat2 * 0.0027313 - V_Bat1;

	LCD1.show(V_Bat1);
	update();
	delay(2000);
	LCD1.show(V_Bat2);
	update();
	//delay(2000);

}

float starttime;
float time;

void v_showbutton()
{
	cHSV oldcolor;
	oldcolor = LCD1.hsv[0];
	if (v_buttonshow == 1)LCD1.setcolorhsv(0, 20, 1, 1);
	if (v_buttonshow == 2)LCD1.setcolorhsv(0, 340, 1, 1);
	LCD1.show(V_Button[v_buttonshow - 1]);
	update();
	delay(2000);
	LCD1.clearALL();
	update();
	LCD1.setcolorhsv(0, oldcolor.H, oldcolor.S, oldcolor.V);
	v_buttonshow = 0;
}

void update()
{
	timerAlarmDisable(timer1);
	timerAlarmDisable(timer2);
	timerAlarmDisable(timer3);

	LCD1.update();
	LCD1.writeLEDs();
	sleepcounter++;

	timerAlarmEnable(timer1);
	timerAlarmEnable(timer2);
	timerAlarmEnable(timer3);
}

void checkbat()
{
	uint8_t samples = 10;
	V_Bat1 = 0, V_Bat2 = 0;
	for (uint8_t i = 0; i < samples; i++)
	{
		V_Bat1 = V_Bat1 + analogRead(Batpin1);
		V_Bat2 = V_Bat2 + analogRead(Batpin2);
	}
	V_Bat1 = V_Bat1 / samples;
	V_Bat2 = V_Bat2 / samples;

	V_Bat1 = V_Bat1 * 0.0016414;
	V_Bat2 = V_Bat2 * 0.0027313 - V_Bat1;

	Serial.print("V1: \t");
	Serial.println(V_Bat1);
	Serial.print("V2: \t");
	Serial.println(V_Bat2);

	float lowbat = 3.2;

	if ((V_Bat1 < lowbat) || (V_Bat2 < lowbat))
	{
		V_Batlow = 1;
		LCD1.setcolorhsv(0, 0, 1, 5);
		LCD1.show(min(V_Bat1, V_Bat2));
		sleepcounter = 0;
		sleeping = 0;
	}
}
void loop()
{
	if (!V_Batlow)
	{
		if (millis() - oldtime_V > 10000)
			oldtime_V = millis(), checkbat();// Serial.println("up");

		if (v_buttonshow)
			v_showbutton();

		if(LCD1.animation!=4)
			mode1();
	}

	if ((sleepcounter > 300 && !sleeping) && !LCD1.animation)
		LCD1.fadeout(1, 1), sleeping = 1;

	if (millis() - oldtime_l > 10)
		oldtime_l = millis(), update();// Serial.println("up");

}

void mode0()
{

}

uint8_t m1_mode = 0;
float m1_break_start;
float m1_break_stop;
float m1_break = 0;

void mode1()
{
	if (remotereset >= 5)
	{
		m1_mode = 0;
		LCD1.setZero();
		delay(300);
		remotebutton = 0;
	}

	if (recordset)
	{
		if (!LCD1.animation)
			recordset = 0, LCD1.colorWheel(1,2,0);
	}

	//Start
	if ((buttons == 1 && !button_used) || remotebutton == 1)
	{
		sleeping = 0;
		remotebutton = 0;
		if (m1_mode == 2)
			m1_break = m1_break + millis() / 1000 - m1_break_start;

		if (m1_mode == 0)
		{
			LCD1.setZero();
			starttime = (millis()-remotedelay[0]) / 1000;
			m1_break = 0;
		}
		button_used = 1;
		m1_mode = 1;

		oldtime = 0;
	}

	//Stop
	if ((buttons == 2 && !button_used) || remotebutton == 2)
	{
		sleeping = 0;
		remotebutton = 0;
		button_used = 1;
		m1_mode = 0;
		m1_break_start = (millis()-remotedelay[1]) / 1000;
		LCD1.anishow(time, 1, 1);

		if (record == 0 || time < record)
			recordset = 1, record = time;
		
		//oldtime = 0;
	}

	//Reset
	if (buttons == 4 && !button_used)
	{
		sleeping = 0;
		starttime = millis() / 1000;
		button_used = 1;
		m1_mode = 0;
		LCD1.setZero();
		oldtime = 0;
	}

	if (buttons == 8 && !button_used)
	{
		if (record != 0)
			record = 0;
	}

	//stop
	if (m1_mode == 0)
	{

	}

	//run
	if (m1_mode == 1)
	{
		sleepcounter = 0;

		time = (float)millis() / 1000 - starttime - m1_break;
		if ((time - oldtime) >= 0.01)
		{
			//Serial.println(time);
			LCD1.anishowcontinuous(time, 1, 2);
			oldtime = time;
		}
	}

	if (buttons == 32 && !button_used)
	{
		button_used = 1;
		LCD1.setcolorhsv(0, LCD1.hsv[0].H + 30, LCD1.hsv[0].S, LCD1.hsv[0].V);
		Serial.print("newHue: "), Serial.println(LCD1.hsv[0].H);
		delay(300);
		sleepcounter = 0;
		sleeping = 0;
		LCD1.show(13.37);
	}
}

void mode2()
{

}