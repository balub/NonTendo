# Nontendo

**Nontendo** is a minimal gamepad project built using the [Glyph-C3](https://pcbcupid.com/glyph-c3) board from **PCBCupid**. It showcases a set of classic retro-style games implemented directly on the ESP32-C3 microcontroller, featuring simple display, audio, and button inputs — all built into the tiny Glyph-C3 form factor.

![Nontendo Glyph-C3 Gamepad](image.png)

---

## What's in the Box?

This repository includes fully working Arduino-based implementations of the following games:

| Game                | Folder Name         | Description                                                   |
| ------------------- | ------------------- | ------------------------------------------------------------- |
| 🐍 Snake            | `work`              | A thinner, slower snake game with buzzer and win/lose states. |
| 🧍 Prince of Persia | `peasant-of-persia` | A tiny tribute with basic movement, traps, and ledges.        |
| ❌⭕ Tic Tac Toe    | `tic-tac-toe`       | 2-player local mode with visual feedback.                     |
| 🏓 Pong             | `pong`              | Single or dual-player paddle battle.                          |

---

## How It Works

- **Board**: [Glyph-C3](https://pcbcupid.com/glyph-c3) (ESP32-C3 based)
- **Display**: 128×32 I2C OLED (SSD1306)
- **Inputs**: Two tactile buttons (`leftBtn = GPIO3`, `rightBtn = GPIO2`)
- **Audio**: Buzzer on GPIO21 for sound effects

Each game is a self-contained Arduino sketch that you can compile and flash using the Arduino IDE or PlatformIO.

---

## Getting Started

### Requirements

- Arduino IDE (or PlatformIO)
- ESP32 board package (ensure **ESP32-C3** is selected)
- Libraries:
  - [`Adafruit SSD1306`](https://github.com/adafruit/Adafruit_SSD1306)
  - [`Adafruit GFX`](https://github.com/adafruit/Adafruit-GFX-Library)

### Wiring (already pre-wired in Glyph-C3)

| Pin        | Connected To   |
| ---------- | -------------- |
| GPIO2      | Right Button   |
| GPIO3      | Left Button    |
| GPIO21     | Buzzer         |
| I2C (OLED) | SDA, SCL lines |

### Flash a Game

1. Open the Arduino IDE
2. Load a sketch (e.g., `work` for Snake)
3. Select the board: `ESP32C3 Dev Module`
4. Select the correct COM port
5. Upload the code

---

## Gameplay Notes

- All games use **left** and **right** buttons for interaction.
- Some games rely on rotation or screen orientation.
- Sounds play via a passive buzzer on game events (e.g., collision, win, etc.).
- All graphics are optimized for a 128×32 OLED screen.

---

## Credits

- Hardware: [Glyph-C3](https://learn.pcbcupid.com/boards/glyph-c3/overview) by **PCBCupid**

---

## Future Ideas

## Make it good and less jank.

## License

MIT — free to use, remix, and share.  
Feel free to fork and expand the Nontendo experience.

---

> _"It’s not Nintendo… but it’s good enough to kill time on the bus."_
