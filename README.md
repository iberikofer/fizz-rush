# 🚀 Fizz Rush

![C++](https://img.shields.io/badge/C++-17-00599C?logo=c%2B%2B&logoColor=white) ![SFML](https://img.shields.io/badge/Library-SFML_3.0.2-8ECA45) ![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?logo=windows&logoColor=white) ![Episodes](https://img.shields.io/badge/Episodes-3-ff69b4) ![Build](https://img.shields.io/badge/Build-Passing-brightgreen)

> A dynamic arcade game built with C++ and SFML 3.0.2

---

## 🎮 About the Game

**Fizz Rush** is an exciting, fast-paced arcade game where you play as a soda can! Your primary objective is simple yet challenging: dodge relentless obstacles, outmaneuver enemies, and collect hearts to survive. The game is structured into multiple thrilling episodes, each introducing unique mechanics, increasing difficulty, and culminating in an epic Boss fight.

### 🌟 Features

- **⚡ Dynamic Gameplay:** Fast-paced action with progressively increasing difficulty and intelligent enemy spawning.
- **👾 Diverse Enemies:** Face off against multiple enemy types (Hands, Bars, Chasers) and survive the ultimate Boss battle in the finale!
- **❤️ Health & Invincibility System:** Collect hearts dropped during gameplay to restore health. Gain brief invincibility frames upon taking damage to reposition yourself.
- **🎶 Immersive Audio:** Each episode features its own unique background soundtrack and satisfying sound effects for hits, healing, and UI interactions.
- **⚙️ Customizable Settings:** A fully functional interactive menu allowing you to toggle music, switch difficulties (Easy, Normal, Hard), and manage debug mode.

## 🛠 Technical Details

- **Language:** C++ 17
- **Graphics & Audio:** [SFML 3.0.2](https://www.sfml-dev.org/) (Simple and Fast Multimedia Library)
- **Compiler:** GCC / MinGW
- **Architecture Highlights:**
  - **Custom Physics & Collision Detection:** Uses an optimized circle-based intersection algorithm (`CollisionCircle`) to handle precise hitboxes instead of basic rectangles.
  - **State Machine Architecture:** Robust handling of game states (`MainMenu`, `Playing`, `Paused`, `Settings`, `GameOver`, `EpisodeTransition`, `Victory`) and Boss AI phases (`Starting`, `Phase1`, `Phase2`, `Death`).
  - **Smooth Transitions:** Custom alpha-blending logic for seamless fade-ins and fade-outs between menus and game episodes.

## 🕹 Controls

Fizz Rush supports both Keyboard and Gamepad/Joystick inputs for a versatile gaming experience!

### ⌨️ Keyboard & Mouse

- `Arrow Keys` (Up, Down, Left, Right) — Move the soda can during gameplay.
- `Left Mouse Click` — Interact with the Main Menu and UI buttons.
- `Escape` — Pause the game / Return to Main Menu.
- `Tab` — Quick exit / Close the game window.

### 🎮 Gamepad / Joystick

- `Left Analog Stick` (Axis X / Axis Y) — Smoothly steer and move the soda can (supports built-in deadzone filtering for precise movement).
- *Make sure your controller is connected before starting the game for it to be automatically detected!*

---

## ⚙️ Build & Run Instructions

Since the game relies on the SFML library, you need to download it for local compilation:

1. **Clone the repository:**

   ```bash
   git clone https://github.com/your-username/fizz-rush.git
   cd fizz-rush
   ```

2. **Download & Setup SFML 3.0.2:**

   - Download [SFML 3.0.2 for MinGW](https://www.sfml-dev.org/download/sfml/3.0.2/) and extract the archive.
   - Place the extracted folder directly into the project root directory and rename it to exactly `SFML-3.0.2` (it should contain `include`, `lib`, etc.).
   - *Note: Don't forget to copy all the `.dll` files from the `SFML-3.0.2/bin` folder into the project's `bin` folder for the game to run correctly.*

3. **Compile the game using `make`:**

   ```bash
   mingw32-make
   ```

4. **Run the game:**

   ```bash
   mingw32-make run
   ```

---

Made with ❤️ and C++
