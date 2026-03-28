# ESP32 Shutter Release Control Unit — Project Specification

**Target camera:** Canon EOS R6  
**Target MCU board:** ESP32-WROOM-32 DevKit  
**Date:** 2026-03-15  

---

## 1. Project Overview

This project implements a wired remote shutter release controller for the Canon EOS R6 camera using an ESP32-WROOM-32 development board. Two 4N36 optocouplers provide galvanic isolation between the ESP32 and the camera's remote terminal, replacing the mechanical switches found in a traditional RS-60E3 remote. Two independent pushbuttons — **Focus** and **Shutter Release** — give the user direct, real-time control over the camera's half-press and full-press functions, mimicking the feel of the original RS-60E3 cable release.

---

## 2. Canon R6 Remote Terminal (E3 / RS-60E3)

The Canon EOS R6 exposes a **2.5 mm stereo (TRS) "Remote" jack** — Canon's proprietary "E3" terminal — on its side.

| TRS Contact | Signal   | Trigger action                          |
|-------------|----------|-----------------------------------------|
| Sleeve      | GND      | Camera circuit ground reference         |
| Ring        | Focus    | Short to GND → initiates autofocus (half-press) |
| Tip         | Shutter  | Short to GND → releases shutter (full-press)    |

> **Important:** The camera supplies approximately **3.3 V / 0.5 mA** on the Focus and Shutter lines. Do **not** connect ESP32 GPIO pins directly to these lines — use the optocoupler isolation circuit described below.

---

## 3. Hardware Components

| Component | Quantity | Notes |
|-----------|----------|-------|
| ESP32-WROOM-32 DevKit | 1 | 3.3 V GPIO logic |
| 4N36 optocoupler | 2 | One for Focus, one for Shutter |
| 220 Ω resistor | 4 | 2× optocoupler input-side current limiting (see §5.1), 2× LED current limiting (see §5.6) |
| LED (standard, e.g. 3 mm) | 2 | 1× Focus status, 1× Shutter status |
| 2.5 mm TRS plug / cable | 1 | Wired to camera remote jack |
| Momentary pushbutton | 2 | One for Focus, one for Shutter Release |
| Assorted resistors | — | Pull-downs / pull-ups as needed |
| Breadboard / PCB, wires | — | |

---

## 4. GPIO Pin Assignments

> The previously used GPIO 30 and GPIO 31 **do not exist** on the ESP32-WROOM-32; they are not broken out on the module. The table below uses valid, output-capable pins.

| GPIO | Direction | Function |
|------|-----------|----------|
| **GPIO 25** | Digital Output | Focus control — drives 4N36 #1 LED anode (via 220 Ω) |
| **GPIO 26** | Digital Output | Shutter control — drives 4N36 #2 LED anode (via 220 Ω) |
| **GPIO 15** | Digital Output | Focus status LED — indicates Focus line is active |
| **GPIO 16** | Digital Output | Shutter status LED — indicates Shutter line is active |
| **GPIO 18** | Digital Input (INPUT_PULLUP) | Focus pushbutton (active LOW) |
| **GPIO 19** | Digital Input (INPUT_PULLUP) | Shutter Release pushbutton (active LOW) |

---

## 5. Circuit Design

### 5.1 Current-Limiting Resistor Calculation (4N36 Input Side)

The 4N36 LED forward voltage is **V_F ≈ 1.2 V**. Driving from a 3.3 V GPIO:

$$R = \frac{V_{GPIO} - V_F}{I_F} = \frac{3.3\,\text{V} - 1.2\,\text{V}}{10\,\text{mA}} = 210\,\Omega$$

Use the next standard value: **220 Ω**. This gives I_F ≈ 9.5 mA, well within the 4N36's 60 mA absolute maximum and sufficient for reliable switching (CTR ≥ 20 % at 10 mA → I_C ≥ 2 mA, which comfortably sinks the camera's 0.5 mA).

### 5.2 Wiring — Focus Channel (4N36 #1)

```
ESP32 GPIO 25 ──[220 Ω]──► Anode  (pin 1)  ┐
                                4N36 #1      │ Input (LED) side
ESP32 GND      ───────────── Cathode (pin 2) ┘

Camera Ring (Focus) ──── Collector (pin 5) ┐
                                4N36 #1     │ Output (transistor) side
Camera Sleeve (GND) ──── Emitter   (pin 4) ┘
```

When GPIO 25 is driven **HIGH**, the LED turns on, the phototransistor conducts, and the camera's Focus line is shorted to camera GND → autofocus is initiated.

### 5.3 Wiring — Shutter Channel (4N36 #2)

