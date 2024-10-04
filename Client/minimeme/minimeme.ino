// miniMEME v0.73  
// Made by Techtek https://github.com/techtek
// join the Discord https://discord.gg/GyjvNMtJ5V


// Include the jpeg decoder library
#include <TJpg_Decoder.h>

// Include SPIFFS
#define FS_NO_GLOBALS
#include <FS.h>
#include "SPIFFS.h"

// Include WiFi and HTTP client
#include <WiFi.h>
#include <HTTPClient.h>

// Include the TFT library
#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library

// Include time-related libraries
#include "time.h"

// Include ArduinoJson library
#include <ArduinoJson.h>

// Change to your WiFi credentials <--------------<---------------
#define WIFI_SSID "YOUR-WIFI-SSID"
#define PASSWORD "YOUR-WIFI-Password"

#include <WebServer.h>
WebServer server(80); // Use port 80 for HTTP
int slideSpeed = 3000; // Default slide speed in milliseconds

// Webinterface
String generateHTML() {
  String html = "<html><body style='color:darkgrey;background-color:black;'>";  
  html += "<td><img src='https://techtek.github.io/miniMEMEwebsite/minimemelogo.png' style='width:366px;height:auto;'></td>";
  

  // Image table (2x2 layout)
  html += "<table border='0' cellspacing='3' cellpadding='3'>";
  html += "<tr>";
  html += "<td><img src='https://raw.githubusercontent.com/techtek/Downloads/main/image1.jpg' style='width:150px;height:150px;'></td>";
  html += "<td><img src='https://raw.githubusercontent.com/techtek/Downloads/main/image2.jpg' style='width:150px;height:150px;'></td>";
  html += "</tr>";
  html += "<tr>";
  html += "<td><img src='https://raw.githubusercontent.com/techtek/Downloads/main/image3.jpg' style='width:150px;height:150px;'></td>";
  html += "<td><img src='https://raw.githubusercontent.com/techtek/Downloads/main/image4.jpg' style='width:150px;height:150px;'></td>";
  html += "</tr>";
  html += "</table>";

  html += "<br></br>";

  html += "<h1>Settings:</h1>";

  

  // Slideshow speed settings form
  html += "<h2>Slideshow Speed</h2>";
  html += "<form action=\"/setSpeed\" method=\"POST\">";
  html += "Slide Speed (ms): <input type=\"text\" name=\"speed\" value=\"" + String(slideSpeed) + "\"><br>";
  
  html += "<br></br>";

  // TFT Rotation settings
  html += "<h2>Screen Rotation</h2>";
  html += "<input type=\"radio\" name=\"rotation\" value=\"1\"> Right<br>";
  html += "<input type=\"radio\" name=\"rotation\" value=\"3\" checked> Left<br>";  // Default rotation
  
  html += "<br></br>";
  
  html += "<input type=\"submit\" value=\"Save Settings\">";

  html += "</form>";

  html += "<br></br>";

  html += "<h2>Switch SlideShow</h2>";
  // Stop and Switch Button
  html += "<form action=\"/stopAndSwitch\" method=\"POST\">";
  html += "<input type=\"submit\" value=\"Full screen collage \">";
  html += "</form>";



  html += "</body></html>";

  return html;  
}


bool stopAndSwitch = false;  // Flag to switch between loops


// load interface images as byte array
#include "minimeme.h" //logo image as byte array
#include "NEWmeme.h" //NEW meme label image as byte array 
#include "trxtronlogo.h" //NEW meme label image as byte array 


// NTP settings
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;  // Your GMT offset
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
  "SUNDOG ",// Replace with the actual custom names
  "SUNCAT ",
  "BULL   ",
  "IVFUN  ",
  "MUNCAT ",
  "SUNWUK ",
  "CZC    "
};

const int numCustomNames = 7;


// API URL for TRX only
const char* apiUrlTRX = "https://api.dexscreener.com/latest/dex/pairs/tron/tfgdbuyp8xez44c76fin3bn3ss6jugouwj"; //TRX price api




// Function to render JPEG to the TFT
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  if (y >= tft.height()) return 0;
  tft.pushImage(x, y, w, h, bitmap);

    tft.setSwapBytes(true);
  tft.pushImage(273,7,38,19,NEWmeme); // show logo
  tft.setSwapBytes(false);
  return 1;
  

}

// Function to fetch image from URL and store in SPIFFS
bool getFile(const char *url, const char *filename) {
  HTTPClient http;
  http.begin(url);  // Specify the URL
  int httpCode = http.GET();  // Make the GET request

  if (httpCode == HTTP_CODE_OK) {  // Check for successful connection
    File file = SPIFFS.open(filename, FILE_WRITE);  // Open file in SPIFFS for writing
    if (!file) {
      tft.println("Failed to open file in SPIFFS");
      return false;
    }

    WiFiClient *stream = http.getStreamPtr();  // Get the pointer to the file stream
    uint8_t buff[128] = { 0 };
    int len = http.getSize();  // Get size of file
    int c = 0;

    while (http.connected() && (len > 0 || len == -1)) {
      size_t size = stream->available();
      if (size) {
        int bytesRead = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
        file.write(buff, bytesRead);
        c += bytesRead;
        if (len > 0) len -= bytesRead;
      }
      delay(1);
    }

    file.close();
    return true;
  } else {
    tft.printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
    return false;
  }

  http.end();  // Close connection
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

  fetchAndDisplayPrices();
  fetchAndDisplayPrice();
}

