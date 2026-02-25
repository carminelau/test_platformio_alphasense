# Alphasense B43F – ESP32-S3 PlatformIO Reader

Firmware for reading analogue outputs of the **Alphasense B4 Series (B43F)**
electrochemical gas sensor via the **Alphasense ISB (Individual Sensor Board)**
or a compatible analogue interface board, on an **ESP32-S3** running the
**Arduino framework** inside **PlatformIO**.

---

## Quick Start

### Prerequisites

| Tool | Version |
|------|---------|
| [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation/) | ≥ 6.x |
| Python | ≥ 3.8 |
| USB driver for your ESP32-S3 board | – |

### 1. Clone / open the project

```bash
git clone https://github.com/carminelau/test_platformio_alphasense.git
cd test_platformio_alphasense
```

### 2. Configure pins and sampling (⚠ required before first flash)

Edit **`include/config.h`** and replace every `!! SOSTITUIRE CON PIN REALI !!`
placeholder with the real GPIO numbers of your wiring (see **Wiring** section).

### 3. Build

```bash
pio run -e esp32s3
```

### 4. Upload

```bash
pio run -e esp32s3 --target upload
```

### 5. Monitor Serial output

```bash
pio device monitor
```

Expected output (CSV, one row per channel per burst):

```
timestamp_ms, channel, adc_raw, voltage_mV, mavg_mV
1523, CH0,  842,  536.43,  534.12
1523, CH1,  310,  197.27,  196.85
```

---

## Configurazione

All tunables live in **`include/config.h`**:

| Constant | Default | Description |
|----------|---------|-------------|
| `NUM_ADC_CHANNELS` | `2` | Number of ADC channels to read |
| `ADC_PINS[]` | `{4, 5}` ⚠ | GPIO numbers – **replace with real pins** |
| `ADC_RESOLUTION_BITS` | `12` | ADC resolution (9–12 bit) |
| `ADC_ATTEN` | `ADC_11db` | Attenuation (full-scale ≈ 2600 mV) |
| `VREF_MV` | `2600.0` | Effective Vref at chosen attenuation |
| `CAL_OFFSET_MV[]` | `{0, 0}` | Per-channel offset correction (mV) |
| `CAL_GAIN[]` | `{1, 1}` | Per-channel gain correction |
| `SAMPLE_INTERVAL_MS` | `500` | Acquisition period (ms) |
| `OVERSAMPLING_COUNT` | `16` | ADC reads averaged per sample |
| `MOVING_AVG_SIZE` | `8` | Software moving-average window |

Values can also be overridden at build time via `build_flags` in `platformio.ini`
(e.g. `-DSAMPLE_INTERVAL_MS=200`).

---

## Calibrazione e limiti

### Offset e Gain

1. Apply a known voltage (e.g. from a precision reference) to each ADC pin.
2. Read the `voltage_mV` value printed over Serial.
3. Compute: `CAL_OFFSET_MV = voltage_raw_mv - reference_mv`
4. If the slope also deviates, set `CAL_GAIN` to correct it.
5. Write the values back into `config.h` and re-flash.

### Non-linearità ADC ESP32-S3 e Vref variabile

- The ESP32-S3 ADC has an integral non-linearity (INL) of ≈ 1–2% at
  12-bit / `ADC_11db`. The true Vref varies chip to chip (±5 %).
- **Recommendation**: use `esp_adc_cal_characterize()` (available in
  `esp_adc_cal.h`) for hardware-based Vref calibration, or add the
  `ESP32AnalogRead` library for a ready-made wrapper.
- For best accuracy at small voltages consider `ADC_0db` (0–800 mV) or
  `ADC_6db` (0–1350 mV) and scale the Alphasense output with a resistor
  divider.

### Rumore e Drift termico

- Random ADC noise: mitigated by oversampling (`OVERSAMPLING_COUNT = 16`
  gains ~2 ENOB). Increase to 64 or 256 for lower noise at the cost of
  longer acquisition time.