Identical circuit using **GPIO 26** and **4N36 #2**, but connecting the phototransistor to the camera's **Tip (Shutter)** contact:

```
ESP32 GPIO 26 ──[220 Ω]──► Anode  (pin 1) ┐
                                4N36 #2     │ Input (LED) side
ESP32 GND      ───────────── Cathode (pin 2)┘

Camera Tip (Shutter) ─── Collector (pin 5) ┐
                                4N36 #2     │ Output (transistor) side
Camera Sleeve (GND) ──── Emitter   (pin 4) ┘
```

When GPIO 26 is driven **HIGH**, the shutter is released.

> **Ground note:** The ESP32 GND and the camera's Sleeve (GND) are **not** connected together. The optocoupler provides full galvanic isolation. The camera circuit is self-contained on the output/transistor side.

### 5.6 Status LEDs

Two LEDs provide visual feedback showing when the Focus and Shutter lines are active.

#### 5.6.1 Focus Status LED (GPIO 15)

```
ESP32 GPIO 15 ──► Anode  ┐
                   LED    │
                  Cathode ┘──[220 Ω]── GND
```

When GPIO 15 is driven **HIGH**, the Focus status LED lights up, indicating the Focus optocoupler is active (autofocus engaged).

#### 5.6.2 Shutter Status LED (GPIO 16)

```
ESP32 GPIO 16 ──► Anode  ┐
                   LED    │
                  Cathode ┘──[220 Ω]── GND
```

When GPIO 16 is driven **HIGH**, the Shutter status LED lights up, indicating the Shutter optocoupler is active (shutter released).

### 5.4 Focus Pushbutton

Connect one terminal of the Focus pushbutton to **GPIO 18**, the other to **GND**. The firmware enables the internal pull-up (`INPUT_PULLUP`). A button press reads as LOW.

### 5.5 Shutter Release Pushbutton

Connect one terminal of the Shutter Release pushbutton to **GPIO 19**, the other to **GND**. The firmware enables the internal pull-up (`INPUT_PULLUP`). A button press reads as LOW.

---

## 6. Functional Requirements

### 6.1 Two-Button Operation

The controller has two independent momentary pushbuttons — **Focus** and **Shutter Release** — that directly control the corresponding optocoupler channels. The user controls timing by how long each button is physically held.

#### 6.1.1 Focus Button (GPIO 18)

| Condition | Action |
|-----------|--------|
| Focus button pressed and held | Assert Focus (GPIO 25 HIGH) — camera initiates autofocus |
| Focus button released | De-assert Focus (GPIO 25 LOW) |

The Focus line remains active for exactly as long as the user holds the button.

#### 6.1.2 Shutter Release Button (GPIO 19)

| Condition | Action |
|-----------|--------|
| Shutter Release button pressed and held | Assert Shutter (GPIO 26 HIGH) — camera releases shutter |
| Shutter Release button released | De-assert Shutter (GPIO 26 LOW) |

The Shutter line remains active for exactly as long as the user holds the button.

#### 6.1.3 Combined Operation (Focus + Shutter)

The two buttons operate **independently and simultaneously**. The typical shooting workflow is:

| Step | User action | Result |
|------|-------------|--------|
| 1 | Press and hold Focus button | Autofocus engages (half-press) |
| 2 | While still holding Focus, press Shutter Release button | Shutter fires (full-press) with focus locked |
| 3 | Release Shutter Release button | Shutter line de-asserts |
| 4 | Release Focus button | Focus line de-asserts |

> **Note:** If the Shutter Release button is pressed **without** the Focus button, only the shutter fires (no autofocus). This is useful when the camera is set to manual focus or when focus has already been acquired.

### 6.2 Status LED Indication

The Focus and Shutter status LEDs shall mirror the state of their respective optocoupler output pins:

| Condition | Focus LED (GPIO 15) | Shutter LED (GPIO 16) |
|-----------|---------------------|------------------------|
| Focus active (GPIO 25 HIGH) | ON | — |
| Focus inactive (GPIO 25 LOW) | OFF | — |
| Shutter active (GPIO 26 HIGH) | — | ON |
| Shutter inactive (GPIO 26 LOW) | — | OFF |

The LEDs are driven in the same `update()` cycle as the optocoupler outputs, ensuring they always reflect the current state with no perceptible delay.

### 6.3 Button Debouncing

Both buttons shall be software-debounced with a minimum period of **50 ms** to prevent false triggers from contact bounce.

### 6.4 Idle State

In the idle state GPIO 25, GPIO 26, GPIO 15, and GPIO 16 shall all be **LOW** (optocouplers off, LEDs off, camera undisturbed).

