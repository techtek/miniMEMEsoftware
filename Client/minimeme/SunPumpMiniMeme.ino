// miniMEME v0.74
// Made by Techtek https://github.com/techtek
// join the Discord https://discord.gg/GyjvNMtJ5V


// Include the jpeg decoder library
#include <TJpg_Decoder.h>

// Include WiFi and HTTP client
#include <WiFi.h>
#include <HTTPClient.h>

// Include the TFT library
#include "SPI.h"
#include <TFT_eSPI.h>       // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();  // Invoke custom library

// Include time-related libraries
#include "time.h"

// Include ArduinoJson library
#include <ArduinoJson.h>

// Add your WiFi credentials here
#define WIFI_SSID "AddHere"
#define PASSWORD "AddHere"

#include <WebServer.h>
WebServer server(80);   // Use port 80 for HTTP
int slideSpeed = 3000;  // Default slide speed in milliseconds


String generateHTML() {
  String html = "<html><body style='color:darkgrey;background-color:black;'>";
  html += "<br>";
  html += "<td><img src='https://techtek.github.io/miniMEMEwebsite/minimemelogo.png' style='width:366px;height:auto;'></td>";

html += "<br>";

// Slideshow HTML
html += "<div id='slideshow' style='position: relative; width: 130px; height: 130px; overflow: hidden; margin: auto;'>";
html += "<img id='slideImage' src='https://raw.githubusercontent.com/techtek/Downloads/main/image1.jpg' style='width: 100%; height: 100%; position: absolute; top: 0; left: 0;'>";
html += "</div>";

// Slideshow script
html += "<script>";
html += "const images = [";
html += "'https://raw.githubusercontent.com/techtek/Downloads/main/image1.jpg',";
html += "'https://raw.githubusercontent.com/techtek/Downloads/main/image2.jpg',";
html += "'https://raw.githubusercontent.com/techtek/Downloads/main/image3.jpg',";
html += "'https://raw.githubusercontent.com/techtek/Downloads/main/image4.jpg'";
html += "];";
html += "let currentIndex = 0;";
html += "function showNextImage() {";
html += "  currentIndex = (currentIndex + 1) % images.length;";
html += "  document.getElementById('slideImage').src = images[currentIndex];";
html += "}";
html += "setInterval(showNextImage, 3000);";  // Change image every 3 seconds
html += "</script>";


  html += "<br></br>";

 
  // Slideshow speed settings form
  html += "<h2><b>Settings</b></h2>";
  html += "<br>";
  html += "<form action=\"/setSpeed\" method=\"POST\">";
  html += "Slide Speed (ms): <input type=\"text\" name=\"speed\" value=\"" + String(slideSpeed) + "\"><br>";

  html += "<br></br>";

 // TFT Rotation settings
html += "<h2>Screen Rotation</h2>";
html += "<form action=\"/saveSettings\" method=\"POST\">";
html += "<input type=\"radio\" name=\"rotation\" value=\"1\"> Right<br>";
html += "<input type=\"radio\" name=\"rotation\" value=\"3\" checked> Left<br>";  // Default rotation

html += "<br>";

// Styled Save Settings button
html += "<button class=\"button\" type=\"submit\"><b>Save Settings</b></button>";
html += "</form>";

html += "<br>";

// Start HTML content
html += "<style>";
html += "body { font-family: Arial, sans-serif; display: flex; flex-direction: column; align-items: center; margin: 0; padding: 0; }";
html += ".container { display: grid; grid-template-columns: 1fr 1fr; gap: 20px; max-width: 400px; margin-top: 20px; }";
html += "h2 { color: #333; font-size: 18px; margin-bottom: 10px; text-align: center; }";
html += ".button { width: 100%; padding: 15px; background-color: #080808; color: white; border: none; border-radius: 8px; cursor: pointer; font-size: 16px; text-align: center; }";
html += ".button:hover { background-color: #14a30b; }";
html += "</style>";

html += "<h2>Switch SlideShow</h2>";
html += "<div class=\"container\">";

// Show Sundog button
html += "<form action=\"/showSundog\" method=\"POST\">";
html += "<button class=\"button\">Sundog Screen</button>";
html += "</form>";

// Show Bull button
html += "<form action=\"/showBull\" method=\"POST\">";
html += "<button class=\"button\">Bull Screen</button>";
html += "</form>";

// Full-screen collage button
html += "<form action=\"/stopAndSwitch\" method=\"POST\">";
html += "<button class=\"button\">MEME Collage</button>";
html += "</form>";

// Show Home button
html += "<form action=\"/home\" method=\"POST\">";
html += "<button class=\"button\">Reset</button>";
html += "</form>";

html += "</div>";

html += "<p style=\"opacity: 0.1;\"><strong>Uptime (Sec):</strong> " + String(millis() / 1000) + "</p>";








  html += "</body></html>";

  return html;
}


