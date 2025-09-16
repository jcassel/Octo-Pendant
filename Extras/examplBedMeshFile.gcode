M140 S65 ; starting by heating the bed for nominal mesh accuracy
M117 Homing all axes ; send message to printer display
G28      ; home all axes
M420 S0  ; Turning off bed leveling while probing, if firmware is set
         ; to restore after G28
M117 Heating the bed ; send message to printer display
M190 S65 ; waiting until the bed is fully warmed up
M300 S1000 P500 ; chirp to indicate bed mesh levels is initializing
M117 Creating the bed mesh levels ; send message to printer display
M155 S30 ; reduce temperature reporting rate to reduce output pollution
@BEDLEVELVISUALIZER	; tell the plugin to watch for reported mesh
G29	   ; run bilinear probing
M155 S3  ; reset temperature reporting
M140 S0 ; cooling down the bed
M500 ; store mesh in EEPROM
M300 S440 P200 ; make calibration completed tones
M300 S660 P250
M300 S880 P300
M117 Bed mesh levels completed ; send message to printer display