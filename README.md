# Arbedclother

Arduino Bedside Clock with Thermometer.

An Arduino bedside clock with thermometer and auto backlight dimming.

---

* LCD (2 rows / 16 characters per row):
  * Displays temperature (from a thermistor). Refreshed every 60 seconds.
  * Displays current date and time (from an RTC module). Refreshed every second.
  * Backlight is automatically dimmed by reading light in the room with a photoresistor. The darker the room, the higher is the backlight intensity and the other way around.

---

## Example of what is displayed on the LCD

```text
+------------------+
| Wednesday 18 May |
| 21.49 C    16:32 |
+------------------+
```

## Bill Of Materials (BOM)

* Arduino Uno
* LCD1602
* DS3231 RTC
* Thermistor
* Photoresistor
* 1K resistor
* 10K resistor
* 3K3 resistor
