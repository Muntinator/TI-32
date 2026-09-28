# TI-32

![built pcb](./pcb/built.png)

[!["Buy Me A Coffee"](https://www.buymeacoffee.com/assets/img/custom_images/purple_img.png)](https://buymeacoffee.com/contactchrh)

## Documentation

Read the DOCUMENTATION.md file

Building it without the PCB (wires straight to an ESP32C3, no MOSFETs)? See
[Wiring Instructions](documentation.md#2-wiring-instructions).

The board can call an LLM by itself (NVIDIA NIM by default, any OpenAI-compatible
endpoint), so you do not have to run the companion server for the GPT commands -
see [No-server mode](documentation.md#31-the-board-does-not-need-the-server).

The sketch in `esp32/` is self-contained: the ArTICL link library
(`TICL.*`, `CBL2.*`, `TIVar.*`) is vendored into the sketch folder, so the only
library left to install is `UrlEncode` - see
[One-time setup](documentation.md#11-one-time-setup).

## Video
[![YouTube](http://i.ytimg.com/vi/Bicjxl4EcJg/hqdefault.jpg)](https://www.youtube.com/watch?v=Bicjxl4EcJg)