bool stopAndSwitch = false;  // Flag to switch between loops


// load interface images as byte array
#include "minimeme.h"     //logo image as byte array
#include "NEWmeme.h"      //NEW meme label image as byte array
#include "trxtronlogo.h"  //NEW meme label image as byte array
#include "sundogbackground.h"  //NEW meme label image as byte array
#include "bullbackground.h"  //NEW meme label image as byte array


// NTP settings
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;      // Your GMT offset
const int daylightOffset_sec = 3600;  // Daylight saving time offset

// Image URLs and filenames
const char* imageUrls[] = {
  "https://raw.githubusercontent.com/techtek/Downloads/main/image1.jpg",
  "https://raw.githubusercontent.com/techtek/Downloads/main/image2.jpg",
  "https://raw.githubusercontent.com/techtek/Downloads/main/image3.jpg",
  "https://raw.githubusercontent.com/techtek/Downloads/main/image4.jpg",
  "https://raw.githubusercontent.com/techtek/Downloads/main/image5.jpg"
};

const char* imageFiles[] = {
  "/image1.jpg",
  "/image2.jpg",
  "/image3.jpg",
  "/image4.jpg",
  "/image5.jpg"
};

const int numImages = 5;
int currentIndex = 0;

// Time variables
char timeHour[3];
char timeMin[3];
char timeSec[3];
char day[3];
char month[6];
char year[5];
char timeWeekDay[3];

#define purple 0x604D

// API URL for coins, combined request
const char* apiUrl = "https://api.dexscreener.com/latest/dex/pairs/tron/tdr7rpu33htog8qo9i676v56bzcjkpjqox,thffwkt8qbmbzyhxen3dmknslm3dbsmmw4,txpp6hxd3cpcd5hsdmbc78iaicxrqijjm1,tcngkaqhefpkecxokzp2ysv1bd2w8arxc8,tjpskeuvnzyq2bdetchdtskljpkqauj1sw,thumltrgjbfamaoa8isuuwfjfkmhtbkzof,ta54vyjqv2pkkwqcre2bmexyevep9ouy9y";

// Custom names for the coins
const char* customCoinNames[] = {
  "SUNDOG ",  // Replace with the actual custom names
  "SUNCAT ",
  "BULL   ",
  "IVFUN  ",
  "MUNCAT ",
  "SUNWUK ",
  "CZC    "
};

const int numCustomNames = 7;


// API URL for TRX only
const char* apiUrlTRX = "https://api.dexscreener.com/latest/dex/pairs/tron/tfgdbuyp8xez44c76fin3bn3ss6jugouwj";  //TRX price api




// Function to render JPEG to the TFT
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  if (y >= tft.height()) return 0;
  tft.pushImage(x, y, w, h, bitmap);


  return 1;
}

// Buffer to hold the downloaded JPEG image
uint8_t* imageBuffer = nullptr;
int imageSize = 0;

// Function to fetch image into memory
bool getFileToMemory(const char* url) {
  HTTPClient http;
  http.begin(url);            // Specify the URL
  int httpCode = http.GET();  // Make the GET request

  if (httpCode == HTTP_CODE_OK) {
    imageSize = http.getSize();
    if (imageSize <= 0) {
      Serial.println("Invalid image size.");
      http.end();
      return false;
    }

    // Allocate memory for the image
    imageBuffer = (uint8_t*)malloc(imageSize);
    if (!imageBuffer) {
      Serial.println("Failed to allocate memory for the image.");
      http.end();
      return false;
    }

    WiFiClient* stream = http.getStreamPtr();
    int bytesRead = 0;

    while (http.connected() && bytesRead < imageSize) {
      int available = stream->available();
      if (available) {
        int len = stream->readBytes(imageBuffer + bytesRead, available);
        bytesRead += len;
        Serial.printf("Downloaded %d/%d bytes\n", bytesRead, imageSize);
      }
      delay(1);  // Prevent watchdog reset
    }

    http.end();
    return true;
  } else {
    Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
    http.end();
    return false;
  }
}

