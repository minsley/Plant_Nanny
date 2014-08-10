Plant_Nanny
===========

An Arduino sketch for monitoring your plants!

It will read air temperature, humidity, light level and soil moisture. 

Built to work with these lovely doodads:

* [SainSmart ST7735R 1.8" TFT LCD](http://www.sainsmart.com/sainsmart-1-8-spi-lcd-module-with-microsd-led-backlight-for-arduino-mega-atmel-atmega.html)
* [DHT11 Temperature/Humidity sensor](http://smile.amazon.com/gp/product/B007YE0SB6/ref=oh_aui_detailpage_o03_s00?ie=UTF8&psc=1)
* [wRobot 553518 Light Sensor](http://smile.amazon.com/gp/product/B00AF278A8/ref=oh_aui_detailpage_o01_s00?ie=UTF8&psc=1)
* [Sunkee Soil Hygrometer](http://smile.amazon.com/gp/product/B00AYCNEKW/ref=oh_aui_detailpage_o02_s00?ie=UTF8&psc=1)

TODO (sorted by difficulty, ascending):

* Flash a message on the LCD when something needs adjustment (water me).
* Normalize the light and moisture values to a useful scale.
* Write historical values in 10, 30 or 60-minute intervals to micro-SD.
* Open/close the moisture sensor circuit at intervals to slow oxidation (and measure less frequently).
* Add a solenoid attached to a water bottle for self-watering.