- **RC low-pass filter** on each ADC line (e.g. 10 kΩ + 100 nF, fc ≈ 160 Hz)
  before the GPIO pin greatly reduces RF pick-up and high-frequency noise.
- Temperature drift: both the Alphasense sensor and the ESP32 ADC drift with
  temperature. If high accuracy is needed over temperature, store and apply a
  temperature correction table.
- **Ground loops**: ensure a clean, single-point GND between ESP32, interface
  board, sensor, and power supply.

### Conversione in concentrazione

`convertToConcentration()` in `include/alphasense_b43f.h` is intentionally a
stub (returns 0.0).  To implement it you need:

1. The TIA (transimpedance amplifier) gain on the ISB board (Ω).
2. The sensor sensitivity from the Alphasense calibration certificate (nA/ppb).
3. The zero-current (baseline) voltage at clean air.
4. (Optional) temperature-compensation coefficients.

See Alphasense application notes **AAN 803** and **AAN 110** for details.

---

## Schema di Collegamento

### Block Diagram

```
  3.3 V / 5 V supply
       │
  ┌────┴────────────────────────────────────────┐
  │        Alphasense ISB / Interface Board      │
  │                                              │
  │  [B43F Sensor]──WE──►[TIA / signal cond.]──►│ OUT_CH0 ──► GPIO4  ┐
  │               ──AE──►[TIA / signal cond.]──►│ OUT_CH1 ──► GPIO5  │
  │                                              │                    │
  │  GND                                         │ GND ───────────────┤
  └──────────────────────────────────────────────┘                    │
                                                                       │
  ┌────────────────────────────────────────────────────────────────────┘
  │            ESP32-S3 DevKit
  │
  │  GPIO4  (ADC1_CH3) ← CH0 – Working electrode output
  │  GPIO5  (ADC1_CH4) ← CH1 – Auxiliary electrode output
  │  3V3    ─────────────────── 3.3 V to ISB board (if powered from MCU)
  │  GND    ─────────────────── Common ground
  └─────────────────────────────────────────────
```

> ⚠ **SOSTITUIRE CON PIN REALI** – The GPIO numbers above are placeholders.
> Check your ISB board datasheet and your physical wiring, then update
> `ADC_PINS[]` in `include/config.h`.

### Wiring Table

| Segnale | Da | A | Note |
|---------|-----|---|------|
| OUT_CH0 (WE) | ISB `VOUT1` | ESP32-S3 `GPIO4` ⚠ | Working electrode analogue output |
| OUT_CH1 (AE) | ISB `VOUT2` | ESP32-S3 `GPIO5` ⚠ | Auxiliary electrode analogue output |
| 3.3 V | ESP32-S3 `3V3` | ISB `VCC` | Only if ISB is 3.3 V compatible – check datasheet |
| GND | ESP32-S3 `GND` | ISB `GND` | Common ground – connect at one point |
| (optional) VREF | ISB `VREF` | ESP32-S3 `GPIO_X` ⚠ | If ISB outputs a reference voltage, route to ADC for self-cal |

> ⚠ All GPIO numbers marked with ⚠ are **placeholders**.
> Replace them with the real GPIO numbers from your schematic/PCB.

---

## Verifica (checklist)

- [ ] **Compila** – `pio run -e esp32s3` → exit 0, no errors
- [ ] **Upload** – `pio run -e esp32s3 --target upload` → "Leaving... Hard resetting…"
- [ ] **Monitor** – `pio device monitor` → CSV rows appear at the expected rate
- [ ] **Valori ADC plausibili** – with no sensor connected expect noise near 0;
      with sensor powered expect values in the 100–2000 mV range (check ISB datasheet)
- [ ] **Calibrazione** – apply known voltage, verify `voltage_mV` matches,
      set `CAL_OFFSET_MV` / `CAL_GAIN` if needed