// Function to display the image directly from memory
void drawImageFromMemory(int x, int y) {
  if (!imageBuffer || imageSize <= 0) {
    Serial.println("No image in memory to display.");
    return;
  }

  // Use TJpg_Decoder to render the image from memory
  TJpgDec.drawJpg(x, y, imageBuffer, imageSize);

  // Free the image buffer after use
  free(imageBuffer);
  imageBuffer = nullptr;
  imageSize = 0;
}

// Handle Web Requests
void handleRoot() {
  server.send(200, "text/html", generateHTML());
}

void handleSetSpeed() {
  if (server.hasArg("speed")) {
    slideSpeed = server.arg("speed").toInt();
  }

  if (server.hasArg("rotation")) {
    int rotation = server.arg("rotation").toInt();
    tft.setRotation(rotation);  // Set TFT rotation based on user input
  }

  server.send(200, "text/html", generateHTML());
}


void handleStopAndSwitch() {
  stopAndSwitch = true;  // Set flag to true to switch loops
  server.send(200, "text/html", generateHTML());
  tft.fillScreen(TFT_BLACK);
}






void handleShowSundog() {

 server.send(200, "text/plain", "Showing Sundog Screen");
  
  // Clear the screen
  tft.fillScreen(TFT_BLACK); 

  // Display "Sundog" on the screen
  tft.setTextSize(3);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setSwapBytes(true);
  tft.pushImage(0, 0, 320, 170, sundogbackground);  // show sundogbackground
  tft.setSwapBytes(false);

   // show placeholder for tiny preview images
  tft.fillRect(4, 90, 45, 50, TFT_BLACK);  // Adjust to clear only the area where the image will be displayed


// show time
struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    tft.println("Failed to obtain time");
    return;
  }

  strftime(timeHour, 3, "%H", &timeinfo);
  strftime(timeMin, 3, "%M", &timeinfo);
  strftime(timeSec, 3, "%S", &timeinfo);
  strftime(day, 3, "%d", &timeinfo);
  strftime(month, 6, "%B", &timeinfo);
  strftime(year, 5, "%Y", &timeinfo);

  
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.drawString(String(timeHour) + ":" + String(timeMin), 55, 103);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.drawString(String(day) + " " + String(month) + " " + String(year), 55, 123);



  // Fetch the Sundog price data
  String sundogPrice;
  float sundogChange;
  if (fetchSundogData(sundogPrice, sundogChange)) {
    // Display the Sundog price and 5-minute change
    tft.setTextSize(4);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(" " + sundogPrice, 150, 135);

      tft.setTextSize(2);

      // Check if sundogChange is positive or negative
      if (sundogChange < 0) {
          tft.setTextColor(TFT_RED);
      } else {
          tft.setTextColor(TFT_GREEN);
      }

      // Display sundogChange with the chosen color
      tft.drawString("" + String(sundogChange) + "%", 200, 110);





        // Display title
    tft.setCursor(280, 110);
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.printf("USD");

    tft.setCursor(285, 100);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    tft.printf("PRICE");

    tft.setCursor(215, 100);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    tft.printf("SUNDOG 5m");

    displayImagesTiny();

      server.handleClient();  // Handle web server requests

      delay(25000);  // Display for 25 seconds
      handleShowSundog(); 


  } else {
    // If data fetching failed, show an error
    tft.setTextSize(2);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("Failed to fetch Sundog data", 50, 100);
  }

  delay(2000);  // Display for 2 seconds

  // Send a response to the web client
  server.send(200, "text/html", generateHTML());

  // Infinite loop to keep displaying the message
  while (true) {
    
delay(2500); // Refresh the price every 25 seconds or any other desired time
    // Optionally, you can check for a condition here to break the loop if needed

    // Fetch and display the Sundog price and 5m price change
    fetchSundogData(sundogPrice, sundogChange);  // Call your function to fetch Sundog price

    
  }
//server.handleClient();  // Handle web server requests
}

// Function to fetch Sundog price data
bool fetchSundogData(String& price, float& change) {
  HTTPClient http;
  http.begin(apiUrl);  // Use the correct URL for fetching Sundog data
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();

    // Allocate the JSON document
    DynamicJsonDocument doc(2048);  // Adjust size if needed

    // Deserialize the JSON data
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      return false;
    }

    // Extract the Sundog price and 5-minute change
    price = doc["pairs"][0]["priceUsd"].as<String>();  // Replace with the correct index for Sundog
    change = doc["pairs"][0]["priceChange"]["m5"].as<float>();

    return true;
    //server.handleClient();  // Handle web server requests
    handleShowSundog();
    
  } else {
    return false;
    //server.handleClient();  // Handle web server requests
    handleShowSundog();
    
    
  }
}







