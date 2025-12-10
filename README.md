# 🥤 Fizz rush!

![Game Status](https://img.shields.io/badge/Status-Playable-green)
![SFML Version](https://img.shields.io/badge/SFML-3.0.2-blue)
![Language](https://img.shields.io/badge/C++-17-orange)

**Fizz rush** is a fast-paced 2D arcade game where you will behold a story of a brave soda can.

> This project was built to explore game engine architecture using **Modern C++** and the newly released **SFML 3.0.2**.

## 🎮 Features

* **Dynamic Gameplay:** Smooth movement physics with acceleration and diagonal handling.
* **Collision System:** Custom AABB collision detection with wall bouncing and enemy interaction.
* **State System:** Fully functional Main Menu, Gameplay loop, and Pause system.
* **Audio & Visuals:** Integrated sound effects (collisions, movement) and "breathing" scale animations.
* **Responsive Window:** Supports Fullscreen (F11) and Windowed modes with auto-scaling background.
* **Modern Tech:** Built using the latest SFML 3.0.2 library.

## 📸 Screenshots

![Gameplay Screenshot](path/to/screenshot.png)

## 🕹️ Controls

| Keyboard |
| Key | Action |
| :--- | :--- |
| **Arrow Keys** | Move the Sprite can |
| **ESC** | Pause Game / Open Menu |
| **F11** | Toggle Fullscreen |
| **Mouse** | Navigate Menu Buttons |

## 🛠️ How to Build & Run

This project uses **VS Code** with **MinGW (UCRT64)**.

### Prerequisites
* C++17 compatible compiler (GCC/MinGW recommended).
* SFML 3.0.2 libraries (included in `(last version folder)`).

### Building
1.  Clone the repository.
2.  Open the folder in **VS Code**.
3.  Press `Ctrl + Shift + B` to build the project.
4.  Run the executable from the `bin` folder.

*Note: Ensure `.dll` files and the `assets` folder are in the same directory as the executable.*

## 📂 Project Structure

* `src/` - Source code (.cpp and .hpp files).
* `bin/` - Executables and DLLs.
* `assets/` - Game resources (fonts, images, sounds).
* `SFML-3.0.2/` - Library files.

## 📜 License

This project is for educational purposes.
SFML is free and open-source software provided under the terms of the zlib/png license.
