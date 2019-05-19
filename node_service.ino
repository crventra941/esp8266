#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

// WiFi config
const char wifissid[] = "INFINITUM17D5A5";
const char wifipsk[] = "F15850E73F";

// OUTPUT Definitions
const int OUTPUT_PIN = 16; // D0

WiFiServer server(80);

void setup()
{
    initHardWare();
    connectWiFi();
    server.begin(); // Server instance in port 80
    setupMDNS();
}

void loop()
{
    WiFiClient client = server.available();

    char statusSystem[] = "{\"system\":\"ready\"}";

    if (!client)
    {
        return;
    }

    String request = client.readStringUntil('\r');
    Serial.println(request);

    String header = "HTTP/1.1 200 OK\r\n";
    header += "Access-Control-Allow-Methods: POST,GET,OPTIONS\r\n";
    header += "Access-Control-Max-Age: 10000\r\n";
    header += "Access-Control-Allow-Origin: *\r\n";
    header += "Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept\r\n";
    header += "Content-Type: application/json\r\n\r\n";

    int alarmStatus = -1;

    if (request.indexOf("/alarm/status") != -1)
    {
        client.print(header);
        client.print(statusSystem);
    }
    if (request.indexOf("/alarm/1") != -1)
    {
        alarmStatus = 1;
        client.print(header);
        client.print("{\"alarm\":\"on\"}");
    }
    if (request.indexOf("/alarm/0") != -1)
    {
        alarmStatus = 0;
        client.print(header);
        client.print("{\"alarm\":\"off\"}");
    }

    digitalWrite(OUTPUT_PIN, alarmStatus);

    client.flush();
    delay(1);
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

    StaticJsonDocument<200> doc;

    digitalWrite(OUTPUT_PIN, LOW);
}

// Setup End Region #
