#include <Arduino.h>
#include <WiFi.h>

const int freq = 5000;
const int channel1 = 0;
const int channel2 = 1;
const int channel3 = 2;
const int resolution = 8;

const char *ssid = "mrm@base";
const char *password = "mrm@2023";
IPAddress local_IP(192, 168, 1, 147);
IPAddress subnet(255, 255, 255, 0);
IPAddress gateway(192, 168, 1, 1);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(4, 4, 4, 4);

WiFiServer wifiServer(80);

char rxData = 'd';
int sm = 0;

void setup()
{
    Serial.begin(115200);
    delay(1000);
    WiFi.config(local_IP, subnet, gateway, primaryDNS, secondaryDNS);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Please connection lagja...");
    }
    Serial.println("Data is in the air!");
    Serial.println(WiFi.localIP());
    wifiServer.begin();

    pinMode(13, OUTPUT); // relay 1
    // pinMode(4, OUTPUT);  // relay 2
    // pinMode(5, OUTPUT);  // water heater
    pinMode(23, OUTPUT); // stepper step
    pinMode(22, OUTPUT); // stepper direction
    pinMode(32, OUTPUT); // auger actuation direction
    pinMode(25, OUTPUT); // auger rotation direction
    pinMode(27, OUTPUT); // sensor suite direction

    ledcSetup(channel1, freq, resolution);
    ledcSetup(channel2, freq, resolution);
    ledcSetup(channel3, freq, resolution);

    ledcAttachPin(33, channel1); // auger actuation PWM
    ledcAttachPin(26, channel2); // auger rotation PWM
    ledcAttachPin(14, channel3); // sensor suite PWM
}

void SMControl(int sm)
{
    switch (sm)
    {
    case 0: // safety
        ledcWrite(channel1, 0);
        ledcWrite(channel2, 0);
        ledcWrite(channel3, 0);
        digitalWrite(13, LOW);
        digitalWrite(23, LOW);
        digitalWrite(22, LOW);
        digitalWrite(32, LOW);
        digitalWrite(25, LOW);
        digitalWrite(27, LOW);
        // digitalWrite(17, LOW);
        // digitalWrite(18, LOW);
        break;

    case 1: // relay 1
        digitalWrite(13, HIGH);
        break;

    case 2: // relay 2
        digitalWrite(4, HIGH);
        break;

    case 3: // water heater
        digitalWrite(5, HIGH);
        break;

    case 4: // servo rotate
        digitalWrite(23, HIGH);
        delay(50);
        digitalWrite(23, LOW);
        delay(50);
        break;

    case 5: // servo direction toggle
        digitalWrite(22, !digitalRead(22));
        //may have to add delay
        break;

    case 6: // auger down with rotation
        digitalWrite(32, HIGH);
        digitalWrite(25, HIGH);
        ledcWrite(channel1, 255);
        ledcWrite(channel2, 255);
        break;

    case 7: // auger up
        digitalWrite(32, LOW);
        digitalWrite(25, LOW);
        ledcWrite(channel1, 255);
        ledcWrite(channel2, 0);
        break;

    case 8: // auger rotation for deposition
        digitalWrite(32, LOW);
        digitalWrite(25, LOW);
        ledcWrite(channel1, 0);
        ledcWrite(channel2, 255);
        break;

    case 9: // sensor suite up
        digitalWrite(27, HIGH);
        ledcWrite(channel3, 255);
        break;

    case 10: // sensor suite down
        digitalWrite(27, LOW);
        ledcWrite(channel3, 255);
        break;

    default:
        ledcWrite(channel1, 0);
        ledcWrite(channel2, 0);
        ledcWrite(channel3, 0);
        break;
    }
}

void loop()
{
    WiFiClient client = wifiServer.available();
    if (client)
    {
        while (client.connected())
        {
            while (client.available() > 0)
            {
                rxData = client.read();
                sm = rxData - '0';
                client.write(rxData);
            }
            delay(10);
            SMControl(sm);
            Serial.println(sm);
        }
        client.stop();
        Serial.println("Khatam bhencho...");
    }
}