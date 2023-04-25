//Gets the Bitcoin/USD Exchange rate from the Coindesk API every 5 minutes
void bitcoinPrice() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;

    if (client.connect("api.coindesk.com", 80)) {
      client.print(String("GET /v1/bpi/currentprice.json HTTP/1.1\r\n") +
                   "Host: api.coindesk.com\r\n" +
                   "User-Agent: Arduino/1.0\r\n" +
                   "Connection: close\r\n\r\n");

      String payload = "";
      bool headers_end = false;

      while (client.connected()) {
        String line = client.readStringUntil('\n');

        if (line == "\r") {
          headers_end = true;
          continue;
        }

        if (headers_end) {
          payload += line;
        }
      }

      if (payload.length() > 0) {
        JSONVar json_obj = JSON.parse(payload);

        if (JSON.typeof(json_obj) == "undefined") {
          Serial.println("Parsing JSON failed.");
          return;
        }

        double price = json_obj["bpi"]["USD"]["rate_float"];
        String price_str = "$" + String(price, 6);
        Serial.println("Bitcoin price: " + price_str);

        // Store the last called bitcoin price and update the last_update variable
        last_price = price_str;
        last_price_update = millis();

        // Call function to display data on ESP8266 display
        showNewData(price_str);
      } else {
        Serial.println("HTTP request failed.");
      }
    } else {
      Serial.println("Failed to connect to server.");
    }

    client.stop();
  }
}



//Gets the current Bitcoin block height from the blockchain.info API every minute

void bitcoinBlock() {
  if (WiFi.status() == WL_CONNECTED) {
    // Connect to the mempool.space API server
    WiFiClientSecure client;
    client.setInsecure();

    if (!client.connect("mempool.space", 443)) {
      Serial.println("Failed to connect to server.");
      return;
    }

    // Send a GET request to fetch the latest Bitcoin block height
    client.print(String("GET /api/blocks/tip/height HTTP/1.1\r\n") +
                 "Host: mempool.space\r\n" +
                 "User-Agent: Arduino/1.0\r\n" +
                 "Connection: close\r\n\r\n");

    // Read the response from the server and extract the block height value
    String response = client.readString();
    int index = response.indexOf("\r\n\r\n") + 4; // skip past HTTP headers
    String block_height_str = response.substring(index);
    block_height_str.trim();
    Serial.println("Bitcoin block height: " + block_height_str);

    // Store the last called block height and update the last_update variable
    last_block = block_height_str;
    last_block_update = millis();

    // Call function to display data on ESP8266 display
    showNewData(block_height_str);

    client.stop();
  }
}