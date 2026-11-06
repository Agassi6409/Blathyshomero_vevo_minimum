#include <SPI.h>
#include <RadioLib.h>
// ESP32-C3 – SX1276 bekötés SX1276: NSS/CS, DIO0, RST, DIO1 (nincs), SPI busz
#define PIN_LORA_CS    7
#define PIN_LORA_DIO0  0
#define PIN_LORA_RST   10
#define PIN_LORA_DIO1  RADIOLIB_NC   // nincs bekötve
#define PIN_SPI_SCK    4
#define PIN_SPI_MISO   5
#define PIN_SPI_MOSI   6

SX1276 lora = new Module(PIN_LORA_CS, PIN_LORA_DIO0, PIN_LORA_RST, PIN_LORA_DIO1,
                         SPI, SPISettings(1000000, MSBFIRST, SPI_MODE0));
void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
  
  SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI);

  Serial.println(F("LoRa vevő indul..."));

  int16_t state = lora.begin(868.0, 125.0, 9, 7, 0x12);

  if (state != RADIOLIB_ERR_NONE) {
    Serial.print(F("HIBA: LoRa init, kod="));
    Serial.println(state);
    while (true) {}
  }
  Serial.println(F("LoRa modul inicializálva"));
  Serial.println(F("Vételre kész. Várakozás LoRa csomagra..."));
}

void loop() {
  uint8_t buf[64];
  size_t bufSize = sizeof(buf);
  int16_t state = lora.receive(buf, bufSize);

  if (state == RADIOLIB_ERR_NONE) {
    size_t len = lora.getPacketLength();
    Serial.println(F("\nCsomag fogadva!"));
    Serial.print(F("  Hossz (byte): "));
    Serial.println(len);

    Serial.print(F("  HEX: "));
    for (size_t i = 0; i < len && i < bufSize; i++) {
      if (buf[i] < 16) Serial.print('0');
      Serial.print(buf[i], HEX);
      Serial.print(' ');
    }
    Serial.println();

  } else if (state != RADIOLIB_ERR_RX_TIMEOUT) {
    Serial.print(F("Fogadási hiba: "));
    Serial.println(state);
  }
  delay(10);
}