void handleShowBull() {

   server.send(200, "text/plain", "Showing Bull Screen");

  
  // Clear the screen
  tft.fillScreen(TFT_BLACK); 

  // Display "Bull" on the screen
  tft.setTextSize(3);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setSwapBytes(true);
  tft.pushImage(0, 0, 320, 170, bullbackground);  // show bullbackground
  tft.setSwapBytes(false);

   // show placeholder for tiny preview images
  tft.fillRect(4, 90, 45, 50, TFT_BLACK);  // Adjust to clear only the area where the image will be displayed


// show time
struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    tft.println("Failed to obtain time");
    return;
  }

  strftime(timeHour, 3, "%H", &timeinfo);
  strftime(timeMin, 3, "%M", &timeinfo);
  strftime(timeSec, 3, "%S", &timeinfo);
  strftime(day, 3, "%d", &timeinfo);
  strftime(month, 6, "%B", &timeinfo);
  strftime(year, 5, "%Y", &timeinfo);

  
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.drawString(String(timeHour) + ":" + String(timeMin), 55, 103);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.drawString(String(day) + " " + String(month) + " " + String(year), 55, 123);



  // Fetch the Bull price data
  String BullPrice;
  float BullChange;
  if (fetchBullData(BullPrice, BullChange)) {
    // Display the Bull price and 5-minute change
    tft.setTextSize(4);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(" " + BullPrice, 130, 135);

      tft.setTextSize(2);

      // Check if BullChange is positive or negative
      if (BullChange < 0) {
          tft.setTextColor(TFT_RED);
      } else {
          tft.setTextColor(TFT_GREEN);
      }

      // Display BullChange with the chosen color
      tft.drawString("" + String(BullChange) + "%", 200, 110);





        // Display title
    tft.setCursor(280, 110);
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.printf("USD");

    tft.setCursor(285, 100);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    tft.printf("PRICE");

    tft.setCursor(215, 100);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    tft.printf("Bull 5m");

    displayImagesTiny();
      server.handleClient();  // Handle web server requests
      delay(25000);  // Display for 25 seconds
      handleShowBull(); 


  } else {
    // If data fetching failed, show an error
    tft.setTextSize(2);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("Failed to fetch Bull data", 50, 100);
  }

  delay(2000);  // Display for 2 seconds

  // Send a response to the web client
  server.send(200, "text/html", generateHTML());

  // Infinite loop to keep displaying the message
  while (true) {
    
delay(2500); // Refresh the price every 25 seconds or any other desired time
    // Optionally, you can check for a condition here to break the loop if needed

    // Fetch and display the Bull price and 5m price change
    fetchBullData(BullPrice, BullChange);  // Call your function to fetch Bull price

    
  }
//server.handleClient();  // Handle web server requests
}

// Function to fetch Bull price data
bool fetchBullData(String& price, float& change) {
  HTTPClient http;
  http.begin(apiUrl);  // Use the correct URL for fetching Bull data
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();

    // Allocate the JSON document
    DynamicJsonDocument doc(2048);  // Adjust size if needed

    // Deserialize the JSON data
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      return false;
    }

    // Extract the Bull price and 5-minute change
    price = doc["pairs"][2]["priceUsd"].as<String>();  // Replace with the correct index for Bull
    change = doc["pairs"][2]["priceChange"]["m5"].as<float>();

    return true;
    //server.handleClient();  // Handle web server requests
    handleShowBull();
    
  } else {
    return false;
    //server.handleClient();  // Handle web server requests
    handleShowBull();
    
    
  }
}





// Function to display the time
void displayTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    tft.println("Failed to obtain time");
    return;
  }

  strftime(timeHour, 3, "%H", &timeinfo);
  strftime(timeMin, 3, "%M", &timeinfo);
  strftime(timeSec, 3, "%S", &timeinfo);
  strftime(day, 3, "%d", &timeinfo);
  strftime(month, 6, "%B", &timeinfo);
  strftime(year, 5, "%Y", &timeinfo);



  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.drawString(String(timeHour) + ":" + String(timeMin) + ":" + String(timeSec), 0, 35);
  tft.setTextSize(1);
  tft.setTextColor(TFT_LIGHTGREY);
  tft.drawString(String(day) + " " + String(month) + " " + String(year), 55, 35);


  //tft.fillRect(150, 0, 1, 200, TFT_VIOLET); // screen line

  // Display title
  tft.setCursor(0, 87);
  tft.setTextSize(1);
  tft.setTextColor(TFT_LIGHTGREY);
  tft.printf("COIN   PRICE    5m");

