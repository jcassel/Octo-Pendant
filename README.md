# Octo Pendant
An OctoPrint remote API pendant touch screen. An alternate touch UI for when you are at your printer but want to ensure constant control from OctoPrint. 

Uses a WeMos D1 mini (esp8266) and a [2.4" Touch TFT](https://www.aliexpress.us/item/3256803259687592.html). Both are inexpensive and easy to obtain.  I also designed a simple carrier board for connecting them together without doing manual wiring although it is possible to do it that way.  I designed a case for it as well. 

Currently there are 5 TFT display screens and 3 web configuration screens . 
  TFT Screens include: Main/Monitor, Axis Control, Temperature Control, Leveling Utility, and Info.
  
  Web Configuration Screens include: Configuration Settings, Uploads(File Management), and Firmware Update(OTA)

*TFT Screens*

**Main/Monitor**
- Using the top 3 buttons you can navigate to the other 3 screens. 
- In the middle is general information about the gcode model loaded by OctoPrint. 
- The lower 3 buttons are for control of the loaded model; Start, Pause, and Stop.
- At the bottom center there is a general status Text... If you tap this, it will take you to the info screen. 

![HomeScreenMD_v1 0 0 16](https://user-images.githubusercontent.com/12872876/210279805-fa2d745c-2cdb-44cc-9bdc-9d67fa618744.png)


**Axis Control**
- The green left pointing arrow at the top takes you back to the main screen.
- Standard X,Y,Z Axis controls. You can also turn the motors on or off as well as the cooling fan.

![AxisControlMD_v1 0 0 16](https://user-images.githubusercontent.com/12872876/210279821-51cb53c2-64e2-4d4d-9ac1-ad7057e71905.png)

**Temperature Control**
- The green left pointing arrow at the top takes you back to the main screen.
- Control the temperature of the Build plate and the Extruder with ease. Select presets or use incremtal values to get it right where you want it. 

![TemperScreenMD_v1 0 0 16](https://user-images.githubusercontent.com/12872876/210279841-eb853578-52ca-445e-b4a8-7daec231781c.png)

**Bed Leveling Utility**
- The green left pointing arrow at the top takes you back to the main screen.

- Controls for doing a manual bed level as well as sending a command set to do a mesh collection process that works with the [Bed Visualizer plugin for Octoprint](https://github.com/jneilliii/OctoPrint-BedLevelVisualizer). 
- Pressing one of the 5 dots on the bed will move manual move to that position on your build plat for paper style leveling. 
- Pressing the Running man on the left(white) will run the mesh collection process and wait. 
- Pressing the Running man on the right(green) will run the mesh collection process and reload the current modle and start it. (Some other options in the setup for this) 

![LevelMD_v1 0 0 16](https://user-images.githubusercontent.com/12872876/210279854-7ed7ca54-84ce-4db0-a5ac-ab7e248c0086.png)


**Pendant Info**
This screen shows Connection details for the Pendand. Also has a reboot button at the bottom. 
![InfoScreenMD_v1 0 0 16](https://user-images.githubusercontent.com/12872876/210279866-fea4a8f1-08ca-41b5-84b9-40f68bb3620d.png)


**Carrier Board**

![TFT-esp8266CarrierBoard_D1](https://user-images.githubusercontent.com/12872876/209448537-5eea7489-23f2-46bc-893f-cf334df4bc69.png)
![TFT-esp8266CarrierBoard_Front](https://user-images.githubusercontent.com/12872876/209448540-9da112db-8b78-45a0-8c82-8d6a644b7c12.png)
![TFT-esp8266CarrierBoard_Back](https://user-images.githubusercontent.com/12872876/209448541-1d713a3f-cb3d-45a9-9385-324ae081ee6e.png)
