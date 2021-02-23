# AdvancedBackpackLight

Easily control the 10 Neopixels on the Adafruit Circuit Playground Bluefruit from the Adafruit Bluefruit mobile app.
This board uses an nRF58240 MCU. Adafruit product page: https://learn.adafruit.com/adafruit-circuit-playground-bluefruit

The controls are accessable through the "Controller" module. Use the "Color Picker" submodule with Button #1 to set a custom color.

| Button        | Function                                   | Notes  |
| ------------- |:-------------:                             | -----: |
| Up Arrow      | Increase Brightness                        |        |
| Down Arrow    | Decrease Brightness                        |        |
| #1 Botton     | Color from "Color Picker"                  |        |
| #2 Botton     | Rainbow Cycle                              |        |
| #3 Botton     | Set Brightness based on light sensor       |        |
| #4 Botton     | Turn off lights                            |        |

The slide switch on the circuit board is used to toggle automatic brightness adjust. When in the on position the brightness of the leds will be adjusted based on the ambient light sensor reading every three seconds.
