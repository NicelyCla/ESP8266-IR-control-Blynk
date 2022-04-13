# ESP8266-IR-control-Blynk
I used an esp8266 to control the lights and the vacuum cleaner (both are not natively home automation) in a domotic way via an app.<br />
The app was created by old version of Blynk, I recommend using the old version while it lasts, if you are lazy like me.<br />
Control takes place via infrared. Basically, a simple wireless IR remote control.<br />
It is necessary to clone the infrared signal, I used the TSOP31238 sensor to acquire the signal.<br />
The wiring diagram for this project is quite intuitive, just drive one or more IR diodes in the way you find most comfortable.<br />
Using this technique it's possible to control in a domotic way many objects that are not natively domotic.<br />
Clearly a necessary and sufficient requirement is that the object could be controlled via an infrared remote control.
