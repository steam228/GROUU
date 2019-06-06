---
layout: default
title: GROUU BOM PT_PT wip
---




# Set V0 GROUU
- - - -


## 1. Sonda de Solo - *Soil Probe*
- - - -
*Um conjunto de componentes formam um sensor wireless de medição de humidade e temperatura do solo. Os dados enviados por este sensor possibilitam tomada de decisão no que toca a opções de rega, entre outras informações importantes para o desenvolvimento das Plantas.*
*Dividido em dois componentes ligados por um fio, o primeiro será o corpo principal e o segundo apenas o encapsulamento de dois eletrodos adicionais, que quando ligados por fio ao corpo principal permitem a avaliação de humidade do solo, a uma profundidade superior.*


##### Componente Principal (corpo) contém:
- 2 electrodos;
- 1 Microcontrolador + Chip Wireless (dependendo do protocolo de comunicação pode assumir diferentes formas, neste caso apenas um ESP12)
![](Set%20V0%20GROUU/00CDE53B-2D0E-4704-A9E1-9CB0C4399E5F%202.png)
![](Set%20V0%20GROUU/8B35F060-8294-48BE-A186-7AB80FA3CB3D%202.png)

- Um termometro digital:
	- Com encapsulamento pp: [1pcs Direct waterproof DS18B20 digital temperature sensor (probe) a large number of original spot can be customised in Sensors from Electronic Components & Supplies on Aliexpress.com |
Alibaba Group](https://www.aliexpress.com/item/1pcs-Direct-waterproof-DS18B20-digital-temperature-sensor-probe-a-large-number-of-original-spot-can-be/32675444739.html?spm=2114.search0104.3.6.6ccf3bb753cbjV&ws_ab_test=searchweb0_0,searchweb201602_1_10065_10068_319_317_10696_453_10084_454_10083_433_10618_431_10304_10307_10820_10821_10301_537_536_10902_10059_10884_10887_321_322_10103,searchweb201603_16,ppcSwitch_0&algo_expid=053b9263-dde6-4dbb-8d99-83143a31b1db-0&algo_pvid=053b9263-dde6-4dbb-8d99-83143a31b1db) ou;
![](Set%20V0%20GROUU/Screenshot_2019-01-30%20US%20$1%2006%208%25%20OFF%201pcs%20Direct%20waterproof%20DS18B20%20digital%20temperature%20sensor%20(probe)%20a%20large%20number%20of%20o...%202.png)
	- integrar no desenho utilizando apenas componente: [Free Shipping 10pcs DALLAS DS18B20 18B20 TO 92 IC CHIP Thermometer Temperature Sensor-in Sensors from Electronic Components & Supplies on Aliexpress.com | Alibaba Group](https://www.aliexpress.com/item/Free-Shipping-10pcs-DALLAS-DS18B20-18B20-18S20-TO-92-IC-CHIP-Thermometer-Temperature-Sensor/32523899337.html?spm=2114.search0104.3.15.6ccf3bb753cbjV&ws_ab_test=searchweb0_0%2Csearchweb201602_1_10065_10068_319_317_10696_453_10084_454_10083_433_10618_431_10304_10307_10820_10821_10301_537_536_10902_10059_10884_10887_321_322_10103%2Csearchweb201603_16%2CppcSwitch_0&algo_pvid=053b9263-dde6-4dbb-8d99-83143a31b1db&algo_expid=053b9263-dde6-4dbb-8d99-83143a31b1db-5)*
![](Set%20V0%20GROUU/Screenshot_2019-01-30%20US%20$5%208%20Free%20Shipping%2010pcs%20DALLAS%20DS18B20%2018B20%20TO%2092%20IC%20CHIP%20Thermometer%20Temperature%20Sensor-in%20Sens...%202.png)

- Alguns componentes complementares: Resistências, condensadores e afins que podem compor a forma da restante placa de muitas formas diferentes, fazendo com que a maior condicionante seja o pp ESP12E;

##### Componente Secundária apenas com dois eletrodos ligados por fio à componente principal. esta é opcional e permite a mediação da humidade do solo a maior profundidade.

![](Set%20V0%20GROUU/5D49B827-73D2-4A7E-9DD4-F1709312F5A5%202.png)
*Exemplos de outros formatos e configurações de Soil Moisture Sensors*
- - - -



## 2. “Encaminhador de Água” - *Water Router 1/4*
- - - -
*Serve para direccionar a água dividindo-a de um para 4. Assim, podemos juntar quantos quisermos em serie podendo desta forma aumentar a distribuição da água por mais zonas de rega.*

- Placa de Controle com:
	- ESP12E;
	- Alguns componentes complementares: Resistências, condensadores e afins que podem compor a forma da restante placa de muitas formas diferentes, fazendo com que a maior condicionante seja o pp ESP12E;
- Solenoid Valves: 1 para 4 12V NC (normally closed) [1Pcs DC 12V DN20 G3/4” 1 in 4 out Plastic Electromagnetic Valve Normally Closed Water Drain Valve-in Valve from Home Improvement on Aliexpress.com | Alibaba Group](https://www.aliexpress.com/item/1Pcs-DC-12V-DN20-G3-4-1-in-4-out-Plastic-Electromagnetic-Valve-Normally-Closed-Water/32897483874.html?spm=2114.search0104.3.232.715c78f3tdeNwy&ws_ab_test=searchweb0_0,searchweb201602_1_10065_10068_319_317_10696_453_10084_454_10083_433_10618_431_10304_10307_10820_10821_10301_537_536_10902_10059_10884_10887_321_322_10103,searchweb201603_16,ppcSwitch_0&algo_expid=76eabde3-c4e3-4ab8-a5b5-27f0efe548b0-33&algo_pvid=76eabde3-c4e3-4ab8-a5b5-27f0efe548b0)
![](Set%20V0%20GROUU/Screenshot_2019-01-30%20US%20$16%2017%2037%25%20OFF%201Pcs%20DC%2012V%20DN20%20G3%204%201%20in%204%20out%20Plastic%20Electromagnetic%20Valve%20Normally%20Closed%20Wate...%202.png)
- - - -



## 3. Sensor de Nível da Água (Para adaptar a qualquer tanque de água) - *Water Level Sensor - Smart Tank*
- - - -
*Pretende-se a criação de uma unidade de sensor que possa ser adaptada à medição do volume existente em qualquer tanque. Bastará instalar e calibrar.*

- Placa de Controle com:
	- ESP12E;
	- Alguns componentes complementares: Resistências, condensadores e afins que podem compor a forma da restante placa de muitas formas diferentes, fazendo com que a maior condicionante seja o pp ESP12E;
- Sensor de nível de água pode ser conseguido de diferentes formas: (explos)
	- aplicar diferentes interruptores ligados por fio:[100V Liquid Water Level Sensor Horizontal Float Switch For Aquariums Fish Tank-in Flow Sensors from Tools on Aliexpress.com | Alibaba Group](https://www.aliexpress.com/item/100V-Liquid-Water-Level-Sensor-Horizontal-Float-Switch-For-Aquariums-Fish-Tank/32947871632.html?spm=2114.search0104.3.9.24f07dd04O1nJO&ws_ab_test=searchweb0_0,searchweb201602_1_10065_10068_319_317_10696_453_10084_454_10083_433_10618_431_10304_10307_10820_10821_10301_537_536_10902_10059_10884_10887_321_322_10103,searchweb201603_6,ppcSwitch_0&algo_expid=cd4f33f5-30f1-44e8-972c-3e771defacdd-1&algo_pvid=cd4f33f5-30f1-44e8-972c-3e771defacdd)
![](Set%20V0%20GROUU/Screenshot_2019-01-30%20US%20$1%2011%2015%25%20OFF%20100V%20Liquid%20Water%20Level%20Sensor%20Horizontal%20Float%20Switch%20For%20Aquariums%20Fish%20Tank-in%20Fl...%202.png)

	- Usar um sensor InfraRed no topo: [Newest 15mA 5V Optical Infrared Water Liquid Level Sensor Liquid Water Level Control Switch Favorable Level Sensors-in Sensors from Electronic Components & Supplies on Aliexpress.com | Alibaba Group](https://www.aliexpress.com/item/Newest-15mA-5V-Optical-Infrared-Water-Liquid-Level-Sensor-Liquid-Water-Level-Control-Switch-Favorable-Level/32748475170.html?spm=a2g0s.9042311.0.0.27424c4dZKJAXZ)
![](Set%20V0%20GROUU/Screenshot_2019-01-30%20US%20$2%2074%2010%25%20OFF%20Newest%2015mA%205V%20Optical%20Infrared%20Water%20Liquid%20Level%20Sensor%20Liquid%20Water%20Level%20Control...%202.png)
- - - -



## 4. Controlador de Bomba de Água - *Water Motor Controller (to fit most 12V Solar water pumps)*
- - - -

*Trata-se de um controlador simples de motor, a colocar como interruptor na alimentação do Motor*

- Placa de Controle com:
	- ESP12E;
	- Alguns componentes complementares: Resistências, condensadores e afins que podem compor a forma da restante placa de muitas formas diferentes, fazendo com que a maior condicionante seja o pp ESP12E
- - - -



## 5. Bomba Doseadora Peristáltica - *Peristaltic Pump*
- - - -
*É uma bomba de pequenas dimensões que faz uma dosagem mais fina e precisa. Pode ser usado para regar um vaso ou para dosear outras substancias liquidas na água. Por exemplo um corrector de pH ou um fertilizante.*

- Placa de Controle com:
	- ESP12E;
	- Alguns componentes complementares: Resistências, condensadores e afins que podem compor a forma da restante placa de muitas formas diferentes, fazendo com que a maior condicionante seja o pp ESP12E
- Sistema de rolos ou elemento excêntrico que ao rodar pressione uma mangueira de silicone do tipo:
- NEMA14 ￼
![](Set%20V0%20GROUU/MotorSide%202.jpg)
![](Set%20V0%20GROUU/Screenshot_2019-02-02%20peristaltic%20pump%20stepper%20at%20DuckDuckGo.jpg)
*Exemplos de outros formatos e configurações de bombas peristalticas*

![](Set%20V0%20GROUU/Peristaltic_pump.gif)
*Como Funciona uma Bomba Peristática*
- - - -


￼
## 6. Carregador de Indução - *(Solar) Inductive Charger*
- - - -

- Placa de Controle com:
	- ESP12E;
	- Alguns componentes complementares: Resistências, condensadores e afins que podem compor a forma da restante placa de muitas formas diferentes, fazendo com que a maior condicionante seja o pp ESP12E
- Uma Bateria de Back-up;
- **Alimentação:**
	- Pode estar ligado à corrente eléctrica 220V e aí terá que ter um conversor ou transformador, tipo carregador de telemóvel;
	- Possui painéis solares: [198E 4W 5.5V Polysilicon Solar Panel Cell Sunlight Charger Road Lamps Outdoors  | eBay](https://www.ebay.com/itm/198E-4W-5-5V-Polysilicon-Solar-Panel-Cell-Sunlight-Charger-Road-Lamps-Outdoors-/273681207899?hash=item3fb8abd25b) *Specification: Colour: Black Material: Polysilicon Length: 18.5cm Width: 7.4cm Thickness: 0.2 Maximum power: 4 (W) Operating current: 720 (A) Operating voltage: 5.5 (V) Working temperature: -20 ℃ - + 85 ℃ Mainly used in solar power road lights, garden lamps, uninterruptible power supply, solar mobile power gifts, etc.*
![](Set%20V0%20GROUU/s-l1600.jpg)
![](Set%20V0%20GROUU/158244_3.jpg)
- - - -



## 7. Núcleo - *Core*
- - - -
*Apenas um computador ligado à rede por Wireless*

- Raspberry Pi Zero W alimentado por transformador ou com bateria pp.
![](Set%20V0%20GROUU/Pi-Zero-W-Tilt-1-1620x1080.jpg)
(modelo 3D no GrabCAD) [3D CAD Model Collection | GrabCAD Community Library](https://grabcad.com/library/raspberry-pi-zero-w-board-1)
![](Set%20V0%20GROUU/Screenshot%202019-02-02%20at%2019.34.35.png)
- - - -



## Elementos Comuns a todos os módulos excepto Carregador.
- - - -

- 1 Inductive Charger - apenas o lado que recebe a carga:
![](Set%20V0%20GROUU/C57A9D53-3890-4C2B-9CDF-74C4127D9533%202.png)
- 1 Bateria Li-On ou Li-Po 3,7V
![](Set%20V0%20GROUU/Screenshot_2019-02-02%203,7V%20lipo%20battery%20at%20DuckDuckGo.jpg)





#phd #grouu
