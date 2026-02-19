<div align="center">

# Object Detection & Avoidance Robot

### _First Prize Winner — Aghaaz '24 Tech Fest_

<img src="./assets/RobotCoffeeLake.jpg" alt="Robot at AGHAAZ'24" width="400px" height="300px">

_An autonomous obstacle-avoiding robot built on a NodeMCU (ESP8266), powered by an ultrasonic sensor and a servo motor — programmed in C++ using the Arduino framework. This robot won **First Prize** at the semi-annual tech fest **AGHAAZ '24**, held in October 2024._

</div>

---

## 📖 Table of Contents

- [About The Project](#-about-the-project)
- [The Hardware](#-the-hardware)
- [Pin Configuration](#-pin-configuration)
- [How The Code Works](#-how-the-code-works)
- [Avoidance Logic — The Thought Process](#-avoidance-logic--the-thought-process)
- [Motor Control](#-motor-control)
- [Functions Breakdown](#-functions-breakdown)
- [What We Deliberately Left Out](#-what-we-deliberately-left-out)
- [Getting Started](#-getting-started)
- [Team](#-team)
- [A Small Journal](#-a-small-journal)

---

## 🎯 About The Project

This project was built for the robot-making competition at **AGHAAZ '24**, our college's semi-annual tech fest. The challenge was simple in concept but tricky in execution — build a robot that can independently navigate a space, detect objects in its path, and figure out a way around them without any human input.

We built an autonomous ground robot that:

- Continuously measures the distance to whatever is in front of it using an ultrasonic sensor.
- Stops the moment something gets too close.
- Physically rotates its sensor using a servo motor to "look right" and evaluate whether that direction is clear.
- Decides on the spot — turn right if it's clear, turn left if it isn't.
- Resumes forward movement once the coast is clear.

The entire logic runs in a tight loop, making decisions in real time with no external input whatsoever. It's a self-contained, reactive system.

---

## 🔧 The Hardware

The robot uses the following components:

| Component                     | Role                                             |
| ----------------------------- | ------------------------------------------------ |
| **NodeMCU (ESP8266)**         | The brain — runs all the code                    |
| **L298N Motor Driver**        | Controls both DC motors                          |
| **2x DC Gear Motors**         | Drive the rear wheels                            |
| **HC-SR04 Ultrasonic Sensor** | Detects distance to obstacles                    |
| **SG90 Servo Motor**          | Rotates the ultrasonic sensor to scan directions |
| **2-Wheel Drive Chassis**     | The body of the robot                            |
| **Caster Wheel**              | Provides balance at the front                    |
| **Li-ion Battery Pack**       | Powers the entire system                         |

---

## 📌 Pin Configuration

The NodeMCU uses GPIO numbers internally, but they map to the labeled D-pins on the board. Here's how everything is wired:

| Pin Label (NodeMCU) | GPIO Number | Connected To        |
| ------------------- | ----------- | ------------------- |
| D8                  | GPIO 15     | Motor Driver IN1    |
| D7                  | GPIO 13     | Motor Driver IN2    |
| D4                  | GPIO 2      | Motor Driver IN3    |
| D3                  | GPIO 0      | Motor Driver IN4    |
| D1                  | GPIO 5      | HC-SR04 Trigger Pin |
| D2                  | GPIO 4      | HC-SR04 Echo Pin    |
| D0                  | GPIO 16     | Servo Motor Signal  |

IN1 & IN2 control the left motor. IN3 & IN4 control the right motor. Sending a HIGH to one and LOW to the other in each pair determines the direction that motor spins.

---

## 💡 How The Code Works

### Startup

When the robot powers on, `setup()` runs once. It initialises the serial monitor (useful for debugging), sets all motor pins as outputs, assigns the echo pin as an input, attaches the servo to its pin, and moves the servo to its resting position at 60 degrees. There's a 2-second delay here to let the servo settle before the robot starts moving.

### The Main Loop

Everything after that happens inside `loop()`, which runs continuously, over and over, as long as the robot is on.

Every single iteration of the loop does the following:

**Step 1 — Fire the ultrasonic sensor.**
The code sends a 10-microsecond HIGH pulse to the Trigger pin. This causes the HC-SR04 to emit a burst of ultrasonic sound. It then listens on the Echo pin for the pulse that bounces back.

**Step 2 — Calculate distance.**
The time it takes for that echo to return is captured using `pulseIn()`. Since sound travels at roughly 343 m/s, and the sound has to travel to the object and back, the formula to get distance in centimetres is:

```
distance = (duration / 2) / 29.1
```

Dividing duration by 2 accounts for the round trip. Dividing by 29.1 converts microseconds of travel time into centimetres.

**Step 3 — Decide what to do.**

- If `distance > 10 cm` → nothing is blocking the path, keep moving forward.
- If `distance ≤ 10 cm` → something is too close. Time to avoid it.

---

## 🧠 Avoidance Logic — The Thought Process

When the robot detects an obstacle within 10 centimetres, it executes the following sequence:

**1. Stop immediately.**
Before doing anything else, all motors are cut. The robot comes to a halt.

**2. Look right.**
The servo rotates to 180 degrees, swinging the ultrasonic sensor to the right. After a short wait (to let the servo physically reach that angle), the code fires the sensor again and reads the distance in that direction using `getDistance()`. The servo then returns to center.

**3. Is the right side clear?**
If the distance measured on the right is greater than 15 cm, there's enough space to turn. The robot turns right and holds that turn for 1 second.

If the right side is also blocked (≤ 15 cm), the robot turns left instead — no questions asked, no further scanning. This is a deliberate design decision explained below.

**4. Stop briefly, then check forward.**
After the turn, the robot pauses for half a second to stabilise, then measures the distance directly ahead once more. If there's more than 5 cm of clearance, it moves forward. If not, it stops and waits for the next loop iteration to reassess.

This entire process — sense, stop, scan, decide, turn, verify, resume — happens in about 3 seconds and then the main loop takes over again.

---

## ⚙️ Motor Control

The L298N motor driver takes 4 input signals — IN1, IN2 for the left motor and IN3, IN4 for the right motor. By setting these HIGH or LOW in specific combinations, you get different movements:

| Movement       | IN1  | IN2  | IN3  | IN4  |
| -------------- | ---- | ---- | ---- | ---- |
| **Forward**    | LOW  | HIGH | LOW  | HIGH |
| **Turn Right** | HIGH | LOW  | LOW  | HIGH |
| **Turn Left**  | LOW  | HIGH | HIGH | LOW  |
| **Stop**       | LOW  | LOW  | LOW  | LOW  |

Turning is achieved by spinning one motor forward while reversing the other — the robot pivots in place. This is called a differential drive or tank-style steering, and it's very effective for compact, two-wheeled robots.

---

## 🗂️ Functions Breakdown

### `getDistance()`

A reusable helper that fires the ultrasonic sensor and returns the measured distance in centimetres. It's called during avoidance to check the right side and then again to verify forward clearance after turning. Keeping this in its own function avoids repeating the same 6 lines of sensor code every time a distance read is needed.

### `lookRight()`

Moves the servo to 180 degrees, waits 1.5 seconds for it to reach position, then returns it to 60 degrees. The delays here are important — without them, you'd try to read the sensor before the servo has physically moved, giving you a wrong reading from the old direction.

### `Forward()`, `TurnRight()`, `TurnLeft()`, `Stop()`

Each of these simply sets the four motor driver pins to the appropriate HIGH/LOW pattern for that movement. They're short, clean, and named clearly so the logic in `loop()` reads almost like plain English.

---

## 🚫 What We Deliberately Left Out

Reading through the code, you'll notice two functions commented out — `lookLeft()` and `moveBackward()`. These weren't forgotten; they were considered and ruled out.

**Why no `lookLeft()`?**
The avoidance logic checks only the right side first. If the right is blocked, it turns left by default without scanning left first. The reasoning: scanning both sides takes more time, adds complexity, and in a competition environment with a dynamic obstacle course, the simpler approach of "right first, left as fallback" was fast enough and reliable enough to win. Adding a left scan would've introduced more delay and more edge cases to handle.

**Why no `moveBackward()`?**
The competition didn't present scenarios where the robot needed to reverse. In an open obstacle course, if you can't go right and you can't go left, backing up and trying a different angle is rarely more productive than just turning. It also introduces the risk of hitting something behind the robot, which the sensor (mounted at the front) wouldn't detect. Leaving it out kept the system simpler and safer.

---

## 🚀 Getting Started

### What You Need

- Arduino IDE (1.8.x or 2.x)
- NodeMCU (ESP8266) board package installed in Arduino IDE
- The `Servo.h` library (comes bundled with the ESP8266 board package)
- The hardware listed in [The Hardware](#-the-hardware) section

### Board Setup in Arduino IDE

1. Open Arduino IDE → File → Preferences
2. Add this URL to "Additional Board Manager URLs":
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
3. Go to Tools → Board → Board Manager, search for `esp8266`, and install it.
4. Select **NodeMCU 1.0 (ESP-12E Module)** under Tools → Board.

### Uploading the Code

1. Clone this repository:
   ```
   git clone https://github.com/anirxddh/robo-navigator.git
   ```
2. Open `FinalCode.ino` in Arduino IDE.
3. Connect your NodeMCU via USB.
4. Select the correct COM port under Tools → Port.
5. Hit Upload.

### Monitoring

Open the Serial Monitor (Tools → Serial Monitor) at **9600 baud** while the robot runs. Every action — distance readings, obstacle detection, turns, direction decisions — is printed in real time. This was invaluable during testing and debugging.

---

## 👾 Team

**Team Coffee Lake** — BTECH, Batch of 2024

| Name              | Roll Number    |
| ----------------- | -------------- |
| Aniruddha Dey     | BTECH/60115/24 |
| Yash Raj Vardhan  | BTECH/60129/24 |
| Savyasachi Sawarn | BTECH/60138/24 |

---

## 📄 License

This project is open source and available under the [MIT License](LICENSE).

Feel free to use it, build on it, learn from it. If it helps you win your own competition, that'd be a nice full circle.

---

## 🏆 A Small Journal

The competition was part of **Aghaaz '24**, our college's tech fest in October 2024. When we signed up, we honestly weren't sure how polished the final robot would be; we had the hardware, we had the idea, and we had a deadline. Ended up naming the team "Team CoffeeLake", this was Yash's idea of naming up. I was pretty curious why he named it and then he told me that this was the name of the Intel's one of the most successful CPU architecture.

The first few hours were mostly wiring chaos. The motor driver got its input pins mixed up more times than I'd like to admit. Getting the servo and the ultrasonic sensor to cooperate, where the servo is at the right angle _before_ the sensor fires — took embarrassingly long to get right. 

Savyasachi, came in clutch as he has been tinkering with electronics since his teenage times to make sily projects. He fixed what I had messed up with 11 minutes straightup by just randomly changing some pins and checking the pin diagram to and fro.

The delays that look almost too simple in the final code were actually the result of a lot of trial and error.

The turning logic went through a few iterations too. Yash, briefly considered scanning both left and right before deciding, but the added complexity wasn't worth it. The "check right, fallback to left" approach was cleaner and faster, and under competition conditions, that matters.

We decided to keep the serial monitor message as "Coffee detected, executing avoidance maneuvers." considering the name of the team.

When the robot ran its first clean course without bumping into anything, it was genuinely one of those moments. The first day was over before it got event started, thankfully, every team got a 2nd day to finally showcase what they were doing after fixing their mistakes. Me and Yash decided to change the codes last minutes and then endeded up winning.

When we won first place, it felt like validation for every 2 AM debugging session for 2 days straightup. I am so grateful to Yash and Savyasachi for teaching me so many things.

This code is exactly what ran on competition day — no cleanup, no refactoring, no prettying things up after the factual win. The commented-out functions, the exact delay values, the serial print statements. All of it. 

If you're reading this and building something similar — good luck. It's worth it. Electronics is fun and always will be.

---

### Contact

- **GitHub**: [@anirxddh](https://github.com/anirxddh)
- **LinkedIn**: [Aniruddha Dey](https://www.linkedin.com/in/anirxddh)
- **X**: [@anirxddh](https://x.com/anirxddh)

---

### Made with 🔧 and a lot of late nights by Team Coffee Lake.
