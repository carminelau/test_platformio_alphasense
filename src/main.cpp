// ============================================================
// main.cpp  –  Alphasense B43F periodic ADC reader
// ESP32-S3 · Arduino framework · PlatformIO
// ============================================================
// What this firmware does
// -----------------------
//   • Reads NUM_ADC_CHANNELS ADC inputs (see config.h)
//   • Every SAMPLE_INTERVAL_MS ms it:
//       – oversample (OVERSAMPLING_COUNT reads, averaged)
//       – apply calibration offset/gain
//       – update a moving-average filter
//       – print results over Serial (USB-CDC or UART)
// ============================================================

#include <Arduino.h>
#include "config.h"
#include "alphasense_b43f.h"

// ------------------------------------------------------------
// Forward declarations
// ------------------------------------------------------------
static void printHeader();
static void printChannelData(const ChannelData& d);

// ------------------------------------------------------------
// setup()
// ------------------------------------------------------------
void setup() {
    Serial.begin(SERIAL_BAUD);

    // Wait up to 3 s for the host to open the CDC port.
    // On boards with hardware UART this returns immediately.
    unsigned long t0 = millis();
    while (!Serial && (millis() - t0) < 3000UL) { delay(10); }

    Serial.println(F("\n========================================"));
    Serial.println(F("  Alphasense B43F ADC reader"));
    Serial.println(F("  ESP32-S3 / Arduino / PlatformIO"));
    Serial.println(F("========================================"));
    Serial.printf("  Channels         : %d\n", NUM_ADC_CHANNELS);
    Serial.printf("  Sample interval  : %u ms\n", SAMPLE_INTERVAL_MS);
    Serial.printf("  Oversampling     : %u\n", OVERSAMPLING_COUNT);
    Serial.printf("  Moving-avg window: %u\n", MOVING_AVG_SIZE);
    Serial.printf("  ADC resolution   : %d bit\n", ADC_RESOLUTION_BITS);
    Serial.printf("  Vref (full-scale): %.0f mV\n", VREF_MV);
    Serial.println(F("----------------------------------------"));
    Serial.println(F("NOTE: voltages are estimates. Calibrate"));
    Serial.println(F("      offset/gain in config.h before use."));
    Serial.println(F("========================================\n"));

    alphasenseInit();

    printHeader();
}

// ------------------------------------------------------------
// loop()
// ------------------------------------------------------------
void loop() {
    static unsigned long lastSampleMs = 0;

    unsigned long now = millis();
    if (now - lastSampleMs < SAMPLE_INTERVAL_MS) return;
    lastSampleMs = now;

    for (uint8_t ch = 0; ch < NUM_ADC_CHANNELS; ch++) {
        ChannelData d = readChannel(ch);
        printChannelData(d);
    }
    Serial.println();  // blank line between bursts
}

// ------------------------------------------------------------
// printHeader()
// Prints a CSV-style column header once at start-up.
// ------------------------------------------------------------
static void printHeader() {
    Serial.println(F("timestamp_ms, channel, adc_raw, voltage_mV, mavg_mV"));
}

// ------------------------------------------------------------
// printChannelData()
// Prints one row per channel in CSV format for easy logging.
// ------------------------------------------------------------
static void printChannelData(const ChannelData& d) {
    Serial.printf("%lu, CH%u, %4d, %7.2f, %7.2f\n",
                  millis(),
                  d.channel,
                  d.adcRaw,
                  d.voltageMV,
                  d.movingAvgMV);
}
