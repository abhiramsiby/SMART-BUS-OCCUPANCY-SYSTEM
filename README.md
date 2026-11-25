# SMART-BUS-OCCUPANCY-SYSTEM

An Arduino-based bidirectional passenger counting system that tracks real-time seat availability inside a bus using two sensors and displays the status on a 16x2 I2C LCD screen.

## Features
- **Bidirectional Detection:** Distinguishes between passengers entering and exiting based on the sequence in which two sensors are triggered.
- **False-Trigger Protection:** Uses a finite state machine with a 4-second timeout window (`TIMEOUT = 4000`) to reset incomplete crossings (e.g., if a passenger steps up to the door and turns back).
- **Real-Time Display:** Shows the exact number of available seats on a 16x2 LCD and switches to a `SEAT FULL` alert when capacity reaches zero.
- **Boundary Guards:** Prevents the seat count from dropping below `0` or exceeding the maximum bus capacity (`5`).

## Hardware Requirements
- Arduino Uno / Nano (or any compatible microcontroller)
- 2x Digital Presence/Proximity Sensors (e.g., IR obstacle sensors or PIR sensors with `HIGH` active output)
- 1x 16x2 LCD Display with I2C Interface Module (Address `0x27`)
- Jumper wires and breadboard

## Pin Configuration
![Arduino Pin and Wiring Diagram](assets/pin_diagram.png)

| Component | Pin / Connection | Arduino Pin | Notes |
| :--- | :--- | :--- | :--- |
| **Outer Sensor (`S1`)** | Signal Out | `D2` | Placed at the outer edge of the doorway |
| **Inner Sensor (`S2`)** | Signal Out | `D4` | Placed at the inner edge of the doorway |
| **I2C LCD** | `SDA` | `A4` (Uno/Nano) | I2C Data line |
| **I2C LCD** | `SCL` | `A5` (Uno/Nano) | I2C Clock line |
| **Power** | `VCC` / `GND` | `5V` / `GND` | Shared across sensors and LCD |

## How It Works
The system relies on a 3-state Finite State Machine (`IDLE`, `ENTRY_UNDERWAY`, `EXIT_UNDERWAY`) using rising-edge detection (`LOW` to `HIGH` transitions):

1. **Passenger Entry (`S1` → `S2`):**
   - Triggering the outer sensor (`S1`) first sets the system to `state = 1` and starts a 4-second timer.
   - Triggering the inner sensor (`S2`) within 4 seconds confirms a complete entry, decrements the available `seats` count, updates the LCD, and resets to `IDLE`.
2. **Passenger Exit (`S2` → `S1`):**
   - Triggering the inner sensor (`S2`) first sets the system to `state = 2` and starts a 4-second timer.
   - Triggering the outer sensor (`S1`) within 4 seconds confirms a complete exit, increments the available `seats` count, updates the LCD, and resets to `IDLE`.
3. **Timeout Reset:**
   - If the second sensor is not triggered within `4000 ms`, the state automatically resets to `IDLE` without altering the seat count.

## System Assumptions & Limitations
For accurate counting, the system operates under the following physical constraints:
- **Single-File Movement:** Passengers must enter or exit one at a time. Two people cannot enter or leave side-by-side simultaneously, as they will register as a single trigger.
- **No Simultaneous Bidirectional Crossing:** While one person is entering the doorway, another person must not attempt to exit at the same time (and vice versa), as overlapping sensor triggers will disrupt the state sequence.
- **Active-HIGH Sensors:** The code assumes both sensors output `HIGH` when an object/person is detected and `LOW` when clear.

## Setup & Installation
1. Install the **`LiquidCrystal_I2C`** library via the Arduino IDE Library Manager.
2. Wire the sensors and I2C LCD according to the pin configuration table above.
3. Adjust `int seats = 5;` in the code if your vehicle has a different seating capacity (remember to also update the upper bound check `if (seats < 5)` in `case 2`).
4. Upload the sketch to your Arduino board.

*(Note: If the LCD backlight turns on but no text appears, verify your I2C module's address—commonly `0x27` or `0x3F`—and adjust the contrast potentiometer on the back of the I2C module.)*
