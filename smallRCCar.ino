/*   
Code credit:
https://github.com/GungorYalcin/ArduinoDersleri/tree/master/gy_NodeMCU_Wifi_car_Blynk
*/
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
char auth[] = "YourBlynkToken"; //YourToken
// Your WiFi credentials.
char ssid[] = "SSID Name"; //YourNetworkName
char pass[] = "Your SSID Password"; //YourPassword
BlynkTimer timer;

const byte L298N_A_pin = D1; // GPI05
const byte L298N_A_In1_pin = D2; //GPI04
const byte L298N_A_In2_pin = D3; //GPI0

const byte L298N_B_In3_pin = D4; // GPI02
const byte L298N_B_In4_pin = D5; // GPIO14
const byte L298N_B_pin = D6; //GPI12

const byte Led1_pin =  D7; //GPIO13  // Left Turn Signal
const byte Led2_pin =  D8; //GPIO15  // Right Turn Signal
const byte Led3_pin =  D9; //GPIO3   // On Headlight 
const byte Led4_pin = D10; //GPI01   // Rear Light
const byte Horn_Pin = D0; //GPI016  //Horn

WidgetLED led1(V1); //Left
WidgetLED led2(V2); //Right
WidgetLED led3(V3); //A1
WidgetLED led4(V4); //B1
WidgetLED led5(V5); //Headlight
WidgetLED led6(V6); //Back 
WidgetLED led7(V7); //A2
WidgetLED led8(V8); //B2

byte LeftTurnSignal = 0; //Left Turn Signal
byte RightTurnSignal = 0; //Right Turn Signal
byte RearLight = 0; //Rear light
/*
 * blynk display
 * V0 joystick,  
 * v1,v2, v3, v4,v5,v6,v7,v8 Led  
 * v10  button push
 * v11 button switch
 */

BLYNK_WRITE(V10) {
/* Horn*/  
    digitalWrite(Horn_Pin,param.asInt());
}

BLYNK_WRITE(V11) {
/* Front headlight*/  
    digitalWrite(Led3_pin,param.asInt());
}

void motorSpeed(int prmA, byte prmA1, byte prmA2, int prmB, byte prmB1, byte prmB2)
{
    analogWrite(L298N_A_pin,prmA);
    analogWrite(L298N_B_pin,prmB);

    digitalWrite(L298N_A_In1_pin,prmA1);
    digitalWrite(L298N_A_In2_pin,prmA2);
    digitalWrite(L298N_B_In3_pin,prmB1);
    digitalWrite(L298N_B_In4_pin,prmB2);

    if (prmA1==LOW) 
        led3.off();
    else
        led3.on();

    if (prmA2==LOW) 
        led7.off();
    else
        led7.on();

    if (prmB1==LOW) 
        led4.off();
    else
        led4.on();

    if (prmB2==LOW) 
        led8.off();
    else
        led8.on();
}

BLYNK_WRITE(V0) {
    int x = param[0].asInt();
    int y = param[1].asInt();
// x =  -2 -1 0 1 2 
// Y =  -2 -1 0 1 2 
    if (y>=0)
    {
        RearLight = 0;
        digitalWrite(Led4_pin,LOW);
    }
    else
    {
        RearLight = 1;
        LeftTurnSignal = 1;
        RightTurnSignal = 1;
        digitalWrite(Led1_pin,HIGH);
        digitalWrite(Led2_pin,HIGH);
        digitalWrite(Led4_pin,HIGH);
    }
 
    if ((x==0) && (y==0)) // stands
    {
        motorSpeed(0,LOW,LOW,0,LOW,LOW); 
        LeftTurnSignal = 0;
        RightTurnSignal = 0;
        digitalWrite(Led1_pin,LOW);
        digitalWrite(Led2_pin,LOW);
    }
    else if ((x==0) && (y>0)) // straight forward 
    {
    if (y==1){ motorSpeed(900,HIGH,LOW,900,HIGH,LOW); }
    else { motorSpeed(1023,HIGH,LOW,1023,HIGH,LOW); }
        LeftTurnSignal = 0;
        RightTurnSignal = 0;
        digitalWrite(Led1_pin,LOW);
        digitalWrite(Led2_pin,LOW);
    }
    else if ((y==0) && (x>0))// Full right turn
    { 
        motorSpeed(900,HIGH,LOW,900,LOW,HIGH); 
        RightTurnSignal = 1;
        digitalWrite(Led2_pin,HIGH);
    }
    else if ((y>0) && (x>0))// Turn right
    {  
        motorSpeed(1023,HIGH,LOW,900,LOW,LOW); 
        RightTurnSignal = 1;
        digitalWrite(Led2_pin,HIGH);
    }
    else if ((y==0) && (x<0))// Full left turn
    { 
        motorSpeed(900,LOW,HIGH,900,HIGH,LOW); 
        LeftTurnSignal = 1;
        digitalWrite(Led1_pin,HIGH);
    }
    else if ((y>0) && (x<0))// Turn left
    {   
        motorSpeed(900,LOW,LOW,1023,HIGH,LOW); 
        LeftTurnSignal = 1;
        digitalWrite(Led1_pin,HIGH);
    }

    if ((y<0) && (x<0))// Back left
        motorSpeed(900,LOW,LOW,1023,LOW,HIGH); 
    else if ((y<0) && (x>0))// Right back
        motorSpeed(1023,LOW,HIGH,900,LOW,LOW); 
    else if ((y<0) && (x==0))// Full back
        motorSpeed(900,LOW,HIGH,900,LOW,HIGH); 
}

void setup() {
    Serial.begin(115200);
    pinMode(L298N_A_In1_pin,OUTPUT);
    pinMode(L298N_A_In2_pin,OUTPUT);
    pinMode(L298N_B_In3_pin,OUTPUT);
    pinMode(L298N_B_In4_pin,OUTPUT);

    digitalWrite(L298N_A_In1_pin,LOW);
    digitalWrite(L298N_A_In2_pin,LOW);
    digitalWrite(L298N_B_In3_pin,LOW);
    digitalWrite(L298N_B_In4_pin,LOW);

    pinMode(Led1_pin,OUTPUT);
    pinMode(Led2_pin,OUTPUT);
    pinMode(Led3_pin,OUTPUT);
    pinMode(Led4_pin,OUTPUT);

    digitalWrite(Led1_pin,LOW);
    digitalWrite(Led2_pin,LOW);
    digitalWrite(Led3_pin,LOW);
    digitalWrite(Led4_pin,LOW);
    Blynk.begin(auth, ssid, pass);
    timer.setInterval(1000L, sendTemps);
}

void sendTemps()
{
    if (LeftTurnSignal==1)
        digitalWrite(Led1_pin,!digitalRead(Led1_pin));

    if (RightTurnSignal==1)
        digitalWrite(Led2_pin,!digitalRead(Led2_pin));

    if (RearLight==1)
        digitalWrite(Led4_pin,!digitalRead(Led4_pin));

    if (digitalRead(Led1_pin)==HIGH)
        led1.on();
    else
        led1.off();

    if (digitalRead(Led2_pin)==HIGH)
        led2.on();
    else
        led2.off();

    if (digitalRead(Led3_pin)==HIGH)
        led5.on();
    else
        led5.off();

    if (digitalRead(Led4_pin)==HIGH)
        led6.on();
    else
        led6.off();
}

void loop() {
    Blynk.run();
    timer.run();
}