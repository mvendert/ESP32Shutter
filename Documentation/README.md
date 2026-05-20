# ESP32Shutter — Code Overview

> **Scope of this document.** This README describes the **current state of the firmware** in [ESP32Shutter/](../ESP32Shutter/). For the intended hardware design, wiring diagrams, and electrical requirements, see [SPECIFICATIONS.md](../ESP32Shutter/SPECIFICATIONS.md). Where the two disagree, the spec is the target and this document records the implementation as it stands.
>
> A companion firmware project, [TriggerTester/](../TriggerTester/), is a small standalone sketch used to validate pushbutton wiring independently of the main controller.

## High-Level Introduction

`ESP32Shutter` is a small Arduino/PlatformIO application that turns an ESP32 into a wired camera remote for a Canon EOS R6. The firmware drives two optocoupler channels so the MCU can assert the camera's focus and shutter lines without sharing ground with the camera body.

At a high level, the code is organized as a polling application with three responsibilities:

1. Read two trigger inputs.
2. Translate those input states into focus/shutter output states.
3. Mirror that state to status LEDs and serial telemetry.

The interesting part is not the control logic itself, which is intentionally very small, but the separation of concerns. The project uses tiny interfaces to isolate input handling from output control and then connects them in a coordinator class. For an Arduino project, this is a reasonably disciplined structure: the application logic is not tied directly to `digitalRead()`/`digitalWrite()`, and the hardware-specific code is kept in narrow concrete classes.

## Runtime Architecture

The runtime path is straightforward:

- `setup()` starts serial logging and initializes the application object.
- `loop()` repeatedly calls `ShutterApp::update()`.
- `ShutterApp` samples the two trigger inputs.
- The sampled states are forwarded to the shutter controller.
- The shutter controller drives the optocoupler GPIOs and the indicator LEDs.
- Periodic serial telemetry prints the effective state.

In other words, the project is a synchronous, non-blocking state propagator. There are no timers, interrupts, RTOS tasks, queues, or asynchronous callbacks involved. The effective loop frequency is whatever the ESP32 can sustain while running `update()` continuously, with debounce and telemetry handled through `millis()`.

## Build Context

The project is configured as a standard PlatformIO Arduino target for an ESP32 dev board:

- Platform: `espressif32`
- Board: `esp32dev`
- Framework: `arduino`
- Serial monitor speed: `115200`

That means the code assumes the normal Arduino core APIs are available and makes no use of ESP-IDF-only features.

### Build & Flash

From the `ESP32Shutter/` project folder:

```powershell
platformio run                 # compile
platformio run --target upload # flash the connected ESP32
platformio device monitor      # open the serial monitor at 115200 baud
```

No extra libraries are required beyond the Arduino-ESP32 core pulled in by PlatformIO.

## File-Level Structure

The source tree is split into a few small units:

- `include/Config.h`: central pin and timing constants.
- `include/ITriggerInput.h`: abstraction for a held/not-held input source.
- `include/IShutterController.h`: abstraction for something that can assert focus and shutter outputs.
- `include/DebouncedButton.h` and `src/DebouncedButton.cpp`: GPIO-backed button input with software debounce.
- `include/OptocouplerShutterController.h` and `src/OptocouplerShutterController.cpp`: GPIO-backed focus/shutter output driver plus LED mirroring.
- `include/ShutterApp.h` and `src/ShutterApp.cpp`: top-level application coordinator.
- `include/Ky008LaserEmitter.h` and `src/Ky008LaserEmitter.cpp`: laser emitter helper class prepared for future trigger modes.
- `include/Ky008LaserSensor.h` and `src/Ky008LaserSensor.cpp`: laser sensor helper class prepared for future trigger modes.
- `src/main.cpp`: object composition root and Arduino entry points.

This is effectively a manual dependency-injection layout. `main.cpp` constructs concrete instances and hands them to `ShutterApp` through interface references.

## Main Composition Root

`src/main.cpp` is intentionally thin. It performs object construction at file scope and leaves all behavior to the composed classes.

The important design choice here is that hardware objects are created once and retained for the life of the program:

- One `DebouncedButton` for focus.
- One `DebouncedButton` for shutter.
- One `OptocouplerShutterController` for outputs.
- One `ShutterApp` that orchestrates them.

This keeps `setup()` and `loop()` almost empty:

- `setup()` calls `Serial.begin(115200)` and `app.begin()`.
- `loop()` calls `app.update()`.

That style scales better than embedding the whole control path directly in `loop()`, because changing input or output hardware does not require rewriting the application flow.

## Configuration Layer

`include/Config.h` is the single source of truth for pin assignments and timing constants.

Current assignments:

- Focus optocoupler drive: GPIO 25
- Shutter optocoupler drive: GPIO 26
- Focus LED: GPIO 15
- Shutter LED: GPIO 16
- Focus button: GPIO 18
- Shutter button: GPIO 19
- Laser emitter: GPIO 32
- Laser sensor: GPIO 33
- Debounce interval: 50 ms
- Telemetry interval: 500 ms

