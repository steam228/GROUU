# JOURNAL


## New mainSoilProbe board design on REV1

### This board includes:
DHT22 (Humidity and Temperature - Near plants Control);
1 x 3 male header for DS18B20 (encapsulated version for Soil Temperature);
2 x 2 male header connections for Moisture 10k with Two probes (one for each probe requiring calibration - see calib. instructions);
1 led (no usage yet, mostly for debug purposes);
1 ftdi set headers;
1 voltage converter 12V - 3,3V;
1 x 2 screw headers to add solar panel + battery (maybe connect solar battery pack now).

### refs:
#### solarpower
https://www.aki.pt/electricidade/producao-energia-renovavel/solar-eolico-energia-propia/acessorios-de-energias-renovaveis/bateria/controladores/painelmanutencaobaterias-p10533.aspx#info
https://www.aki.pt/electricidade/producao-energia-renovavel/solar-eolico-energia-propia/kits-solares/kitbombadeaguamicrosolar-p49581.aspx
https://mauser.pt/catalog/product_info.php?products_id=82391

Main Probe

#### Multiple Analogs -

https://www.instructables.com/id/Multiple-Analog-Inputs-on-Only-One-Analoge-Pin/

#### ESP12 + DHT22 -

https://cityos.io/tutorial/2006/ESP12-and-DHT22

#### ESP12  + temp

https://iot-playground.com/blog/2-uncategorised/41-esp8266-ds18b20-temperature-sensor-arduino-ide

#### Voltage Regulator

http://www.ti.com/lit/ds/symlink/lm3480.pdf

#### use FTDI on ESP12

https://www.hackster.io/harshmangukiya/program-esp8266-esp-12e-with-arduino-using-ftdi-cable-2310c9
