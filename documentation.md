
---

# ESP32 Code Deployment and Server Setup Documentation

## 1. Downloading Code to the ESP32

### 1.1 One-time setup

1. **Install the ESP32 board support.** In the Arduino IDE open
   *File → Preferences* and add this to *Additional boards manager URLs*:
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
   Then in *Tools → Board → Boards Manager* install **esp32 by Espressif
   Systems**, and pick the board **XIAO_ESP32C3** (*Tools → Board → esp32 →
   XIAO_ESP32C3*).
2. **Install the one library the sketch needs.** *Tools → Manage Libraries*:
   - **UrlEncode** (by plageoj) — provides `UrlEncode.h`.
   The calculator link code is **already inside the sketch folder**:
   `TICL.h/.cpp`, `CBL2.h/.cpp` and `TIVar.h/.cpp` are a vendored copy of
   ArTICL (by KermMartian), so there is no ArTICL install step and no
   `Library not found: <TICL.h>` error. Keep those six files next to
   `esp32.ino`, because the Arduino IDE only compiles files in a sketch's own
   folder, and keep `ArTICL-LICENSE.txt` alongside them: ArTICL is BSD-3
   licensed, so the notice has to travel with the source.
   `WiFi`, `WiFiClientSecure`, `HTTPClient` and `Preferences` ship with the
   ESP32 core, so nothing else is needed.
3. **Leave `#define CAMERA` commented out.** The XIAO ESP32C3 has no camera
   connector; with it commented the `snap`/`solve` commands just answer
   "pictures not supported" instead of breaking the build.

### 1.2 Configure and flash

1. **Open the Arduino IDE**:
   - Open the `.ino` file located in the `esp32` directory using the Arduino IDE.