For an embedded project, this is the right level of centralization. Nothing in the codebase hardcodes pin numbers outside `Config.h`, so a board revision can usually be handled there.

## Input Abstraction: `ITriggerInput`

`ITriggerInput` is the smallest useful abstraction in the project:

- `begin()` initializes the input source.
- `isHeld()` reports whether the trigger is currently active.

The benefit is not theoretical. The application layer does not care whether the trigger comes from a pushbutton, a beam-break sensor, a software test shim, or a future network command. Anything that can answer `isHeld()` can be dropped into `ShutterApp`.

For experienced Arduino developers, this is the core architectural lever in the project. It is what makes later expansion realistic without collapsing back into a monolithic `loop()`.

## Button Implementation: `DebouncedButton`

`DebouncedButton` is the concrete `ITriggerInput` implementation used today.

It stores:

- The GPIO pin.
- The debounce interval in milliseconds.
- The last raw state read from the pin.
- The last stable debounced state.
- The timestamp of the most recent raw-state transition.

The debounce algorithm is the standard edge-timestamp pattern:

1. Read the raw pin state.
2. If the raw state changed since last sample, update `lastRawState_` and store `millis()`.
3. If the raw state has remained unchanged for at least `debounceMs_`, promote it to `stableState_`.
4. Return `stableState_`.

This implementation is simple and non-blocking. It does not delay the loop, and it does not require interrupts. For low-rate human inputs, that is the correct tradeoff.

### Important Behavioral Note

The current code uses:

- `pinMode(pin_, INPUT)`
- `digitalRead(pin_) == HIGH` as the pressed condition

That means the implementation currently assumes an active-high button signal.

This does **not** match the project specification, which describes the buttons as wired with `INPUT_PULLUP` and pressed = `LOW`. If the hardware is actually wired per the specification, the current firmware will interpret button state incorrectly unless the wiring provides an external pull-down and drives the pin high on press.

So the README should be read as documentation of the current codebase, not as proof that the code fully matches the specification.

## Output Abstraction: `IShutterController`

`IShutterController` defines the output side:

- `begin()` initializes hardware.
- `setFocus(bool active)` drives the focus channel.
- `setShutter(bool active)` drives the shutter channel.

Again, the value is that `ShutterApp` does not know or care whether the outputs are optocouplers, relays, MOSFETs, or even a simulator that just logs state.

## Output Driver: `OptocouplerShutterController`

This class owns four GPIOs:

- focus output pin
- shutter output pin
- focus LED pin
- shutter LED pin

It also caches two booleans:

- `focusActive_`
- `shutterActive_`

Those cached states are important because the class only writes GPIOs and emits serial messages when a state transition actually occurs. That avoids redundant `digitalWrite()` calls and prevents the serial log from filling with the same message every loop iteration.

### `begin()`

`begin()` configures all four pins as outputs, drives them low, and logs an initialization line over serial. This establishes a safe idle state immediately at startup.

### `setFocus()` and `setShutter()`

Each setter follows the same pattern:

1. Compare requested state to cached state.
2. If nothing changed, return without touching hardware.
3. Update the cached state.
4. Write the control GPIO.
5. Write the matching LED GPIO.
6. Print a transition message.

The LED behavior is intentionally coupled to the control pins, so the LED is not an independent subsystem. It is just a direct visual mirror of the logical output state.

## Application Coordinator: `ShutterApp`

`ShutterApp` is where the project's actual behavior lives.

It holds references to:

- the focus input
- the shutter input
- the shutter controller

And it tracks:

- telemetry interval
- last telemetry timestamp
- last observed focus state
- last observed shutter state

### `begin()`

`begin()` initializes both input devices and the shutter controller, then prints `Shutter app ready`.

### `update()`

`update()` is the core control loop:

1. Read focus button held state.
2. Read shutter button held state.
3. Forward focus state to `setFocus()`.
4. Forward shutter state to `setShutter()`.
5. Every `telemetryIntervalMs_`, print a compact status line.

The behavior is purely combinational with state only around debounce and logging. There is no sequencing logic such as:

- forcing focus before shutter
- minimum press times
- trigger latching
- delayed release
- edge-triggered capture pulses

That means the shutter button can assert the shutter output independently of the focus button, exactly as the spec describes for manual-focus or pre-focused use.

### On `lastFocusHeld_` and `lastShutterHeld_`

The class stores `lastFocusHeld_` and `lastShutterHeld_`, but the current implementation does not use those values for behavior. They are written at the end of `update()` and then ignored.

This suggests one of two things:

- the fields were added in anticipation of edge-based logic later, or
- they are leftover state that can be removed without changing current behavior.

For now, they are harmless but redundant.

## Serial Telemetry Model

There are two kinds of serial output in the project:

1. Transition logs from the shutter controller when focus/shutter changes.
2. Periodic summary logs from `ShutterApp` every 500 ms.

That combination is useful during bring-up because it gives both event-level visibility and a heartbeat view of the current state. It is still light enough that it should not materially disturb a simple polling application like this.