delay(1000);  
  fetchAndDisplayPrices();
delay(1000);   
  fetchAndDisplayPrice();
}

// Function to display images
// Function to display images
void displayImages() {
  for (currentIndex = 0; currentIndex < numImages; ++currentIndex) {
    // Fetch the image directly to memory
    if (getFileToMemory(imageUrls[currentIndex])) {
      //tft.setCursor(0, 50);
      //tft.println(ESP.getFreeHeap());

      delay(500);
      // Clear the area for the image
      tft.fillRect(150, 0, 400, 200, TFT_BLACK);

      delay(500);
      // Draw the image from memory
      drawImageFromMemory(150, 0);

      delay(250);
      tft.setSwapBytes(true);
      delay(250);
      tft.pushImage(273, 7, 38, 19, NEWmeme);  // Show logo
      delay(250);
      tft.setSwapBytes(false);
      delay(250);

      // You can add server handling or other tasks here if needed
      delay(250);
    } else {
      tft.println("Failed to download image");
      continue;
    }

    // Display each image for the duration of slideSpeed
    delay(slideSpeed);
    tft.fillRect(150, 0, 400, 200, TFT_BLACK);
    delay(250);
    server.handleClient();
  }
}




// Function to display images as a tiny preview
// Function to display images as a tiny preview
void displayImagesTiny() {
  for (currentIndex = 0; currentIndex < numImages; ++currentIndex) {
    // Fetch the image directly to memory
    if (getFileToMemory(imageUrls[currentIndex])) {
      // Clear the area for the tiny image preview
      tft.fillRect(4, 90, 45, 50, TFT_BLACK);  // Adjust to clear only the area where the image will be displayed

      // Scale the image for preview
      TJpgDec.setJpgScale(4);

      // Draw the image from memory
      drawImageFromMemory(4, 90);

    } else {
      tft.println("Failed to download image");
      continue;
    }

    // Display each image for 3 seconds
    delay(slideSpeed);
  }
}



// Function to display images at random locations
void displayImagesRandom() {
  for (currentIndex = 0; currentIndex < numImages; ++currentIndex) {
    // Fetch the image directly to memory
    if (getFileToMemory(imageUrls[currentIndex])) {
      // Generate random coordinates for the image
      int x = random(-20, 360 - 170);
      int y = random(-0, 170 - 170);

      // Draw the image from memory at random location
      drawImageFromMemory(x, y);

    } else {
      tft.println("Failed to download image");
      continue;
    }

    // Display each image for the specified slide speed
    delay(slideSpeed);
  }
}



void fetchAndDisplayPrice() {
  HTTPClient http;
  http.begin(apiUrlTRX);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();

    // Allocate the JSON document
    DynamicJsonDocument doc(2048);  // Increase size if needed based on the response size

    // Deserialize the JSON data
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      tft.println("Failed to parse JSON");
      tft.println(error.c_str());
      return;
    }

    // Extract the priceUsd value from the correct path
    String priceUsd = doc["pairs"][0]["priceUsd"].as<String>();

    // Extract the 5min change percentage from the priceChange object
    float priceChangem5 = doc["pairs"][0]["priceChange"]["m5"].as<float>();

    // Combine both values into one string
    String displayString = "TRX " + priceUsd + " " + String(priceChangem5) + "%";

    // Clear the previous text area
    //tft.fillRect(0, 60, 320, 30, TFT_BLACK);

    // Display the price in white
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("" + priceUsd, 30, 60);

    // Set the color based on the price change value
    if (priceChangem5 < 0) {
      tft.setTextColor(TFT_RED, TFT_BLACK);  // Set color to red for negative change
    } else {
      tft.setTextColor(TFT_GREEN, TFT_BLACK);  // Set color to green for positive/zero change
    }

    // Display the 1-hour change in the appropriate color
    tft.setTextSize(1);
    tft.drawString(" " + String(priceChangem5) + "%", 100, 70);  // Adjust position for alignment
    tft.setTextColor(TFT_WHITE, TFT_BLACK);                      // Set color to green for positive/zero change
    tft.drawString(" TRX", 100, 60);



  } else {
    tft.printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}


