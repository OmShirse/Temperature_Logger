#include "DHT.h"


#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);


const long READ_INTERVAL_MS = 2000;
unsigned long lastReadTime = 0;


#define GPIO_OUT_W1TS 0x60000304  // Set GPIO HIGH
#define GPIO_OUT_W1TC 0x60000308  // Set GPIO LOW
#define GPIO_ENABLE_W1TS 0x60000310

#define LED_GPIO 2  // Built-in LED on many NodeMCU boards


#define REG_WRITE(addr, val) (*((volatile uint32_t *)(addr)) = (val))


void led_on() {
  REG_WRITE(GPIO_OUT_W1TC, (1 << LED_GPIO)); 
}

void led_off() {
  REG_WRITE(GPIO_OUT_W1TS, (1 << LED_GPIO)); 
}

void setup() {

  Serial.begin(115200);
  Serial.println();
  Serial.println(F("--- DHT11 Logger Starting (Memory Mapped GPIO Demo) ---"));

  dht.begin();

  REG_WRITE(GPIO_ENABLE_W1TS, (1 << LED_GPIO));

  led_off();
}


void loop() {

  unsigned long currentTime = millis();

  if (currentTime - lastReadTime >= READ_INTERVAL_MS) {

    lastReadTime = currentTime;

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);

    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println(F("ERROR: Failed to read from DHT sensor! Check wiring."));
      led_on();   
      return;
    }

    led_off();

    float hic = dht.computeHeatIndex(t, h, false);
    float hif = dht.computeHeatIndex(f, h);

    Serial.print(F("Time [ms]: "));
    Serial.print(currentTime);
    Serial.print(F(" | Hum: "));
    Serial.print(h);
    Serial.print(F("% | Temp: "));
    Serial.print(t);
    Serial.print(F("°C / "));
    Serial.print(f);
    Serial.print(F("°F | HI: "));
    Serial.print(hic);
    Serial.print(F("°C / "));
    Serial.print(hif);
    Serial.println(F("°F"));
  }
}
