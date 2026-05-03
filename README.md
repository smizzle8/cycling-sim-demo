# Cycling Sim Demo

A tiny self-contained cycling simulation. Open in Chrome, see a bike move.

## Files

- **`bike.html`** — the visual demo. Open in Chrome/Edge.
- **`bike.ino`** — Arduino firmware (single sketch, both pedal + steering).

## Try it RIGHT NOW (no hardware needed)

1. Double-click `bike.html` to open in Chrome or Edge.
2. Click **"Demo mode (keyboard)"**.
3. Use arrow keys:
   - **↑** hold to pedal (bike accelerates)
   - **← / →** steer left/right
   - **Space** brake
4. Watch the bike move on the road.

That's it — instant visual demo for your friend.

## Connect the real Arduino (later)

### 1. Wire it up

| Component | Pin |
|---|---|
| Hall sensor `VCC` | Arduino `5V` |
| Hall sensor `GND` | Arduino `GND` |
| Hall sensor `OUT` | Arduino `D2` (interrupt-capable) |
| Potentiometer outer 1 | Arduino `5V` |
| Potentiometer wiper | Arduino `A0` |
| Potentiometer outer 2 | Arduino `GND` |

### 2. Flash the firmware

1. Open `bike.ino` in Arduino IDE.
2. Select board: Arduino Uno (or Nano, both work).
3. Select port (Tools → Port → the one that says Arduino).
4. Click Upload.

### 3. Connect from the browser

1. Open `bike.html` in Chrome or Edge (Web Serial only works there).
2. Click **"Connect to Arduino"**.
3. Pick the Arduino port from the popup.
4. Pedal — bike moves. Turn handlebar — bike steers.

## How it works

- Arduino reads the Hall sensor pulses (1 pulse per pedal revolution) and computes RPM over a 1-second rolling window.
- Arduino reads the potentiometer (0-1023) and normalizes to -1.0 (full left) to +1.0 (full right).
- Arduino sends `RPM,ANGLE` over USB serial at 20 Hz (e.g. `"62.30,-0.42\n"`).
- The HTML page parses each line and updates the bike's speed (from RPM) and heading (from angle).
- A bird's-eye-view canvas shows the bike riding down a road, with steering and speed reflected live.

## Customizing

- **RPM-to-speed scaling**: edit `bike.ino` line where `currentRPM = (pulses / seconds) * 60.0;` if your wheel has multiple magnets per revolution. Or edit `bike.html`'s `updatePhysics` function (search for `targetSpeedMs`).
- **Potentiometer calibration**: if your full-left isn't 0 and full-right isn't 1023, edit `POT_MIN` and `POT_MAX` constants in `bike.ino`.
- **Visual style**: all colors are CSS variables at the top of `bike.html` (search `:root`).
