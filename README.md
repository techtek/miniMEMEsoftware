# Client and Server Software


![Alt text](https://techtek.github.io/miniMEMEwebsite/minimemelogo.png)


The Memes on Sunpump.meme are small pieces of art and a snapshot of our time and culture! With miniMEME, you can display them on a small inexpensive device to turn your cupboard into a mini meme art gallery, showcasing the latest and greatest Tron memes.

Works with ESP32-S3 single board computers

![Alt text](https://github.com/techtek/miniMEMEwebsite/blob/main/images/Screenshot2.jpg?raw=true)

Website: https://techtek.github.io/miniMEMEwebsite/


-------------------

Join the miniMEME, Tron IOT community on:

Telegram: https://t.me/+UQ2B4besKqw0NDY0

Discord: https://discord.gg/GyjvNMtJ5V


-----------------

Client Setup

1. Make sure your board is installed and working correctly by uploading any of the demo sketches from the demo folder. 
2. Download the miniMEME repository files and load the client folder "minimeme" into your arduino IDE software by opening the miniMEME.ino file. 
3. Make sure you have downloaded and installed these versions of the specific libararies from the Arduino IDE library manager    

Tjpg_Decoder 1.1.0

NTPCLIENT 3.2.1

SD 1.3.0

Adafruit SPIFlash 5.0.0

wifimanager 2.0.17

TFT_eSPI 2.5.43

ArduinoJson 7.2.0

Use these specific versions and dont update the libraries or it might result in errors.  


--------------------------------------------------------------------------

Server setup (optional)

Chrome extension to download new memes and prepare them into small JPG images that the board can handle
- Install Google chrome
- Load the miniMEME extension into the browser 
- Go to sunpump.meme and open the miniMEME extension and press start (if you have a old slow computer you will need set a higher refresh time in the interface (40 / 60 seconds)  
- The extension will refresh the page and download new memes to the Downloads folder 

Batch script to upload the downloaded and preapred jpg meme files to github
- Install git for windows 
- Initiate git to use the Downloads folder
- Run the batch script to push the downloaded and prepared meme files to github

(it is adviced to use the server side software in a Virtual machine or freshly installed Windows server because the Chrome Extension is limited to only download to the Downloads folder without a save as dialog. The Downloads folder is being used and synced and it is important to keep that in mind and to use it only for miniMEME)  
 
