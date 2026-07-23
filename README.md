# 🚀 Fizz Rush

![C++](https://img.shields.io/badge/C++-17-red?logo=c%2B%2B&logoColor=white) ![SFML](https://img.shields.io/badge/Library-SFML_3.0.2-yellow) ![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?logo=windows&logoColor=white) ![Episodes](https://img.shields.io/badge/Episodes-3-purple) ![Build](https://img.shields.io/badge/Build-Passing-brightgreen)

> A dynamic arcade game built with C++ and SFML 3.0.2

*Read this in [🇺🇦 Українською](#-fizz-rush-українська-версія)*

---

## 🎮 About the Game

**Fizz Rush** is an exciting, fast-paced arcade game where you play as a soda can! Your primary objective is simple yet challenging: dodge relentless obstacles, outmaneuver enemies, and collect hearts to survive. The game is structured into multiple thrilling episodes, each introducing unique mechanics, increasing difficulty, and culminating in an epic Boss fight.

### 🌟 Features

- **⚡ Dynamic Gameplay:** Fast-paced action where surviving becomes harder the longer you play, featuring an intelligent enemy spawn system.
- **👾 Diverse Enemies:** Face off against multiple enemy types (Hands, Bars, Chasers) and survive the ultimate Boss battle in the finale!
- **👹 Multi-Phase Boss Battle:** The final episode features a meticulously designed boss fight. The boss transitions through different phases, unleashing varying attack patterns including ricocheting cookies and sweeping brushes (fast and slow variants), challenging your reflexes to the limit!
- **✨ Visual Polish & Effects:** Experience a handcrafted particle system used for boss defeat animations, seamless screen fade transitions between episodes, and a highly polished interactive UI with butter-smooth hover and press animations.
- **❤️ Health & Protection System:** Collect hearts dropped during gameplay to restore health. Gain a brief shield of invulnerability after taking damage, giving you a safe moment to escape and reposition yourself.
- **🎶 Immersive Audio:** Each episode features its own unique background soundtrack and satisfying sound effects for hits, healing, and UI interactions.
- **⚙️ Extensive Settings & Persistence:** An interactive, bilingual (English/Ukrainian) settings menu featuring an intuitive 2-column layout with 2D grid navigation. Toggle Music, VSync, FPS Counter, Debug Hitboxes, and Difficulty (Easy, Normal, Hard). All preferences are automatically saved to a `settings.ini` file and loaded on your next session!

## 🛠 Technical Details

- **Language:** C++ 17
- **Graphics & Audio:** [SFML 3.0.2](https://www.sfml-dev.org/) (Simple and Fast Multimedia Library)
- **Compiler:** GCC / MinGW
- **Architecture Highlights:**
  - **Custom Physics & Collision Detection:** Uses an optimized circle-based intersection algorithm (`CollisionCircle`) to handle precise hitboxes instead of basic rectangles.
  - **Particle & VFX System:** A lightweight, custom-built particle engine handling temporary visual effects like enemy particles and healing indicators without massive performance overhead.
  - **State Machine Architecture:** Robust handling of game states (`MainMenu`, `Playing`, `Paused`, `Settings`, `GameOver`, `EpisodeTransition`, `Victory`) and Boss AI phases (`Starting`, `Phase1`, `Phase2`, `Death`).
  - **Configuration Management:** Reliable parsing and generation of INI configuration files using standard C++ streams for seamless state persistence across sessions.
  - **Dynamic Input Switching:** Seamlessly switch between Mouse, Keyboard, and Gamepad on the fly, with intelligent mouse-snapping to the actively focused UI element.
  - **Smooth Transitions:** Custom alpha-blending logic for seamless fade-ins and fade-outs between menus and game episodes.

## 🏆 Project Achievements

This project showcases proficiency in C++ programming and modern game development principles using the SFML library.

**Key Achievements:**

- 🎯 **100% Original Code:** The entire game logic, physics engine, AI, and state management were written from scratch.
- 📚 **Educational Value:** Demonstrates advanced OOP concepts including encapsulation, inheritance, polymorphism, and custom data structures.
- 🔄 **Complete Software Cycle:** Includes planning, design, implementation, testing, and documentation phases as required for academic software engineering.

## 🕹 Controls

Fizz Rush supports both Keyboard and Gamepad/Joystick inputs for a versatile gaming experience!

### ⌨️ Keyboard & Mouse

- `Arrow Keys` (Up, Down, Left, Right) / `W, S, A, D` — Move the soda can during gameplay, or navigate the UI menus!
- `Enter` — Confirm selection in the UI menus.
- `Left Mouse Click` — Interact with the Main Menu and UI buttons.
- `Escape` — Pause the game / Return to Main Menu / Go Back in menus.

### 🎮 Gamepad / Joystick

- `Left Analog Stick` (Axis X / Axis Y) / `D-Pad` — Smoothly steer and move the soda can (supports built-in deadzone filtering for precise movement), or navigate the UI menus.
- `A Button` — Confirm selection in the UI menus.
- `B Button` — Go back / Exit menus.
- `Start Button` — Pause the game / Return to Main Menu.
- *Note on Rumble / Vibration*: The game supports controller vibration (rumble) when you take damage or defeat a boss! **This feature is exclusively available on Windows (via XInput).**
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

## ⚖️ Disclaimer & Educational Purpose

This project was developed strictly for educational and academic purposes during studies at the **Vinnytsia National Technical University (VNTU)**.

**Fair Use & Copyright Notice:**

- This game is **non-commercial** and is not intended for any form of monetization or profit.
- Certain graphical assets, images, and audio files used within this project were sourced from the internet.
- Any trademarks, brands, or specific characters depicted in this game remain the property of their respective owners. Their inclusion is purely for educational demonstration and creative learning context under Fair Use guidelines.
- If you are the copyright holder of any asset used in this repository and wish for it to be removed, please open an issue and it will be taken down immediately.

### 🎵 Credits & Acknowledgements

<details>
<summary>Click to view Audio & Music Credits</summary>

- Music by [Maksym Malko](https://pixabay.com/users/backgroundmusicforvideos-46459014/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=402451) from [Pixabay](https://pixabay.com//?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=402451)
- Sound Effect by [Aleksandr Karabanov](https://pixabay.com/users/muzaproduction-24990238/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=13491) from [Pixabay](https://pixabay.com//?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=13491)
- Sound Effect by [Universfield](https://pixabay.com/users/universfield-28281460/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=142334) from [Pixabay](https://pixabay.com/sound-effects//?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=142334)
- Music by [Luca Di Alessandro](https://pixabay.com/users/lucadialessandro-25927643/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=146875) from [Pixabay](https://pixabay.com//?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=146875)
- Music by [Melody Ayres-Griffiths](https://pixabay.com/users/melodyayresgriffiths-27269767/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=139389) from [Pixabay](https://pixabay.com/music//?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=139389)
- Music by [Emmraan](https://pixabay.com/users/emmraan-24732583/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=261292) from [Pixabay](https://pixabay.com//?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=261292)
- Sound Effect by [freesound_community](https://pixabay.com/users/freesound_community-46691455/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=14562) from [Pixabay](https://pixabay.com//?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=14562)
- Sound Effect by [Phurisarah H](https://pixabay.com/users/doubleducks-45864631/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=360379) from [Pixabay](https://pixabay.com//?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=360379)
- Sound Effect by [freesound_community](https://pixabay.com/ru/users/freesound_community-46691455/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=80047) from Pixabay
- Sound Effect by [freesound_community](https://pixabay.com/ru/users/freesound_community-46691455/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=101422) from Pixabay
- Sound Effect by [freesound_community](https://pixabay.com/ru/users/freesound_community-46691455/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=101553) from Pixabay

</details>

---

## 🇺🇦 Fizz Rush (Українська версія)

> Динамічна аркадна гра, створена на C++ та SFML 3.0.2

---

## 🎮 Про гру

**Fizz Rush** — це захоплива та динамічна аркадна гра, де ви граєте за банку газованки! Ваша головна мета проста, але водночас складна: уникати невпинних перешкод, ухилятися від ворогів та збирати сердечка, щоб вижити. Гра поділена на кілька захопливих епізодів, кожен з яких додає унікальні механіки, підвищує складність і завершується епічною битвою з Босом.

### 🌟 Особливості

- **⚡ Динамічний геймплей:** Швидкісний екшен, у якому виживати стає все важче з кожною хвилиною гри, завдяки продуманій системі появи ворогів.
- **👾 Різноманітні вороги:** Зіткніться з різними типами ворогів (Руки, Блоки, Переслідувачі) та виживіть в ультимативній битві з Босом у фіналі!
- **👹 Багатофазна Битва з Босом:** Фінальний епізод пропонує ретельно продуману битву. Босс змінює фази, використовуючи різні патерни атак, включаючи печиво, що рикошетить від стін, та гігантські щітки (повільні й швидкі), перевіряючи ваші рефлекси на міцність!
- **✨ Візуальний блиск та ефекти:** Насолоджуйтесь власною системою частинок для анімацій (наприклад, при знищенні боса), безшовними затемненнями екрану при переході між епізодами та надзвичайно відшліфованим інтерактивним інтерфейсом із плавними анімаціями наведення та натискання кнопок.
- **❤️ Система здоров'я та захисту:** Збирайте сердечка під час гри, щоб відновлювати здоров'я. Після отримання шкоди ваш персонаж отримує тимчасовий щит невразливості, що дає змогу безпечно відступити та змінити позицію.
- **🎶 Атмосферний звук:** Кожен епізод має власний унікальний саундтрек та приємні звукові ефекти для ударів, лікування та взаємодії з меню.
- **⚙️ Розширені налаштування та збереження:** Двомовне (Англійська/Українська) інтерактивне меню з інтуїтивно зрозумілим двоколонковим дизайном та 2D-навігацією. Ви можете налаштувати музику, VSync, лічильник FPS, хітбокси та рівень складності (Легкий, Нормальний, Важкий). Усі ваші налаштування автоматично зберігаються у файл `settings.ini` та завантажуються під час наступного запуску гри!

## 🛠 Технічні деталі

- **Мова:** C++ 17
- **Графіка та звук:** [SFML 3.0.2](https://www.sfml-dev.org/) (Simple and Fast Multimedia Library)
- **Компілятор:** GCC / MinGW
- **Архітектурні особливості:**
  - **Кастомна фізика та виявлення зіткнень:** Використовує оптимізований алгоритм перевірки перетинів на основі кіл (`CollisionCircle`) для обробки точних хітбоксів замість звичайних прямокутників.
  - **Система частинок та VFX:** Легкий, власноруч написаний рушій частинок, що обробляє тимчасові візуальні ефекти, такі як частинки ворогів або індикатори лікування, без зайвого навантаження на продуктивність.
  - **Архітектура керування станами (State Machine):** Надійна обробка ігрових станів (`MainMenu`, `Playing`, `Paused`, `Settings`, `GameOver`, `EpisodeTransition`, `Victory`) та фаз штучного інтелекту Боса (`Starting`, `Phase1`, `Phase2`, `Death`).
  - **Керування конфігурацією:** Надійний парсинг та генерація INI-файлів за допомогою стандартних потоків вводу/виводу C++ для збереження налаштувань між ігровими сесіями.
  - **Динамічне перемикання вводу:** Миттєве перемикання між мишею, клавіатурою та геймпадом "на льоту" з розумним "прилипанням" курсора миші до активного елемента інтерфейсу.
  - **Плавні переходи:** Кастомна логіка альфа-змішування (alpha-blending) для безшовних ефектів появи та згасання екрану між меню та ігровими епізодами.

## 🏆 Досягнення проєкту

Цей проєкт демонструє володіння мовою програмування C++ та сучасними принципами розробки ігор з використанням бібліотеки SFML.

**Ключові досягнення:**

- 🎯 **100% Оригінальний код:** Вся ігрова логіка, фізичний рушій, штучний інтелект та управління станами були написані з нуля.
- 📚 **Навчальна цінність:** Демонструє просунуті концепції ООП, включаючи інкапсуляцію, успадкування, поліморфізм та власні структури даних.
- 🔄 **Повний цикл розробки ПЗ:** Включає етапи планування, проєктування, реалізації, тестування та документації, як того вимагає академічна інженерія програмного забезпечення.

## 🕹 Управління

Fizz Rush підтримує як клавіатуру, так і геймпад/джойстик для універсального ігрового досвіду!

### ⌨️ Клавіатура та миша

- `Стрілки` (Вгору, Вниз, Вліво, Вправо) / `W, S, A, D` — Переміщення банки під час гри, або навігація по меню!
- `Enter` — Підтвердження вибору в меню.
- `Лівий клік миші` — Взаємодія з головним меню та кнопками інтерфейсу.
- `Escape` — Пауза / Повернення в головне меню / Назад у меню.

### 🎮 Геймпад / Джойстик

- `Лівий аналоговий стік` (Вісь X / Вісь Y) / `Хрестовина (D-Pad)` — Плавне керування та переміщення банки (підтримує вбудовану фільтрацію "мертвої зони" для точного переміщення), або навігація по меню.
- `Кнопка A` — Підтвердження вибору в меню.
- `Кнопка B` — Назад / Вихід із меню.
- `Кнопка Start` — Пауза / Повернення в головне меню.
- *Примітка щодо вібрації (Rumble)*: Гра підтримує вібрацію геймпада при отриманні шкоди або знищенні боса! **Ця функція працює виключно на Windows (через XInput).**
- *Переконайтеся, що ваш контролер підключений перед початком гри, щоб він був виявлений автоматично!*

---

## ⚙️ Інструкція зі збірки та запуску

Оскільки гра покладається на бібліотеку SFML, вам потрібно завантажити її для локальної компіляції:

1. **Склонуйте репозиторій:**

   ```bash
   git clone https://github.com/your-username/fizz-rush.git
   cd fizz-rush
   ```

2. **Завантажте та налаштуйте SFML 3.0.2:**

   - Завантажте [SFML 3.0.2 для MinGW](https://www.sfml-dev.org/download/sfml/3.0.2/) та розпакуйте архів.
   - Помістіть розпаковану папку прямо в кореневу директорію проєкту та перейменуйте її рівно на `SFML-3.0.2` (вона повинна містити `include`, `lib` тощо).
   - *Примітка: Не забудьте скопіювати всі файли `.dll` з папки `SFML-3.0.2/bin` у папку `bin` вашого проєкту, щоб гра запускалася коректно.*

3. **Скомпілюйте гру за допомогою `make`:**

   ```bash
   mingw32-make
   ```

4. **Запустіть гру:**

   ```bash
   mingw32-make run
   ```

---

## ⚖️ Відмова від відповідальності та навчальна мета

Цей проєкт був розроблений виключно в навчальних та академічних цілях під час навчання у **Вінницькому національному технічному університеті (ВНТУ)**.

**Fair Use та авторські права:**

- Ця гра є **некомерційною** і не призначена для будь-якої форми монетизації чи отримання прибутку.
- Певні графічні ресурси, зображення та аудіофайли, використані в цьому проєкті, були взяті з інтернету.
- Будь-які торгові марки, бренди або специфічні персонажі, зображені в цій грі, залишаються власністю їх відповідних власників. Їх включення сюди здійснюється виключно для навчальної демонстрації та творчого навчального контексту згідно з принципами добросовісного використання (Fair Use).
- Якщо ви є правовласником будь-якого ресурсу, використаного в цьому репозиторії, і бажаєте, щоб його було видалено, будь ласка, відкрийте Issue, і він буде негайно видалений.

### 🎵 Подяки та Автори (Credits)

<details>
<summary>Натисніть, щоб розгорнути список авторів аудіо</summary>

- Music by [Maksym Malko](https://pixabay.com/users/backgroundmusicforvideos-46459014/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=402451) from [Pixabay](https://pixabay.com//?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=402451)
- Sound Effect by [Aleksandr Karabanov](https://pixabay.com/users/muzaproduction-24990238/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=13491) from [Pixabay](https://pixabay.com//?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=13491)
- Sound Effect by [Universfield](https://pixabay.com/users/universfield-28281460/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=142334) from [Pixabay](https://pixabay.com/sound-effects//?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=142334)
- Music by [Luca Di Alessandro](https://pixabay.com/users/lucadialessandro-25927643/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=146875) from [Pixabay](https://pixabay.com//?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=146875)
- Music by [Melody Ayres-Griffiths](https://pixabay.com/users/melodyayresgriffiths-27269767/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=139389) from [Pixabay](https://pixabay.com/music//?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=139389)
- Music by [Emmraan](https://pixabay.com/users/emmraan-24732583/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=261292) from [Pixabay](https://pixabay.com//?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=261292)
- Sound Effect by [freesound_community](https://pixabay.com/users/freesound_community-46691455/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=14562) from [Pixabay](https://pixabay.com//?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=14562)
- Sound Effect by [Phurisarah H](https://pixabay.com/users/doubleducks-45864631/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=360379) from [Pixabay](https://pixabay.com//?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=360379)
- Sound Effect by [freesound_community](https://pixabay.com/ru/users/freesound_community-46691455/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=80047) from Pixabay
- Sound Effect by [freesound_community](https://pixabay.com/ru/users/freesound_community-46691455/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=101422) from Pixabay
- Sound Effect by [freesound_community](https://pixabay.com/ru/users/freesound_community-46691455/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=101553) from Pixabay

</details>

---

Made with ❤️ and C++ at VNTU
