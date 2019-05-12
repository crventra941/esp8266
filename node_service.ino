#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

// WiFi config
const char wifissid[] = "INFINITUM17D5A5";
const char wifipsk[] = "F15850E73F";

// OUTPUT Definitions
const int OUTPUT_PIN = 16;  // D0

WiFiServer server(80);

void setup()
{
    initHardWare();
    connectWiFi();
    server.begin();         // Server instance in port 80
    setupMDNS();
}

void loop()
{
    WiFiClient client = server.available();

    char statusSystem[] =
        "{\"system\":\"ready\"}";

    if (!client)
    {
        return;
    }

    String request = client.readStringUntil('\r');
    Serial.println(request);

    String header = "HTTP/1.1 200 OK\r\n";
    header += "Content-Type: application/json\r\n\r\n";

    String systemInfo = "";

    int alarmStatus = -1;

    if (request.indexOf("/alarm/status") != -1)
    {
        client.print(header);
        client.print(statusSystem);
    }
    if (request.indexOf("/alarm/1") != -1)
    {
        alarmStatus = 1;
    }
    if (request.indexOf("/alarm/0") != -1)
    {
        alarmStatus = 0;
    }

    if (alarmStatus >= 0)
    {
        digitalWrite(OUTPUT_PIN, alarmStatus);
        systemInfo = outputStatus(alarmStatus);
    }

    client.print(header);
    client.print(systemInfo);

    client.flush();
    delay(1);
}

String outputStatus(int alarmSt)
{
    String response;
    if (alarmSt >= 0)
    {
        response = (alarmSt) ? "{\"alarm\":\"on\"}" : "{\"alarm\":\"off\"}";
    }
    return response;
}

// # Setup Region
void connectWiFi()
{
    byte ledStatus = LOW;
    Serial.println();
    Serial.println("Conecting to: " + String(wifissid));

    WiFi.mode(WIFI_AP);
    WiFi.begin(wifissid, wifipsk);

    while (WiFi.status() != WL_CONNECTED)
    {
        // Enable OUTPUT
        digitalWrite(OUTPUT_PIN, ledStatus);
        ledStatus = (ledStatus == LOW) ? LOW : HIGH;

        delay(100);
    }

    Serial.println("Wifi connected!");
    Serial.println(WiFi.localIP());
    digitalWrite(OUTPUT_PIN, LOW);
}

void setupMDNS()
{
    //Call mdns to setup mdns to pint to domain.local
    if (!MDNS.begin("thing"))
    {
        Serial.println("Error setting up MDNS responder!");
        while (1)
        {
            delay(1000);
        }
    }
    Serial.println("mDNS responder started");
}

void initHardWare()
{
    Serial.begin(115200);

    pinMode(OUTPUT_PIN, OUTPUT);

    digitalWrite(OUTPUT_PIN, LOW);
}

// Setup End Region #
