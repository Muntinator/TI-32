#ifndef SECRETS_H
#define SECRETS_H

#define HTTP_USERNAME "" // not used
#define HTTP_PASSWORD "" // not used

#define WIFI_SSID "" // Your wifi name
#define WIFI_PASS "" // Your wifi password

// Optional: make the router always see the same device by overriding the board's
// WiFi MAC address, which is handy for a DHCP reservation or a MAC allow-list.
// Format AA:BB:CC:DD:EE:FF. Leave it empty to keep the hardware address. Use a
// locally administered address (second hex digit 2, 6, A or E) so you are not
// pretending to be a specific vendor's hardware. Applied before WiFi.begin() on
// every boot. This is for convenience, not security: MAC addresses are spoofable.
#define WIFI_MAC ""
#define SERVER "" // Your server ip address, or "" if you only want direct AI
#define CHAT_NAME "" // Your name on the chat program
// #define SECURE // not used

// Call the model straight from the board, so you do not need to run the Node
// server in server/ at all for the GPT commands. With AI_API_KEY filled in,
// SERVER can stay empty. Images, chat rooms and the programs list are files on
// the server, so those still need it.
//
// The default is NVIDIA's hosted NIM API. Get a key from build.nvidia.com: open
// a model page, press "Get API Key" (that flow also grants the key the "Public
// API Endpoints" permission the endpoint requires), and it starts with nvapi-.
// Any provider with an OpenAI-compatible /v1/chat/completions endpoint works;
// these three lines are the only thing that changes to switch,
//   NVIDIA: https://integrate.api.nvidia.com/v1/chat/completions
//           key nvapi-...  model e.g. meta/llama-3.1-8b-instruct
//   OpenAI: https://api.openai.com/v1/chat/completions
//           key sk-...     model e.g. gpt-4o
#define AI_API_URL "https://integrate.api.nvidia.com/v1/chat/completions"
#define AI_API_KEY "" // nvapi-...
#define AI_MODEL "meta/llama-3.1-8b-instruct"

// Have the ESP32 install the launcher on the calculator by itself at startup,
// so the first install needs no link cable, no TI-Connect and no typed-in
// program. Comment this out if you would rather the board stayed off the link
// until the calculator talks to it. See pushLauncher() in esp32.ino.
#define AUTOLAUNCH

// Wires-only build (no PCB): uncomment while you are bringing the link up to
// accept commands without the launcher's unlock value. Leave it off for a
// finished build so the calculator still has to send the password. You only
// need this if the calculator's launcher is not the one compiled into the
// sketch, because that one sends the password itself.
// #define PASSWORDLESS

#endif // SECRETS_H {CUSTOM}
