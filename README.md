# clifx-hw-esp32

A physical controller for LIFX lights, built on an ESP32. The goal is a box on
the desk with a dial and a couple of knobs that sets your lights' color
directly over the LAN, using [clifx](https://github.com/AndrewFila/clifx) for
the protocol.

Right now it's at the prototype stage. The controls drive an RGB LED on the
board rather than real bulbs, so the color math and input handling can get
worked out before the networking goes in.

## Controls

| Input           | Pin      | Does                                      |
|-----------------|----------|-------------------------------------------|
| Rotary encoder  | 34, 39   | Hue, 5° per click, wraps around at 360°    |
| Encoder button  | 36       | On / off                                  |
| Potentiometer   | 4        | Saturation, 0–100%                        |
| Potentiometer   | 35       | Brightness, 0–100%                        |
| RGB LED         | 12/27/14 | Shows the current color (R/G/B)           |

Every change also prints the HSV value and the resulting RGB over serial at
115200 baud.

## Building

It's a [PlatformIO](https://platformio.org/) project targeting `esp32dev`
with the Arduino framework.

```sh
pio run -t upload
pio device monitor -b 115200
```

## Next

- WiFi, and sending `SetColor` to real bulbs with clifx
- An LCD showing the current color