// Function to fetch and display prices with custom names
void fetchAndDisplayPrices() {


  tft.setSwapBytes(true);
  tft.pushImage(-1, 0, 142, 31, minimeme);  // display minimeme logo
  tft.setSwapBytes(false);

  //Display MEME placeholder text "Downloading new memes "

  tft.drawRect(150, 0, 200, 200, TFT_BLACK);
  tft.drawRect(160, 5, 150, 160, TFT_GREEN);
  tft.fillRect(170, 0, 130, 200, TFT_BLACK);   //vertical
  tft.fillRect(150, 16, 160, 140, TFT_BLACK);  // hotizontal
  tft.setCursor(200, 75);
  tft.setTextSize(1);
  tft.setTextColor(TFT_GREEN);
  tft.printf("DOWNLOADING");  // downloading new memes placeholder
  tft.setCursor(205, 85);
  tft.printf("NEW MEMES");
  // delay(50000);



  HTTPClient http;
  http.begin(apiUrl);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();

    // Allocate the JSON document
    DynamicJsonDocument doc(2048);  // Adjust size based on response size if needed

    // Deserialize the JSON data
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      tft.println("Failed to parse JSON");
      tft.println(error.c_str());
      return;
    }

    // Clear previous text area
    // tft.fillRect(0, 80, 320, 150, TFT_BLACK); // Adjust to clear only the area where the data will be displayed







    int yOffset = 100;  // Start y offset for the first coin
    int index = 0;      // Index for custom names
    for (JsonObject pair : doc["pairs"].as<JsonArray>()) {
      if (index >= numCustomNames) break;  // Prevent out-of-bounds access

      String customName = customCoinNames[index];
      String priceUsd = pair["priceUsd"].as<String>();
      float priceChangeM5 = pair["priceChange"]["m5"].as<float>();

      String displayString = customName + "" + priceUsd + " " + String(priceChangeM5) + "%";

      // Set text color based on price change
      if (priceChangeM5 < 0) {
        tft.setTextColor(TFT_RED, TFT_BLACK);  // Red for negative change
      } else {
        tft.setTextColor(TFT_GREEN, TFT_BLACK);  // Green for positive/zero change
      }

      tft.setCursor(0, yOffset);
      tft.setTextSize(1);
      tft.drawString(displayString, 0, yOffset);

      yOffset += 10;  // Adjust space between lines
      index++;
    }
  } else {
    tft.printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}







void setup() {
  Serial.begin(115200);
  tft.println("\n\nTesting TJpg_Decoder library with NTP time");




  // Initialize the TFT
  tft.begin();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(3);

  // The JPEG image can be scaled by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tft_output);

  tft.setSwapBytes(true);
  tft.pushImage(150, 75, 30, 30, trxtronlogo);  // show logo
  tft.setSwapBytes(false);



  tft.setSwapBytes(true);
  tft.pushImage(-1, 0, 142, 31, minimeme);
  tft.setSwapBytes(false);

  tft.println("miniMEME is starting...");

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    tft.print(".");
    delay(500);
  }
  tft.println("\nWiFi connected");

  tft.println("More settings: ");
  tft.println(WiFi.localIP());  // Print IP address


  delay(2000);

  // Configure time from NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  //Initialize Web Server
  server.on("/", handleRoot);                         // Serve the root page
  server.on("/setSpeed", HTTP_POST, handleSetSpeed);  // Handle speed setting
  server.on("/stopAndSwitch", handleStopAndSwitch);
  server.on("/showSundog", handleShowSundog);  // New handler registration
  server.on("/showBull", handleShowBull);  // New handler registration
  server.on("/home", reset);  // New handler registration
  server.begin();                            // Start the server
}

void reset() {
 server.send(200, "text/plain", "Back to home screen");
    delay(100);  // Small delay before restarting
    ESP.restart();  // Restart the ESP32 
}


void loop() {

server.handleClient();

  if (stopAndSwitch) {
    alternateLoop();  // Switch to the alternate loop
  } else {
    mainLoop();  // Continue with the main loop
  }
}






void mainLoop() {
  static unsigned long lastFetchTime = 0;
  unsigned long currentMillis = millis();






  displayTime();    // Display the time
  displayImages();  // Display the images in a loop

  // Fetch and display prices every 1 minute
  if (currentMillis - lastFetchTime >= 6000000) {  // 6000000 ms = 100 minute

  delay(1000);  // Small delay before restarting
    fetchAndDisplayPrice();
    delay(1000);  // Small delay before restarting
    fetchAndDisplayPrices();

    lastFetchTime = currentMillis;
  }
}



//loop for fullscreen collage mode
void alternateLoop() {

  displayImagesRandom();  // Display the images in a loop
}