2. **Configure Secrets**:
   - Navigate to the `secrets.h` file.
   - Fill in the required information (such as WiFi credentials, Hardware Pins, etc.) as specified by your project.
   - Optional switches in the same file: `WIFI_MAC` for a fixed MAC address
     ([§1.3](#13-optional-a-fixed-wifi-mac-address)), `AI_API_KEY` / `AI_MODEL`
     for direct AI mode ([§3.1](#31-the-board-does-not-need-the-server)),
     `AUTOLAUNCH` to stop the board installing the launcher by itself, and
     `PASSWORDLESS` for bringing a link up without the password.

3. **Obtain Server IP Address**:
   - Refer to the [Server Setup](#3-running-the-server) section to get the IP address of your server.
   - Update the IP address in your `secrets.h` to match the server IP.

4. **Flash the ESP32**:
   - Connect the XIAO over USB-C, select its port in *Tools → Port*, and click
     Upload. If the port does not appear, hold the **BOOT** button, tap
     **RESET**, release **BOOT**, then upload.
   - A successful flash ends with `[ready]` and a `[wiring]` line on the serial
     monitor at 115200 baud. See [Verifying the wiring](#22-verifying-the-wiring).

### 1.3 Optional: a fixed WiFi MAC address

If your router hands devices addresses by MAC (a DHCP reservation, or a Wi-Fi
allow-list), set `WIFI_MAC` in `esp32/secrets.h`:

```c
#define WIFI_MAC "02:1A:C3:84:32:01"
```

Rules of thumb:

- Format is `AA:BB:CC:DD:EE:FF`. Anything else is rejected with a serial
  message and the board keeps its hardware address.
- Make the **second** hex digit a `2`, `6`, `A` or `E`. That sets the *locally
  administered* bit, which marks the address as yours rather than a real
  vendor's. The sketch prints a reminder if you forget, but still applies it.
- The first byte must be even (unicast). An odd first byte is a multicast
  address and is refused.
- The override is applied on every boot, before the board joins your network,
  and is not stored anywhere. Delete the line to go back to the hardware MAC.

Watch the serial monitor at boot to confirm:

```
[mac] hardware: 24:0A:C4:11:22:33
[mac] using:    02:1A:C3:84:32:01
```

The `[mac] hardware:` line is the address to use for a reservation if you would
rather not override anything. This is convenience, not security: MAC addresses
are easy to spoof, so never treat an allow-list as protection.

## 2. Wiring Instructions

### 2.1 Wires-only build (no PCB, no MOSFETs)

You do not need the `pcb/` board. Three wires straight from the calculator's
2.5 mm I/O jack to the XIAO ESP32C3 are enough:

| Link cable wire | Calculator | XIAO ESP32C3 |
| --- | --- | --- |
| tip (red) | TIP | **D1** (GPIO3) |
| ring (white) | RING | **D10** (GPIO10) |
| base (bare) | GND | **GND** |

Rules for a wire build:

- **Power the XIAO from USB-C.** The board took its 5V from the PCB connector
  (`J1` pin 4); with wires there is no 5V source, and the calculator does not
  provide one.
- **GND is mandatory.** Both devices need a common reference, so the bare/sleeve
  wire always gets connected.
- **3.3V only.** ESP32-C3 I/O is not 5V tolerant, and the sketch never drives a
  link line high: it pulls a line low (open drain) or releases it and lets the
  pull-ups hold it high. That open-drain behaviour is why the two MMBF170s on
  the PCB are not needed, and because the pull-ups are on the ESP32 side, the
  wire build cannot push 5V onto a GPIO by itself.
- **No pull-up resistors, no extra components.** You do not need to add any
  resistors. The sketch turns on the ESP32-C3's built-in pull-ups on both link
  lines (`pinMode(TIP, INPUT_PULLUP)` / `pinMode(RING, INPUT_PULLUP)`, about 45k
  each), which is what the PCB's R1/R2 did. That is also the mode ArTICL/CBL2 is
  written for: it never drives a line high, it only pulls low or releases the
  line and relies on the MCU's internal pull-ups, so straight wires are the
  supported configuration, not a compromise. If a line ever misbehaves, recheck
  the wiring and the unlock value rather than adding parts.
- D1 and D10 are ordinary GPIOs, not boot strapping pins, so this wiring needs
  **no changes to `secrets.h`** beyond your WiFi credentials and server address.

### 2.2 Verifying the wiring

1. Flash the sketch, power the XIAO from USB-C, and open the serial monitor at
   115200. At boot it prints a `[wiring]` report. `TIP 1 RING 1` means both
   lines idle high, so nothing is shorted and nothing is holding the link down.
2. Run the `source/programs/LINKTEST.8xp.txt` program on the calculator (type it
   in, or compile it to `LINKTEST.8xp` with SourceCoder 3 / TI-Connect). It
   sends the unlock value and command 3, then shows what the ESP32 answered.
   A `LINK OK TIP 1 RING 1` reply means TIP, RING and GND are correct in *both*
   directions, and the launcher will work.
3. Start the ESP32 first, then run the launcher on the calculator. Opening the
   serial monitor resets the ESP32, so close it before transferring programs.

### 2.3 The unlock value

The launcher that ships inside the sketch stores `42069` into the calculator's
`P` variable and sends it; `esp32.ino` accepts `42069` and the older `69420`.
If you change `PASSWORDS` in the sketch, change it in `LINKTEST.8xp.txt` too, or
uncomment `PASSWORDLESS` in `secrets.h` to skip the unlock while testing.

Keep in mind that the launcher the ESP32 actually transfers to the calculator is
the one compiled into `esp32/launcher.h` (regenerate it with
`source/preplauncher.sh`). The copy in `source/programs/LAUNCHER.8xp.txt` is an
older revision of that program and never sends the unlock value, so if you
rebuild a launcher from it, either add `42069->P` / `Send(P)` at the top or
define `PASSWORDLESS`.

### 2.4 Other hardware

- Wiring configurations may vary depending on the ESP32 model you're using.
- Refer to your specific hardware's datasheet for pin configurations.

### 2.5 Bringing a wire build up, end to end

Do these in order. Each step proves one part before the next depends on it.

1. **Decide how the board gets its AI answers.** Either put an `AI_API_KEY` in
   `esp32/secrets.h` and skip the server entirely
   ([§3.1](#31-the-board-does-not-need-the-server)), or run the server and put
   its address in `SERVER`. Also fill in `WIFI_SSID` / `WIFI_PASS` and a
   `CHAT_NAME` either way. If you do run the server, `SERVER` must include the
   scheme and the port, e.g. `#define SERVER "http://192.168.1.50:8080"`, since
   the sketch appends paths like `/gpt/ask` directly to it.
2. **Flash the sketch** ([§1.2](#12-configure-and-flash)) with everything else
   untouched, and power the XIAO from USB-C.
3. **Wire the link** ([§2.1](#21-wires-only-build-no-pcb-no-mosfets)): TIP → D1,
   RING → D10, GND → GND, three wires, no resistors.
4. **Check the serial monitor.** Expect `[ready]` then `[wiring] TIP 1 RING 1`.
   If a line reads `0`, the link is being held low — fix the wiring before
   going further.
5. **Let the board install the launcher itself.** Leave the calculator at its
   home screen (or put it on `2nd → LINK → RECEIVE`) and watch the serial
   monitor: right after `[ready]` the sketch prints
   `[launcher] offering TI32 to the calculator` and keeps offering it every 5
   seconds for about two minutes. When it lands you get
   `[launcher] TI32 is on the calculator - run it from PRGM`. Nothing is needed
   on the calculator, and no link cable or TI-Connect is involved: the launcher
   is the one compiled into `launcher.h`, which sends the unlock value itself.
6. **If it never lands**, put the calculator on `LINK → RECEIVE`, then tap
   RESET on the XIAO (or unplug and replug USB-C) for a fresh two minutes of
   attempts. If the `[launcher]` lines never appear at all, `AUTOLAUNCH` has
   been commented out in `secrets.h`.
7. **Run `TI32` on the calculator.** Its menus drive GPT, images, chat and
   programs; `UPDATE` re-downloads the sketch's launcher copy any time you flash
   a newer one.
8. **Close the serial monitor for normal use.** Opening it resets the ESP32 and
   interrupts a transfer in progress.

LINKTEST (`source/programs/LINKTEST.8xp.txt`) is still worth having to prove
both directions of the wiring, but it is no longer part of getting started.
Remember those files are program *sources*: type them into the calculator, or
compile them to `.8xp` with SourceCoder 3 or TI-Connect, before running them.

If a GPT question fails while the link itself works, the problem is on the
network side rather than the wiring: check the `successful unlock` line, then
the `ai status` or HTTP code the sketch prints
([§3.1](#31-the-board-does-not-need-the-server)).

## 3. Running the Server

The server is **optional**. The ESP32 can ask OpenAI directly, which is what you
want if you have no spare machine to leave running - see
[§3.1](#31-the-board-does-not-need-the-server) and skip this section. Run it
when you want the extras: the IMAGES library, chat rooms, the APPS program list,
and keeping the API key on your computer instead of on the board.

1. **Prerequisites**:
   - Ensure you have [Node.js](https://nodejs.org/) and npm (Node Package Manager) installed on your machine.

2. **Install Server Dependencies**:
   - Navigate to the `server` directory in your terminal or command prompt.
   - Run the following command to install the necessary dependencies:
     ```bash
     npm install
     ```

3. **Add your OpenAI API key**:
   - Create a file called `.env` in the `server` directory containing:
     ```
     OPENAI_API_KEY=sk-...
     ```
     This is the server-side copy of the key, used when the board's own
     `AI_API_KEY` is left empty. Without it the server still starts, but GPT
     questions answer `500` and show an error on the calculator. `PORT` is
     optional and defaults to `8080`. The board and the server can both hold a
     key; the board's takes priority because it never leaves the board. Note the
     server's own code is written against OpenAI's SDK, so it is the *server's*
     key that would need changing to point it at NVIDIA - the direct mode above
     is the supported way to run on NIM.

4. **Start the Server**:
   - To run the server, execute the following command from the `server`
     directory:
     ```bash
     node index.mjs
     ```
   - It prints `listening on 8080`. Leave this terminal running; the ESP32 has
     to reach it over your LAN, so the two must be on the same network.
   - Optional extras, both of which just need a folder that exists when the
     server starts (it reads them once at boot): drop `.8xp` files into
     `server/programs/` to fill the calculator's APPS menu, and put pictures in
     `server/images/` for the IMAGES menu (this version has no camera support,
     so `snap`/`solve` are limited to "pictures not supported").

### 3.1 The board does not need the server

The sketch can call the model itself, so the GPT commands work with no companion
server, no Node, and no computer left running. The default provider is NVIDIA's
hosted NIM API:

1. Get a key from [build.nvidia.com](https://build.nvidia.com/): sign in, open a
   model page (for example `meta/llama-3.1-8b-instruct`), and press **Get API
   Key**. The key starts with `nvapi-`. Use that button rather than generating a
   key elsewhere on the site: it also grants the key the *Public API Endpoints*
   permission the endpoint needs, and without it every call answers `403`.
2. In `esp32/secrets.h` set:
   ```c
   #define AI_API_URL "https://integrate.api.nvidia.com/v1/chat/completions"
   #define AI_API_KEY "nvapi-..."
   #define AI_MODEL "meta/llama-3.1-8b-instruct"
   ```
   Leave `SERVER` as `""`. The board uses the key whenever it is not empty and
   falls back to `SERVER` when it is. The model must be one your key can reach;
   the model list on build.nvidia.com shows a *Ready* badge next to callable
   ones.
3. Reflash. Nothing else changes: `WIFI_SSID` / `WIFI_PASS` are still needed,
   because the board talks to `integrate.api.nvidia.com` over HTTPS.
4. Ask a question from the launcher. The serial monitor logs `ai provider`,
   `ai model`, `ai status: 200` and then the reply. A `401`/`403` is the key or
   its permissions, a `404` usually means `AI_MODEL` is wrong, and a negative
   status means the board never reached the network.

Any other provider that speaks the same OpenAI-compatible
`/v1/chat/completions` shape works by changing only those three lines - for
OpenAI itself that is `https://api.openai.com/v1/chat/completions`, an `sk-...`
key, and something like `gpt-4o`. The request is built as
`{"model":…, "stream":false, "max_tokens":512, "messages":[…]}` so replies stay
small enough to page onto the calculator.

Two things worth knowing about this mode:

- **The key lives on the board.** Anyone who can read the flash or the serial
  line can read it, so treat a board with a key on it like a credential. Use a
  restricted key, or run the server instead and keep the key there.
- **Images, chat rooms and the programs list still need the server**, because
  they are files under `server/images/`, `server/programs/` and `server/chat.json`
  rather than something the board can answer on its own. With `SERVER` empty
  those menu items now say `no server: images/chat/programs need it` instead of
  hanging or failing silently. Everything else - ASK/GPT, the reply follow-up
  (`HISTORY`) and the launcher update - works with no server at all.

Certificate checking is deliberately off for this connection (`setInsecure()`),
because the board has no room for a CA bundle; that is the same trade-off the
sketch already makes for the companion server under `SECURE`.

## 4. Retrieving Your Server IP Address

Only needed if you run the server: this is the address that goes in `SERVER` in
`secrets.h`. Skip it if you are using
[§3.1](#31-the-board-does-not-need-the-server).

To obtain the server's IP address:

### On Windows:

1. Open the Command Prompt by searching for "cmd" in the start menu or pressing `Win + R` and typing `cmd`.
2. Run the following command:
   ```bash
   ipconfig
   ```
3. Locate your specific network adapter (e.g., Wi-Fi or Ethernet).
4. Use the IPv4 address associated with that network adapter. This will be the server's IP address.

---

### Additional Notes:
- Ensure your ESP32 and server are connected to the same network.
- If you encounter issues with the server or ESP32 connectivity, verify firewall settings and network configurations.

