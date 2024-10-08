
![Alt text](https://raw.githubusercontent.com/techtek/miniMEMEwebsite/refs/heads/main/images/minimemeintro.jpg)


# miniMEME Client and Server Software



The Memes on Sunpump.meme are small pieces of art and a snapshot of our time and culture! With miniMEME, you can display them on a small inexpensive device to turn your cupboard into a mini meme art gallery, showcasing the latest and greatest Tron memes.

Works with ESP32-S3 single board computers

Website: https://techtek.github.io/miniMEMEwebsite/

![Alt text](https://d112y698adiu2z.cloudfront.net/photos/production/software_photos/003/069/363/datas/gallery.jpg)

![Alt text](https://d112y698adiu2z.cloudfront.net/photos/production/software_photos/003/069/365/datas/gallery.jpg)

![Alt text](https://d112y698adiu2z.cloudfront.net/photos/production/software_photos/003/069/364/datas/gallery.jpg)




-------------------

Join the miniMEME, Tron IOT community on:

Telegram: https://t.me/+UQ2B4besKqw0NDY0

Discord: https://discord.gg/GyjvNMtJ5V


-----------------


![Alt text](https://d112y698adiu2z.cloudfront.net/photos/production/software_photos/003/059/941/datas/gallery.jpg)

![Alt text](https://d112y698adiu2z.cloudfront.net/photos/production/software_photos/003/059/944/datas/gallery.jpg)




# Client Setup

1. Install the Arduino IDE and make sure your board is installed and working correctly by uploading any of the demo sketches from the demo folder. 
2. Download the miniMEME repository files and load the client folder "minimeme" into your arduino IDE software.
3. Make sure you have downloaded and installed these versions of the specific libararies from the Arduino IDE library manager:   

Tjpg_Decoder 1.1.0

NTPCLIENT 3.2.1

SD 1.3.0

Adafruit SPIFlash 5.0.0

wifimanager 2.0.17

TFT_eSPI 2.5.43

ArduinoJson 7.2.0

Use these specific versions and dont update the libraries or it might result in errors. 

4. Change your wifi credentials at line 30 

5. Upload to your board 

6. miniMEME is starting and should connect to your wifi network and the local IP address of the device will be displayed on the screen at start

7. You can view the device web interface by opening the shown local IP address

8. The server that prepares the images for the miniMEME device is run by me and might not always be online you can check the server side activity here: 
https://github.com/techtek/Downloads/commits/main/

If you want to run the server side application, you can continue with the next steps to setup the miniMEME server side software.



--------------------------------------------------------------------------

# Server setup (optional)

Chrome extension: to download new memes and prepare them into small JPG images that the board can handle
- Install Google chrome
- Load the miniMEME extension into the browser 
- Go to sunpump.meme and open the miniMEME extension and press start (if you have a old slow computer you will need set a higher refresh time in the interface (40 / 60 seconds)  
- The extension will refresh the page and download new memes to the Downloads folder 

Batch script: to upload the downloaded and preapred jpg meme files to github
- Install git for windows 
- Initiate git to use the Downloads folder
- Run the batch script to push the downloaded and prepared meme files to github

(it is adviced to use the server side software in a Virtual machine or freshly installed Windows server because the Chrome Extension is limited to only download to the Downloads folder without a save as dialog. The Downloads folder is the only folder that can be used by chrome extensions, and it is important to keep that in mind and to use it only for miniMEME)  
 
