#include <Arduino.h>
#include <cmath>
#include <string>

#define LED_RED 12
#define LED_GREEN 27
#define LED_BLUE 14
#define CLK 34
#define DT 39
#define SW 36 // pressed = low, unperessed = high
#define SAT 4 //saturation
#define BRI 35 //brightness

bool LEDState = true;
int lastState = HIGH;
int prevCLK = HIGH;
unsigned long lastDebounceTime = 0;
int lastBri = 0;
int lastSat = 0;

double hue = 0.0;
int saturation = 100;
int brightness = 100;

double mod(double n, double modulo)
{
    return n - modulo * floor(n / modulo);
}
int clamp(int n, int min, int max)
{
    if (n < min) return min;
    if (n > max) return max;
    return n;
}

void setHSV(int hue, int saturation, int value)
{
    hue = mod(hue, 360);
    int red = 0;
    int green = 0;
    int blue = 0;

    if (hue >= 0 && hue < 60)
    {
        red = 255;
        green = (255 * hue) / 60;
    }
    else if (hue >= 60 && hue < 120)
    {
        green = 255;
        red = 255 - (255 * (hue - 60) / 60);
    }
    else if (hue >= 120 && hue < 180)
    {
        green = 255;
        blue = (255 * (hue - 120) / 60);
    }
    else if (hue >= 180 && hue < 240)
    {
        blue = 255;
        green = 255 - (255 * (hue - 180) / 60);
    }
    else if (hue >= 240 && hue < 300)
    {
        blue = 255;
        red = (255 * (hue - 240) / 60);
    }
    else
    {
        red = 255;
        blue = 255 - (255 * (hue - 300) / 60);
    }


    //saturation
    int finalR = red + (255 - red) * (1 - saturation / 100.0);
    int finalG = green + (255 - green) * (1 - saturation / 100.0);
    int finalB = blue + (255 - blue) * (1 - saturation / 100.0);

    //brightness
    finalR = finalR * (value / 100.0);
    finalG = finalG * (value / 100.0);
    finalB = finalB * (value / 100.0);

    std::string out("(" + std::to_string(hue) + "deg, " + std::to_string(saturation) + "%, " + std::to_string(value) + "%), {" + std::to_string(finalR) + ", " + std::to_string(finalG) + ", " + std::to_string(finalB) + "}");

    Serial.println(out.c_str());

    analogWrite(LED_RED, finalR);
    analogWrite(LED_GREEN, finalG);
    analogWrite(LED_BLUE, finalB);
}


void setOffOn()
{
    unsigned long debounceDelay = 150;
    int currentState = digitalRead(SW);

    if (lastState == HIGH && currentState == LOW)
    {
        unsigned long now = millis();
        if (now - lastDebounceTime > debounceDelay)
        {
            LEDState = !LEDState;
            if (LEDState)
            {
                setHSV(hue, saturation, brightness);
            }
            else
            {
                analogWrite(LED_RED, 0);
                analogWrite(LED_GREEN, 0);
                analogWrite(LED_BLUE, 0);
            }
            lastDebounceTime = now;
        }
    }

    lastState = currentState;
}

void setHue()
{
    unsigned long debounceDelay = 150;
    int currentCLK = digitalRead(CLK);
    int currentDT = digitalRead(DT);
    if (currentCLK == LOW && prevCLK == HIGH && LEDState)
    {
        unsigned long now = millis();
        if (now - lastDebounceTime > debounceDelay)
        {
            if (currentDT == HIGH)
            {
                setHSV(hue += 5, saturation, brightness);
            }
            else
            {
                setHSV(hue -= 5, saturation, brightness);
            }
            lastDebounceTime = now;
        }
    }
    prevCLK = currentCLK;
}

void setBrightness()
{
    int currentBri = analogRead(BRI);
    if (abs(currentBri - lastBri) > (4095/70))
    {
        double b = currentBri;
        brightness = clamp(((b / 4095) * 104) - 2, 0, 100);
        lastBri = currentBri;
        setHSV(hue, saturation, brightness);
    }
}

void setSaturation()
{
    int currentSat = analogRead(SAT);
    if (abs(currentSat - lastSat) > (4095/70))
    {
        double s = currentSat;
        saturation = clamp(((s / 4095) * 104) - 2, 0, 100);
        lastSat = currentSat;
        setHSV(hue, saturation, brightness);
    }
}


void setup()
{
    Serial.begin(115200);
    pinMode(SW, INPUT);
    pinMode(CLK, INPUT);
    pinMode(DT, INPUT);
    pinMode(SAT, INPUT);
    pinMode(BRI, INPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    setHSV(hue, saturation, brightness);
}

void loop()
{
    setOffOn();
    setHue();
    setSaturation();
    setBrightness();
}