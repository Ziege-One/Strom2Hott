*Strom Sensor für HoTT Telemetrie Mit Arduino Mini Pro*

*Beschreibung*

Strom2HoTT Sensor kann Strom und Spannung an Zwei Analogen Eingängen (A0/1) Messen und via HoTT Telemetrie auf dem Sender bringen.

*Funkionen:*

- Kapazitätsmessung Spannung und Strom

- Kapazitätsalarm und Spannungsalarm (über HoTTmenu am Sender einstellbar) 

- Online Kalibrierung von Nullpunkt und Messspanne (über HoTTmenu am Sender)

- Zusatzanzeigen Interne Spannung und Temperatur vom Arduino Mini Pro

- Geplante Funktion: Max Strom Min/Max Spannung. 

*Stückliste:*

- Arduino Pro Mini 328 - 5V/16MHz oder kompatibel.

- Spannungsteiler für Telemetrieeingang am HoTT-Empfänger 1.8 KOhm und 3.3 KOhm Widerstand.

- Servokabel für HoTT Eingang am Empfänger

- Spannungssensor z.B. Spannungsteiler mit 18 KOhm und 47 KOhm Widerstand. (bis ca 18V)

- Stromsensor z.B Amp Stomsensor oder ACS758LCB HAL-Stromsensor 50A 100A

*Schaltplan*

![alt tag](http://strom2hott.googlecode.com/svn/wiki/Strom2HoTT.jpg)

*Seiten*

![alt tag](http://strom2hott.googlecode.com/svn/wiki/Strom2HoTT_Seite_1.jpg)

Hier können die Alarme ein bzw aus geschaltet werden
Die Alarmwerte für min Spannung und Max Verbrauchte Kapazität eingestellt werden.
Das Interval der Alarmierung alle X Sekunden

![alt tag](http://strom2hott.googlecode.com/svn/wiki/Strom2HoTT_Seite_2.jpg)

Das Kalibriermenü (Analogeingänge 0-5V sind 0-1023 Digit => 1 Digit = 4,88mV)
OFF -> Nullpunkt hier können die Digits für den Nullpunkt eingestellt werden ( 0 = 0mV; 1 Digit = 4,88mV usw)
COEF -> Ist der Messbereich Strom bzw Spannung pro Digit in /10mV ( 485 = 48,5mV pro Digit)

Code baut auf die Versionen von John_lenfr auf *Link http://johnlenfr.1s.fr/index.php?option=com_content&view=category&layout=blog&id=59&Itemid=110
Danke für die Vorarbeit!!!
