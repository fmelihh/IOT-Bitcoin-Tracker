This project is an IoT application on an ESP32 circuit that monitors the current price of Bitcoin via Wi-Fi using Binance APIs, displays it on an OLED screen, and triggers an n8n flow if the threshold limit is exceeded.

The triggered n8n flow scans the current market through Perplexity APIs and sends the up-to-date information as a report via Gmail.

The platform used to design the circuit is https://wokwi.com


### Circuit Desings
![circuit off](/static/circuit1.png)
![circuit onn](/static/circuit2.png)

The circuit codes are in the sketch.ino file.

You can use the `diagram.json` file to create the circuit directly on the Wokwi platform.


### N8N Flow
![n8n](/static/n8n.png)

You can use the `n8n.json` file to create n8n flow