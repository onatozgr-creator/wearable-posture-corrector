# Wearable Posture Corrector
*An embedded hardware prototype for real-time spinal tilt tracking and haptic feedback.*

---

## 📌 Project Overview
This project is an embedded hardware prototype I developed to actively monitor upper body posture and provide immediate physical (haptic) feedback when slouching occurs. 

Rather than relying on continuous smartphone connectivity, cloud synchronization, or background apps, I wanted to build a fully self-contained, battery-powered wearable device that handles sensing, processing, power regulation, and actuation directly on hardware.

---

## 🛠️ System Architecture & Hardware Design

The hardware is designed around four main sub-circuits: sensing, processing, actuation, and power management.

```text
[ 3.7V Li-Ion Battery ] 
          │
          ▼
[ TP4056 Charging Module (Type-C) ]
          │
          ▼
[ MT3608 Boost Converter (Boost to 5.0V) ]
          │
    ┌─────┴────────────────────────┐
    ▼                              ▼
[ Arduino Nano (ATmega328P) ]    [ MPU6050 IMU ]
    │                              (I2C: A4/A5)
    ▼
[ Transistor Driver Circuit (NPN + Diode) ]
    │
    ▼
[ Coin Vibration Motor ]
```

### 1. Power Regulation & Battery Management
* **Battery & Charging:** Powered by a 3.7V single-cell Lithium-ion battery. Charging is handled by a TP4056 module with integrated over-discharge and short-circuit protection, accessible via a modern USB Type-C port.
* **Step-Up Conversion:** Since the Arduino Nano and peripherals operate reliably at 5V logic, an MT3608 DC-DC boost converter was integrated to step up the variable battery voltage (3.2V to 4.2V) to a stable 5.0V system rail.

### 2. Sensing & Microcontroller
* **Sensor (MPU6050):** Utilizes the 3-axis accelerometer registers of the MPU6050 communicated over hardware I2C (A4 as SDA, A5 as SCL).
* **Processing Unit:** An ATmega328P-based Arduino Nano executes the tilt estimation algorithm, runs the timing state machine, and controls the output driver.

### 3. Haptic Actuation Subsystem
* To drive the vibration motor safely without drawing excessive current through the microcontroller I/O pins, a low-side switching circuit was implemented:
  * **Switch:** 2N2222 NPN transistor driven from digital pin D3 through a 1 kOhm base resistor.
  * **Protection:** A 1N4148 flyback diode placed anti-parallel across the motor terminals to clamp inductive voltage spikes generated when switching the motor off.

---

## 🧠 Firmware & Algorithm Implementation

The embedded C++ firmware running on the microcontroller performs three primary tasks: calibration, angle estimation, and debounced threshold verification.

### 1. Baseline Calibration Routine
To accommodate different wearing angles or body types, the device calibrates on every power cycle:
* During the first 2 seconds after boot, the device samples initial accelerometer data while the user remains in an upright posture.
* The calculated pitch angle is saved as the reference baseline (Pitch_0).

### 2. Pitch Angle Calculation
The firmware reads raw tri-axis accelerometer values (Ax, Ay, Az) and computes the inclination angle using standard trigonometric projection:

```text
Pitch = atan2(-Ax, sqrt(Ay^2 + Az^2)) * (180.0 / PI)  
```

### 3. Debounce & State Filtering
A common issue in posture tracking is false triggering caused by normal, transient movements (e.g., reaching for an item, taking a sip of water, or adjusting sitting position).
* **Threshold Detection:** If the current pitch angle deviates from the baseline by more than a set threshold (e.g., tilt angle >= 15 degrees), an internal timer increments.
* **Debounce Window:** The vibration motor triggers only if the deviation is sustained continuously for more than **2.0 seconds**.
* **Instant Recovery:** As soon as the user straightens up (tilt angle < 15 degrees), the timer resets and the motor turns off immediately.

---

## 🔍 Engineering Decisions & Challenges

* **Handling Transients vs. Responsiveness:** Determining the balance between immediate correction and avoiding false positives. A software timer-based debounce filter (2.0-second delay) eliminated false alarms from natural desk movements while maintaining high responsiveness.
* **Power Rail Stability:** Using a boost converter directly alongside an inductive load (vibration motor) introduced voltage fluctuations on the 5V line. Decoupling capacitors and flyback clamping were necessary to maintain clean supply voltages to the MPU6050 and MCU.
* **Sensor Placement & Mechanical Factors:** Testing showed that mounting the sensor on loose fabric introduced error due to cloth shifting. The prototype yields the most consistent angular measurements when clipped firmly to a collar or worn on a chest/shoulder strap.

---

## 📁 Repository Contents

```text
├── firmware/
│   └── posture_detector.ino    # Arduino C++ source code (IMU read & state logic)
├── schematics/
│   ├── circuit_diagram.png     # Full schematic and wiring diagram
│   └── power_rail.png          # Power stage & boost converter layout
├── photos/
│   └── prototype_build.jpg     # Assembled bench prototype photo
└── README.md                   # Project overview & documentation
```
