// ============================================================
// main.cpp  –  Alphasense B43F via ADS1115 (I2C)
// ESP32-S3 · Arduino framework · PlatformIO
// ============================================================
// What this firmware does
// -----------------------
//   • Initialises Wire (I2C) and the ADS1115 16-bit ADC
//   • Every SAMPLE_INTERVAL_MS ms it:
//       – reads WE (AIN0) and AE (AIN1) single-ended
//         (or hardware differential if configured)
//       – applies per-channel calibration offset/gain
//       – computes signal = WE - AE
//       – updates EMA (Exponential Moving Average) filter
//       – prints a CSV row over Serial-CDC
// ============================================================

#include <Arduino.h>
#include "config.h"
#include "alphasense_b43f.h"

// ------------------------------------------------------------
// Forward declarations
// ------------------------------------------------------------
static void printHeader();
static void printReading(const B43FReading& r);

// ------------------------------------------------------------
// setup()
// ------------------------------------------------------------
void setup() {
    Serial.begin(SERIAL_BAUD);

    // Wait up to 3 s for the host to open the CDC port.
    unsigned long t0 = millis();
    while (!Serial && (millis() - t0) < 3000UL) { delay(10); }

    Serial.println(F("\n========================================"));
    Serial.println(F("  Alphasense B43F – ADS1115 I2C reader"));
    Serial.println(F("  ESP32-S3 / Arduino / PlatformIO"));
    Serial.println(F("========================================"));
    Serial.printf("  I2C SDA          : GPIO%d\n", SDA_PIN);
    Serial.printf("  I2C SCL          : GPIO%d\n", SCL_PIN);
    Serial.printf("  ADS1115 address  : 0x%02X\n", ADS1115_I2C_ADDRESS);
    Serial.printf("  WE channel       : AIN%d\n", ADS_CH_WE);
    Serial.printf("  AE channel       : AIN%d\n", ADS_CH_AE);
    Serial.printf("  Differential mode: %s\n",
                  ADS1115_DIFFERENTIAL_MODE ? "YES (HW)" : "NO (SW WE-AE)");
    Serial.printf("  EMA alpha        : %.2f\n", (float)EMA_ALPHA);
    Serial.printf("  Sample interval  : %u ms\n", SAMPLE_INTERVAL_MS);
    Serial.println(F("----------------------------------------"));
    Serial.println(F("NOTE: voltages are estimates."));
    Serial.println(F("      Set WE/AE offset+gain in config.h"));
    Serial.println(F("      after calibration with reference."));
    Serial.println(F("========================================\n"));

    if (!adsInit()) {
        Serial.println(F("[WARN] ADS1115 init failed. Will retry each sample."));
    }

    printHeader();
}

// ------------------------------------------------------------
// loop()
// ------------------------------------------------------------
void loop() {
    static unsigned long lastMs = 0;

    unsigned long now = millis();
    if (now - lastMs < SAMPLE_INTERVAL_MS) return;
    lastMs = now;

    B43FReading r = adsRead();
    printReading(r);
}

// ------------------------------------------------------------
// printHeader()
// CSV column header printed once at startup.
// ------------------------------------------------------------
static void printHeader() {
    Serial.println(F("timestamp_ms, we_raw, ae_raw, "
                     "we_v, ae_v, signal_v, "
                     "we_ema, ae_ema, signal_ema"));
}

// ------------------------------------------------------------
// printReading()
// One CSV row per sample burst.
// ------------------------------------------------------------
static void printReading(const B43FReading& r) {
    if (!r.valid) {
        Serial.printf("%lu, ERROR: ADS1115 not responding\n", millis());
        return;
    }
    Serial.printf("%lu, %6d, %6d, "
                  "%8.5f, %8.5f, %9.5f, "
                  "%8.5f, %8.5f, %9.5f\n",
                  millis(),
                  (int)r.we_raw, (int)r.ae_raw,
                  r.we_v,    r.ae_v,    r.signal_v,
                  r.we_ema,  r.ae_ema,  r.signal_ema);
}
