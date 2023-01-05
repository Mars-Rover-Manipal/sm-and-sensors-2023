#include <WiFi.h>

const int Rdir = 5;
const int Ldir = 25;
const int Rpwm = 18;
const int Lpwm = 33;

const int freq = 8000;
const int Lchannel = 0;
const int Rchannel = 1;
const int resolution = 8;

const int fact = 255 / 8000;

int x = 0, y = 0;
float gear = 1.0;
char c;
char data[16] = "000000000000000";

const char *ssid = "Raven";
const char *password = "wifi041102";

WiFiServer wifiServer(80);

void MotorCode(int x, int y, float gear)
{
    // STOP
    if (abs(x) < 20 && abs(y) < 20)
    {
        digitalWrite(Ldir, LOW);
        digitalWrite(Rdir, LOW);
        ledcWrite(Lchannel, 0);
        ledcWrite(Rchannel, 0);
    }

    // FORWARD MAX
    else if (abs(x) < 10 && y > 0)
    {
        digitalWrite(Ldir, HIGH);
        digitalWrite(Rdir, HIGH);
        ledcWrite(Lchannel, (uint32_t)(abs(y) * (gear * 0.1) * fact));
        ledcWrite(Rchannel, (uint32_t)(abs(y) * (gear * 0.1) * fact));
    }

    // BACKWARD MAX
    else if (abs(x) < 10 && y < 0)
    {
        digitalWrite(Ldir, LOW);
        digitalWrite(Rdir, LOW);
        ledcWrite(Lchannel, (uint32_t)(abs(y) * (gear * 0.1) * fact));
        ledcWrite(Rchannel, (uint32_t)(abs(y) * (gear * 0.1) * fact));
    }

    // SPOT LEFT
    else if (x < 0 && abs(y) <= 10)
    {
        digitalWrite(Ldir, LOW);
        digitalWrite(Rdir, HIGH);
        ledcWrite(Lchannel, (uint32_t)(abs(x) * (gear * 0.1) * fact));
        ledcWrite(Rchannel, (uint32_t)(abs(x) * (gear * 0.1) * fact));
    }

    // SPOT RIGHT
    else if (x > 0 && abs(y) <= 10)
    {
        digitalWrite(Ldir, HIGH);
        digitalWrite(Rdir, LOW);
        ledcWrite(Lchannel, (uint32_t)(abs(x) * (gear * 0.1) * fact));
        ledcWrite(Rchannel, (uint32_t)(abs(x) * (gear * 0.1) * fact));
    }

    // OCTET 1
    else if (x > 0 && y > 0 && x > y)
    {
        digitalWrite(Ldir, HIGH);
        digitalWrite(Rdir, LOW);
        ledcWrite(Lchannel, (uint32_t)(abs(x) * (gear * 0.1) * fact));
        ledcWrite(Rchannel, (uint32_t)(abs(abs(x) - abs(y)) * (gear * 0.1) * fact));
    }

    // OCTET 2
    else if (x > 0 && y > 0 && x < y)
    {
        digitalWrite(Ldir, HIGH);
        digitalWrite(Rdir, HIGH);
        ledcWrite(Lchannel, (uint32_t)(abs(y) * (gear * 0.1) * fact));
        ledcWrite(Rchannel, (uint32_t)(abs(abs(x) - abs(y)) * (gear * 0.1) * fact));
    }

    // OCTET 3
    else if (x < 0 && y > 0 && abs(x) < y)
    {
        digitalWrite(Ldir, HIGH);
        digitalWrite(Rdir, HIGH);
        ledcWrite(Lchannel, (uint32_t)(abs(abs(x) - abs(y)) * (gear * 0.1) * fact));
        ledcWrite(Rchannel, (uint32_t)(abs(y) * (gear * 0.1) * fact));
    }

    // OCTET 4
    else if (x < 0 && y > 0 && abs(x) >= y)
    {
        digitalWrite(Ldir, LOW);
        digitalWrite(Rdir, HIGH);
        ledcWrite(Lchannel, (uint32_t)(abs(abs(x) - abs(y)) * (gear * 0.1) * fact));
        ledcWrite(Rchannel, (uint32_t)(abs(x) * (gear * 0.1) * fact));
    }

    // OCTET 5
    else if (x < 0 && y < 0 && abs(x) > abs(y))
    {
        digitalWrite(Ldir, LOW);
        digitalWrite(Rdir, HIGH);
        ledcWrite(Lchannel, (uint32_t)(abs(x) * (gear * 0.1) * fact));
        ledcWrite(Rchannel, (uint32_t)(abs(abs(x) - abs(y)) * (gear * 0.1) * fact));
    }

    // OCTET 6
    else if (x < 0 && y < 0 && abs(x) < abs(y))
    {
        digitalWrite(Ldir, LOW);
        digitalWrite(Rdir, LOW);
        ledcWrite(Lchannel, (uint32_t)(abs(y) * (gear * 0.1) * fact));
        ledcWrite(Rchannel, (uint32_t)(abs(abs(x) - abs(y)) * (gear * 0.1) * fact));
    }

    // OCTET 7
    else if (x > 0 && y < 0 && abs(x) < abs(y))
    {
        digitalWrite(Ldir, LOW);
        digitalWrite(Rdir, LOW);
        ledcWrite(Lchannel, (uint32_t)(abs(abs(x) - abs(y)) * (gear * 0.1) * fact));
        ledcWrite(Rchannel, (uint32_t)(abs(y) * (gear * 0.1) * fact));
    }

    // OCTET 8
    else if (x > 0 && y < 0 && abs(x) > abs(y))
    {
        digitalWrite(Ldir, HIGH);
        digitalWrite(Rdir, LOW);
        ledcWrite(Lchannel, (uint32_t)(abs(abs(x) - abs(y)) * (gear * 0.1) * fact));
        ledcWrite(Rchannel, (uint32_t)(abs(x) * (gear * 0.1) * fact));
    }
}

void setup()
{
    Serial.begin(115200);
    delay(1000);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the WiFi network");
    Serial.println(WiFi.localIP());
    wifiServer.begin();

    ledcSetup(Lchannel, freq, resolution);
    ledcSetup(Rchannel, freq, resolution);
    ledcAttachPin(Lpwm, Lchannel);
    ledcAttachPin(Rpwm, Rchannel);
    pinMode(Ldir, OUTPUT);
    pinMode(Rdir, OUTPUT);
}

void loop()
{
    int index = 0;
    WiFiClient client = wifiServer.available();
    if (client)
    {
        while (client.connected())
        {
            while (client.available() > 0)
            {
                char c = client.read();
                if (index < 15)
                {
                    data[index] = c;
                    index++;
                    data[index] = '\0';
                }
                Serial.println(data);
                client.write(c);
            }
            delay(10);
        }
        client.stop();
        Serial.println("Client disconnected");
    }

    gear = data[1];

    x = data[3] * 10000 + data[4] * 1000 + data[5] * 100 + data[6] * 10 + data[7];
    y = data[9] * 10000 + data[10] * 1000 + data[11] * 100 + data[12] * 10 + data[13];

    x = x - 8000;
    y = y - 8000;

    if (abs(x) < 500)
        x = 0;
    if (abs(y) < 500)
        y = 0;

    MotorCode(x, y, gear);
}