### 6.5 Serial Debug Output

During development, the firmware shall print pin states and timing values to the UART at **115200 baud** via `Serial.println()`.

---

## 7. Software Architecture (for the junior developer)

```
setup()
  ├─ Serial.begin(115200)
  ├─ pinMode(FOCUS_PIN,          OUTPUT)     → LOW
  ├─ pinMode(SHUTTER_PIN,       OUTPUT)     → LOW
  ├─ pinMode(FOCUS_LED_PIN,     OUTPUT)     → LOW
  ├─ pinMode(SHUTTER_LED_PIN,   OUTPUT)     → LOW
  ├─ pinMode(FOCUS_BUTTON_PIN,  INPUT_PULLUP)
  └─ pinMode(SHUTTER_BUTTON_PIN, INPUT_PULLUP)

loop()
  ├─ read Focus button (debounced)
  │   ├─ held  → digitalWrite(FOCUS_PIN,   HIGH), digitalWrite(FOCUS_LED_PIN,   HIGH)
  │   └─ released → digitalWrite(FOCUS_PIN, LOW),  digitalWrite(FOCUS_LED_PIN,   LOW)
  ├─ read Shutter Release button (debounced)
  │   ├─ held  → digitalWrite(SHUTTER_PIN,   HIGH), digitalWrite(SHUTTER_LED_PIN,   HIGH)
  │   └─ released → digitalWrite(SHUTTER_PIN, LOW),  digitalWrite(SHUTTER_LED_PIN,   LOW)
  └─ serial debug output (periodic)
```

---

## 8. Pin Summary (quick reference card)

```
ESP32-WROOM-32
┌──────────────────────────┐
│  GPIO 25 ──[220Ω]── 4N36 #1 (Focus)   → Camera Ring
│  GPIO 26 ──[220Ω]── 4N36 #2 (Shutter) → Camera Tip
│  GPIO 15 ──► Focus LED    ──[220Ω]── GND
│  GPIO 16 ──► Shutter LED  ──[220Ω]── GND
│  GPIO 18 ── Focus pushbutton ── GND
│  GPIO 19 ── Shutter Release pushbutton ── GND
│  GND     ── Optocoupler cathodes, Button GND, LED resistors
│  3.3V    ── (unused)
└──────────────────────────┘
Camera 2.5 mm TRS plug
  Sleeve (GND) ── 4N36 #1 emitter, 4N36 #2 emitter
  Ring   (Focus)  ── 4N36 #1 collector
  Tip    (Shutter) ── 4N36 #2 collector
```

---

## 9. Open Questions / Clarifications Needed

1. **Intervalometer mode:** Should the device support automatic repeated shooting at fixed intervals? If yes, what range (seconds/minutes)?
2. **Bulb mode:** Is long-exposure / bulb mode (hold shutter open for a programmable duration) required?
3. **Display/feedback:** Is any visual feedback (LED, OLED display) required beyond Serial debug output?
4. **Power supply:** Will the ESP32 be powered via USB or a dedicated battery pack?
5. ~~**Focus-only mode:** Should the Focus channel be triggerable independently (without shutter release)?~~ **Resolved** — Focus and Shutter are now independent buttons.

## Additional Information
Canon’s published still-photo burst limits for the EOS R6 are:

1. About 12 fps with the mechanical shutter
2. About 20 fps with the electronic shutter

That translates to:

1. 12 fps -> about 83 ms between frames
2. 20 fps -> about 50 ms between frames

What that means for your ESP32 shutter trigger:

If the camera is in a continuous drive mode, the best spec-based approach is to hold the shutter contact closed and let the camera free-run.
A faster optocoupler or faster GPIO toggling will not make the R6 shoot faster than its own rated burst speed.
Canon does not appear to publish a minimum E3 remote-contact pulse width or a maximum remote pulse repetition rate, so there is no Canon spec saying "X ms pulse guarantees one shot."
Practical conclusion:

For repeated shots using the E3 remote input, design around 12 Hz max mechanical or 20 Hz max electronic.
If you want one pulse per frame, do not schedule rising edges faster than 83 ms apart for mechanical or 50 ms apart for electronic.
If you want the highest reliable burst rate, use continuous drive mode and keep the shutter optocoupler on for the duration of the burst.
About the optocoupler itself:

A 4N36-class phototransistor optocoupler is electrically fast enough for 12 to 20 fps by a wide margin.
Your spec already assumes only about 0.5 mA sink current on the camera side, so the device is not current-limited for this job.
If you really meant 4N46 instead of 4N36, I would verify that exact part’s datasheet before finalizing hardware, but the camera is still the dominant limit here.