## Laser Support Scaffold

`Ky008LaserEmitter` and `Ky008LaserSensor` are not currently wired into `main.cpp` or `ShutterApp`, so they are dormant support code rather than an active feature.

They provide a clean base for the "beam broken triggers shutter" requirement mentioned in the specification:

- `Ky008LaserEmitter` wraps a digital output and tracks whether the emitter is enabled.
- `Ky008LaserSensor` wraps a digital input and exposes three views: raw read, beam detected, and beam broken.

The sensor class is designed with configurable input mode and configurable active state, which is the right move because KY-008 style hobby modules are often paired with different receiver boards and signal conventions.

The important architectural point is that this laser code is still below the application layer. To make it useful, the project still needs an input adapter that implements `ITriggerInput` or a richer application state machine that can combine manual and optical trigger sources.

## Control Semantics

As implemented today, the firmware semantics are:

- Focus button held -> focus output asserted.
- Focus button released -> focus output released.
- Shutter button held -> shutter output asserted.
- Shutter button released -> shutter output released.
- Focus and shutter may be active simultaneously.
- Idle state is all outputs low.

This maps closely to the electrical behavior of a simple two-button remote release.

## Design Assessment

For a small Arduino application, the current design is solid in a few specific ways:

- The main loop stays minimal.
- Hardware access is pushed into concrete driver classes.
- Application logic depends on interfaces, not pin APIs.
- Debounce is non-blocking.
- Outputs are edge-filtered to avoid redundant writes and logs.
- Pin/timing constants are centralized.

The code is still intentionally simple, and that simplicity is a strength here. There is no unnecessary abstraction beyond the two interfaces that actually buy flexibility.

## Gaps and Next Logical Steps

An experienced Arduino developer will notice a few likely next steps immediately:

1. **Bug / spec mismatch:** align the button implementation with the documented hardware by switching to `INPUT_PULLUP` and active-low handling — the spec ([SPECIFICATIONS.md §5.4–5.5](../ESP32Shutter/SPECIFICATIONS.md)) wires the buttons to GND and expects a press to read LOW.
2. Remove or actually use `lastFocusHeld_` and `lastShutterHeld_` to keep `ShutterApp` tight.
3. Decide whether future laser triggering should appear as another `ITriggerInput` (cleanest) or whether the app needs an explicit state machine for delayed capture logic.
4. Add timing-based shutter pulse generation if the optical trigger should fire a controlled pulse rather than track a held state.
5. Consider a small hardware abstraction test harness so application behavior can be verified without hardware — the existing interfaces already make this straightforward.

##Links
https://www.waveshare.com/wiki/Laser_Sensor
https://www.youtube.com/watch?v=nkIkBK2J19g
receiver- RJT474
https://www.youtube.com/watch?v=uuRNfvj5Sz8

The right way:
To get full brightness safely, power the laser module using the 5V (VIN or 5V) pin on your ESP32 board, and use a simple NPN transistor (like a 2N2222) as a switch to control it via a 3.3V GPIO pin

Wiring Setup:
1.Transistor Base: Connect to an ESP32 GPIO pin (e.g., GPIO 23) through a 1kΩ resistor.
2.Transistor Collector: Connect to the Ground (-) pin of the KY-008 module.
3.Transistor Emitter: Connect to the GND pin of the ESP32.
4.KY-008 S (Signal) Pin: Connect to the 5V / VIN pin of the ESP32 (which outputs 5V when plugged into USB).
5.KY-008 Center Pin: Leave disconnected.

If you do not have a transistor, you can use a logic-level shifter to cleanly convert the 3.3V ESP32 signal up to 5V to drive the laser module.


Transistor Switching Method (Recommended)This method acts like an electronic switch. The ESP32's 3.3V pin safely handles a tiny signal current to turn on the transistor, which then securely handles the 5V power loop for the laser module.        +5V (VIN / 5V) --------------------------> [ S ] 
                                                                [   ]  KY-008 Laser
                                                                [ - ]
                                                                  |
                                                                  |
                                                             [Collector]
                                                                  |
  ESP32 GPIO (e.g., 23) --[ 1kΩ Resistor ]---- [Base]     2N2222 NPN Transistor
                                                     |
                                                 [Emitter]
                                                     |
        GND -----------------------------------------+----------------> [GND]
Note on Bipolar Transistors (like 2N2222): Looking at the flat side of the transistor with the pins pointing down, the pinout from left to right is Emitter (1), Base (2), Collector (3).

## Summary

`ESP32Shutter` is a compact, interface-driven Arduino application for controlling isolated focus and shutter outputs from an ESP32. The present codebase already has a reasonable separation between input sampling, application policy, and output driving, which makes it a good base for the future laser-trigger and delayed-release features mentioned in the project specification.

The main implementation caveat is that the current button polarity/configuration in code appears inconsistent with the written hardware spec. Aside from that, the architecture is clean, readable, and well positioned for incremental growth.