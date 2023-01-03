# Octo Pendant
![SplashMD_v1 0 0 16](https://user-images.githubusercontent.com/12872876/210280350-4bd8bed7-bccd-4132-bae1-61645655429a.png)
An OctoPrint remote API pendant touch screen. An alternate touch UI for when you are at your printer but want to ensure constant control from OctoPrint. Also includes some utilities to make leveling your printer a little easier. This is really handy for anyone that runs a print farm and wants quick local control of a printer that might be getting run from a multi instance deploy of Octoprint.  It’s so much faster to just tap tap tap for a couple of things than to go to a terminal and access the right instance. One might also find it useful if you are running a headless printer. 

## Features:
* Connects to OctoPrint though API. 
* Displays running Job Status. 
* Allows control of running Job.
  * Can start, pause/resume, and stop loaded files.
* Can control Axis movement.
* Can control Tool and bed temperatures.
* Can run mesh leveling 
* Includes a utility for Manual bed leveling


Currently there are 5 TFT display screens and 3 web configuration screens. 
  TFT Screens include: Main/Monitor, Axis Control, Temperature Control, Leveling Utility, and Info.
  
  Web Configuration Screens include: Configuration Settings, Uploads (File Management), and Firmware Update (OTA)

## Parts

**Required**
* Wemos D1 Mini (4M) [Amazon](https://www.amazon.com/dp/B073CQVFLK/) or [AliExpress](https://www.aliexpress.us/item/3256803535819463.html) or [Tindie](https://www.tindie.com/stores/jcsgotthis/)
* 2.4" Touch TFT (ILI9341) Or equivalent [AliExpress](https://www.aliexpress.us/item/2251832829271342.html) or [Amazon](https://www.amazon.com/dp/B09XHRKFMM) or [Tindie](https://www.tindie.com/stores/jcsgotthis/)
* USB Cable for programing and power 

**nice to have**
* TFT to D1 carrier board [Tindie](https://www.tindie.com/stores/jcsgotthis/)

## Setup
Once you have compiled and downloaded the Firmware, you will be greeted by the touch calibration screen. Go ahead and calibrate it by touching the screen in each corner as indicated by the screen. 

Now you should see a screen with some text and dashes in a bright yellow on a black back ground. Each screen is composed of a background image and a button coordinate system for checking if you touched something interesting on the screen.

At the bottom of the screen you should see text that says "No File Loaded" Touch this. This will take you to the info screen. Its not got anything interesting on it yet but if you get to that screen, you will know things are generally going to work. 

Next you have a choice on how you load the data files. You can either use the "ESP Sketch data upload tool" or you can connect to the device over Wi-Fi and upload the needed files in to 
the spiffs.  

Using the Data upload tool is the quickest if you have it installed. But if you don’t have it installed, its not hard to just upload everything manually. 

To upload the files manually, (my preferred method) do the following. 

* Using your phone or any PC with a Wi-Fi connection, connect to the new Wi-Fi Access point with a name starting with “OctoPendant-New". 
    * Username: admin 
	* Password: Password1 (this is just the default starting point; yes you can change this in the interface.) 
* Once connected, open a browser and connect to this address:
	http://192.168.4.1/spiffs
	You will need to put in the username and password again. 
	
* On this screen you can select files to upload to the device. Best results are if you upload everything in the /data folder in the following order. Note: there are 10 files.
	* style.css
	* scripts.js
	* spiffs.htm (After this the web interface will become a little better)
	Try to refrain from testing hte other menu items, they are not uploaded yet and you will not get what you want.
	* ota.htm
	* mansettings.js
	* mansettings.htm
	* Home.bmp
	* control1.bmp
	* Level.bmp
	* Temps.bmp

* Now that everything is loaded, you can use the menu in the web interface to go to the Configuration screen. (First menu item at the top of the web page) 
* on this screen, much of it should be self-explanatory but I will go through it in detail. 
    * At the very top is a button to start a scan for available Wi-Fi signals. Push that button to start it. It will return a list of available SSIDs. 
    * Enter in the Wi-Fi SSID for the network your OctoPrint printer is on. 
    * Enter the Password for that SSID (yes, its clear text).
    * Unit Name: This is the network name or host name of the device as it would show up when registering for DNS or DHCP.
    * You can leave the next 2 at default.. They should just work as is, although I have seen good results with 2000 for delay.
    * The Access Password is the password to gain access to the web pages you are using. You can change it. If you forget it, you can change it through serial connection. 
    * HA Unique Id, is specifically for referencing the device in home assistant and even then it's value is only reference. Best practice is to make it match your Unit Name unless you know why you are changing it. 
    * Enable MQTT this will have the device connect to an MQTT broker and send status updates. *Note: This works and will send a general online status to your MQTT broker but I have not really fleshed out any real capabilities in this area yet.)*
    * MQTT Server: The IP Or Full DNS for your MQTT Broker. Cannot be blank if MQTT is enabled. 
    * MQTT UserName: Username for the connection to the MQTT Broker can be blank even if MQTT is enabled.
    * MQTT Password: Password for the connection to the MQTT Broker Can be blank even if MQTT is enabled.
    * MQTT Port: Must be an int value (0 is valid if MQTT is not enabled), typically 1883 
    * OctoPrint URL: The IP or URL for your OctoPrint instance. 
    * OctoPrint Port: The port for your OctoPrint instance. (5000 is the default)
    * OctoPrint APIKey: A key generated by OctoPrint. I strongly suggest you create a user and an API key just for this device. 
    * Enable Mesh Bed Leveling: Only enable this if you have followed the (Mesh Bed leveling guide in the Guides folder)
    * OctoPrint Level GCode File?: This is the path to the gcode file you will create when following the MeshBedLeveling Guide) 
    * Reload Model?: Is only available when Mesh bed leveling is enabled. If checked, and bed leveling is evoked using the green Running man button, if a model other than the "OctoPrint Level GCode file:" is loaded it will reload it after it completes the Mesh level process. 
    * Start Model?: Is is only available when Mesh bed leveling is enabled. If checked, and bed leveling is evoked using the green Running man button and the "Reload Model button is checked as well, OctoPendant will start the print process for the reloaded model. 
    * Bed Volume X in mm: Fill this in with your printers print volume in the X axis.
    * Bed Volume Y in mm: Fill this in with your printers print volume in the Y axis.
    * Bed Volume Offset in mm: This and the 2 other Volume settings are used for manual bed leveling Utility screen where they are used to calculate the locations for the 5 probe/test points are located. Offset is how far from the edge. 
    * Auto Zero X,Y,Z axis on level screen entry: Recommend that this is checked for machines that don’t do mesh leveling. It will ensure marlin knows where it is before you start a manual bed level process. If you do not Zero all, you could crash your hot end. 
    * Time Zone OffSet hrs: Negative or positive number for your time zone. See this [list](https://www.timeanddate.com/time/zones/) for offset values. Do not include the (+) for Positive values.
 
## TFT Screens

**Main/Monitor**
- Using the top 3 buttons you can navigate to 3 other screens. 
    - Axis Control
    - Leveling Util
    - Temperature control
- In the middle of the screen is general information about the gcode model loaded by OctoPrint. 
    -  Status:  General status of the OctoPrint instance (Printer too).
    -  File: Name of the loaded file,  if there is one.
    -  Uploaded: Date time when the loaded file was uploaded.
    -  Print time Est: Estimated duration hh:mm:ss for the loaded file. 
    -  Print Time: How long has the current print been printing hh:mm:ss
    -  Print Time Left: How much time does OctoPrint estimate is left to complete the current print.
- The lower 3 buttons are for control of the loaded model; Start, Pause, and Stop.
- At the bottom center there is a general status Text this will change dependent on the printers overall status as well as if you have given it a command from the above list. Additionally, If you tap this, it will take you to the info screen. 
![HomeScreenMD_v1 0 0 16](https://user-images.githubusercontent.com/12872876/210279805-fa2d745c-2cdb-44cc-9bdc-9d67fa618744.png)
***

**Axis Control**
- The green left pointing arrow at the top takes you back to the main screen.
- Standard X,Y,Z Axis controls. You can also turn the motors on or off as well as the cooling fan. 
- Home buttons work independently.
- The value buttons can be used to select a value for relative movement of the selected Axis motion button. 
    - OctoPrint will cache the commands so pressing an arrow several times will produce multiple moves. 
    - Note that not all printers will support the Fan on off action. Think Printers that only have one fan like the Kingroon KP3.
![AxisControlMD_v1 0 0 16](https://user-images.githubusercontent.com/12872876/210279821-51cb53c2-64e2-4d4d-9ac1-ad7057e71905.png)
***

**Temperature Control**
- The green left pointing arrow at the top takes you back to the main screen.
- Control the temperature of the Build plate and the Extruder with ease. Select presets or use incremental values to get it right where you want it. 
    - Top section of the screen is for controlling the Extruder (hot end) temperature.
        - At the top right of the section, the hot end temperatures are shown as [ actual / setting ]
        - Selecting a top value buttons like 1, 5, 10, 20 set the increment for the (+) and (-) Buttons.
        - Selecting bottom values buttons like 170,180,200,230 set the value selected into the set box. 
        - Pushing the Red Power button will turn off the heat to the hot end. 
        - Pushing the Green enter button will set the value selected in the center setting box. 
    - Bottom section of the screen is for controlling the build plate(print bed) temperature.
        - At the top right of the section, the Print bed temperatures are shown as  [ actual / setting ]
        - Selecting a top value buttons like 1, 5, 10, 20 set the increment for the (+) and (-) Buttons.
        - Selecting bottom values buttons like 50, 60, 70, 100 set the value selected into the set box. 
        - Pushing the Red Power button will turn off the heat to the print bed. 
        - Pushing the Green enter button will set the value selected in the center setting box. 
![TemperScreenMD_v1 0 0 16](https://user-images.githubusercontent.com/12872876/210279841-eb853578-52ca-445e-b4a8-7daec231781c.png)

***
**Bed Leveling Utility** (This screen is still in progress)
Controls for doing a manual bed level as well as sending a command set to do a mesh collection process that works with the [Bed Visualizer plugin for Octoprint](https://github.com/jneilliii/OctoPrint-BedLevelVisualizer). 
- The green left pointing arrow at the top takes you back to the main screen.
    - Settings in the Web interface have an effect on how this screen works. See the Setup section and Guides for level Utility usage.  
- Pressing the Home Button on this screen will home X,Y,Z axis *(see web interface for settings related to homing settings)*
- Pressing one of the 5 dots on the bed will send the print head to that position on your build plat for paper style leveling. Z height will be absolute zero. *(Be sure you have homed your machine prior to using this screen.)*  
- Pressing the Running man on the left (white) will run the mesh collection process and wait. *(see web interface for settings related to mesh leveling settings)*
- Pressing the Running man on the right (green) will run the mesh collection process and reload the current model and start it. *(see web interface for settings related to mesh leveling settings)*
![LevelMD_v1 0 0 16](https://user-images.githubusercontent.com/12872876/210279854-7ed7ca54-84ce-4db0-a5ac-ab7e248c0086.png)

***
**Pendant Info Screen**
This screen shows Connection details for the Pendant. Also has a reset button at the bottom. I plan on adding more here as it makes sense. 
![InfoScreenMD_v1 0 0 16](https://user-images.githubusercontent.com/12872876/210279866-fea4a8f1-08ca-41b5-84b9-40f68bb3620d.png)
***

## Carrier Board
I created a Carrier board for connecting the D1 mini to the 2.4" TFT(Touch) board. Note that the TFT has an SD Card slot on it but it is not wired up. Yes it looks like it is.. The posts are there for support. You can solder these boards together and that saves a lot of space but be careful, the SD card cover is right over 2 of the pins of the D1 mini. 

grbl files are available as part of the repo. Also boards are available from my [Tindie](https://www.tindie.com/stores/jcsgotthis/) Store along with other related items.

D1 mini mounted on carrier*(Note that I have a small Cap to help stabilize power for the board. It’s not really needed.):
![TFT-esp8266CarrierBoard_D1](https://user-images.githubusercontent.com/12872876/209448537-5eea7489-23f2-46bc-893f-cf334df4bc69.png)

Carrier without D1 mini:
![TFT-esp8266CarrierBoard_Back](https://user-images.githubusercontent.com/12872876/209448541-1d713a3f-cb3d-45a9-9385-324ae081ee6e.png)


TFT mount side:
![TFT-esp8266CarrierBoard_Front](https://user-images.githubusercontent.com/12872876/209448540-9da112db-8b78-45a0-8c82-8d6a644b7c12.png)