// Function to display images
void displayImages() {
  for (currentIndex = 0; currentIndex < numImages; ++currentIndex) {
    // Fetch the image file
    bool loaded_ok = getFile(imageUrls[currentIndex], imageFiles[currentIndex]);

    if (loaded_ok) {
      // Draw the image on the TFT at the right side of a 320px screen, and add a animations
      tft.fillRect(150, 0, 400, 200, TFT_BLACK); // Adjust to clear only the area where the image will be displayed
      TJpgDec.drawFsJpg(170, 0, imageFiles[currentIndex]);
      TJpgDec.drawFsJpg(160, 0, imageFiles[currentIndex]);   
      TJpgDec.drawFsJpg(152, 0, imageFiles[currentIndex]);
      TJpgDec.drawFsJpg(151, 0, imageFiles[currentIndex]);
      TJpgDec.drawFsJpg(150, 0, imageFiles[currentIndex]);
      
    } else {
      tft.println("Failed to download image");
      continue;
    }

       server.handleClient(); // Handle web server requests


    // Display each image for 3 seconds
    delay(slideSpeed);
    tft.fillRect(150, 0, 400, 200, TFT_BLACK);
  }
}


void displayImagesRandom() {
  for (currentIndex = 0; currentIndex < numImages; ++currentIndex) {
    // Fetch the image file
    bool loaded_ok = getFile(imageUrls[currentIndex], imageFiles[currentIndex]);

    if (loaded_ok) {
      // Generate random coordinates for the image
      int x = random(-20, 360 - 170); 
      int y = random(-0, 170 - 170); 


      // Draw the image at random location
      TJpgDec.drawFsJpg(x, y, imageFiles[currentIndex]);
      
    } else {
      tft.println("Failed to download image");
      continue;
    }

    server.handleClient(); // Handle web server requests

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
    DynamicJsonDocument doc(2048); // Increase size if needed based on the response size

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
    tft.setTextColor(TFT_WHITE, TFT_BLACK);  // Set color to green for positive/zero change
    tft.drawString(" TRX", 100, 60);



  } else {
    tft.printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}


// Function to fetch and display prices with custom names
void fetchAndDisplayPrices() {


    tft.setSwapBytes(true);
    tft.pushImage(-1,0,142,31,minimeme); // display minimeme logo
    tft.setSwapBytes(false);

      //Display MEME placeholder text "Downloading new memes " 

    tft.drawRect(150,0,200,200, TFT_BLACK);
    tft.drawRect(160,5,150,160, TFT_GREEN);
    tft.fillRect(170,0,130,200, TFT_BLACK);   //vertical
    tft.fillRect(150,16,160,140, TFT_BLACK);      // hotizontal
    tft.setCursor( 200, 75 );
    tft.setTextSize(1);
    tft.setTextColor(TFT_GREEN);
    tft.printf("DOWNLOADING");  // downloading new memes placeholder
    tft.setCursor( 205, 85 );
    tft.printf("NEW MEMES");
    // delay(50000);



  HTTPClient http;
  http.begin(apiUrl);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();

    // Allocate the JSON document
    DynamicJsonDocument doc(2048); // Adjust size based on response size if needed

    // Deserialize the JSON data
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      tft.println("Failed to parse JSON");
      tft.println(error.c_str());
      return;
    }

    // Clear previous text area
   // tft.fillRect(0, 80, 320, 150, TFT_BLACK); // Adjust to clear only the area where the data will be displayed







    int yOffset = 100; // Start y offset for the first coin
    int index = 0; // Index for custom names
    for (JsonObject pair : doc["pairs"].as<JsonArray>()) {
      if (index >= numCustomNames) break; // Prevent out-of-bounds access
      
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

      yOffset += 10; // Adjust space between lines
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
  

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    tft.println("SPIFFS initialization failed!");
    while (1) yield();
  }
  tft.println("SPIFFS initialization done.");

  // Initialize the TFT
  tft.begin();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(3);

  // The JPEG image can be scaled by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tft_output);

  tft.setSwapBytes(true);
  tft.pushImage(150,75,30,30,trxtronlogo); // show logo
  tft.setSwapBytes(false);


  
  tft.setSwapBytes(true);
  tft.pushImage(-1,0,142,31,minimeme);
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
  tft.println(WiFi.localIP()); // Print IP address
  

  delay(2000);

  // Configure time from NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

//Initialize Web Server
server.on("/", handleRoot); // Serve the root page
server.on("/setSpeed", HTTP_POST, handleSetSpeed); // Handle speed setting
server.on("/stopAndSwitch", handleStopAndSwitch);
server.begin(); // Start the server


}




void loop() {
  if (stopAndSwitch) {
    alternateLoop();  // Switch to the alternate loop
  } else {
    mainLoop();  // Continue with the main loop
  }
}






void mainLoop() {
    static unsigned long lastFetchTime = 0;
  unsigned long currentMillis = millis();






  displayTime();  // Display the time
  displayImages();  // Display the images in a loop

  // Fetch and display prices every 1 minute
  if (currentMillis - lastFetchTime >= 60000) { // 60000 ms = 1 minute
    fetchAndDisplayPrice();
    fetchAndDisplayPrices();
    
    lastFetchTime = currentMillis;
  }
}



//loop for fullscreen collage mode
void alternateLoop() {

 displayImagesRandom();  // Display the images in a loop

}

