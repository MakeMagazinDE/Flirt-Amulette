![GitHub Logo](http://www.heise.de/make/icons/make_logo.png)

Maker Media GmbH

***

# Flirt-Amulette mit ESP-Now
### Wir basteln tragbare Schmuckstücke, die über ESP-Now und WifiEspNow miteinander kommunizieren können. 

In dem Artikel der Ausgabe 7/24 der Make erklären wir, wie die Kommunikation mithilfe von ESP-Now und WifiEspNow funktioniert. Anschließend wird eine Bauanleitung für „Flirt-Amulette“ vorgestellt. Diese signalisieren mittels RGB-LEDs, ob und welche anderen Amulette sich in der Nähe befinden. Zudem sind die Amulette in der Lage, einfache Nachrichten auszutauschen, indem ein direkt auf die Amulette gelöteter Taster betätigt wird.

![Picture](https://github.com/MakeMagazinDE/Flirt-Amulette/blob/master/Bild%2003%20-%20GitHub.jpg)

Den vollständigen Artikel mit der Bauanleitung gibt es in der [Make-Ausgabe 7/24 ab Seite XXX](https://www.heise.de/select/make/2019/5/1571592996373573). Hier findet ihr die für die Amulette benötigten Sketches in drei verschiedene Ausführungen:

Im ersten (Make_ESPnow_minimal) werden die MAC-Adressen der ESPs (alle Typen der Serien ESP8266 und ESP32), auf denen der gleiche Sketch läuft, übertragen und durch unterschiedliche Blinkrhythmen der eingebauten LEDs auf dem Empfängermodul angezeigt.

Ein etwas erweiterter Sketch (Make_ESPnow_button) ermöglicht das Senden von Nachrichten zwischen den ESPs: Wenn ein (eingebauter oder aufgelöteter) Taster auf einem der ESPs gedrückt wird, wird auf den anderen ESPs eine Funktion aufgerufen, die eine serielle Meldung ausgibt und den Empfang per LED anzeigt.

Der dritte Sketch (Make_ESPnow) steuert unter Verwendung der WS2812FX-Bibliothek LEDs an. Wenn andere Module erkannt werden, dann pulsiert die LED langsam in einer Farbe, die von den letzten drei Bytes der MAC-Adresse des Partners abhängt.
