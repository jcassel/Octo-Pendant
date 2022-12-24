# esp8266_OPRA
OctoPrint remote API pendant touch screen. An alternate touch UI for when you are at your printer but want to ensure constant control from OctoPrint. 

Uses a WeMos D1 mini (esp8266) and a [2.4" Touch TFT](https://www.aliexpress.us/item/3256803259687592.html). Both are inexpensive and easy to obtain.  I also designed a simple carrier board for connecting them together without doing manual wiring although it is possible to do it that way.  I designed a case for it as well. 

Currently there are 4 screens.  
**Main/Monitor**
- Using the top 3 buttons you can navigate to the other 3 screens. 
- In the middle is general information about the gcode model loaded by OctoPrint. 
- The lower 3 buttons are for control of the loaded modle; Start, Pause, and Stop.

![MainScreen](https://user-images.githubusercontent.com/12872876/209440841-b36259b7-ba4e-4c40-864f-83541c9c4043.jpg)

**Axis Control**
- The green left pointing arrow at the top takes you back to the main screen.
- Standard X,Y,Z Axis controls. You can also turn the motors on or off as well as the cooling fan.

![AxisControlScreen](https://user-images.githubusercontent.com/12872876/209441008-9cc742f9-9f5c-40e4-8073-92307a4f583d.jpg)

**Temperature Control**
- The green left pointing arrow at the top takes you back to the main screen.
- Control the temperature of the Build plate and the Extruder with ease. Select presets or use incremtal values to get it right where you want it. 

![TempControlScreen](https://user-images.githubusercontent.com/12872876/209441018-ec0bdd19-0305-450d-8d06-dc1ef7f1a4c8.jpg)

**Bed Leveling Utility**
- The green left pointing arrow at the top takes you back to the main screen.

- Controls for doing a manual bed level as well as sending a command set to do a mesh collection process that works with the [Bed Visualizer plugin for Octoprint](https://github.com/jneilliii/OctoPrint-BedLevelVisualizer). 
- Pressing one of the 5 dots on the bed will move manual move to that position on your build plat for paper style leveling. 
- Pressing the Running man on the left(white) will run the mesh collection process and wait. 
- Pressing the Running man on the right(green) will run the mesh collection process and reload the current modle and start it. (Some other options in the setup for this) 

![BedLevelUtilityScreen](https://user-images.githubusercontent.com/12872876/209441041-b2653ec4-34af-47b7-8b26-64b30f023b44.jpg)

