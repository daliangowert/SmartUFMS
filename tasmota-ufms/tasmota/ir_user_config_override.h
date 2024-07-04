/*
  user_config_override.h - user configuration overrides my_user_config.h for Tasmota

  Copyright (C) 2021  Theo Arends

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _USER_CONFIG_OVERRIDE_H_
#define _USER_CONFIG_OVERRIDE_H_

/*****************************************************************************************************\
 * USAGE:
 *   To modify the stock configuration without changing the my_user_config.h file:
 *   (1) copy this file to "user_config_override.h" (It will be ignored by Git)
 *   (2) define your own settings below
 *
 ******************************************************************************************************
 * ATTENTION:
 *   - Changes to SECTION1 PARAMETER defines will only override flash settings if you change define CFG_HOLDER.
 *   - Expect compiler warnings when no ifdef/undef/endif sequence is used.
 *   - You still need to update my_user_config.h for major define USE_MQTT_TLS.
 *   - All parameters can be persistent changed online using commands via MQTT, WebConsole or Serial.
\*****************************************************************************************************/

// -- Module configuration -----------------------
#ifdef FALLBACK_MODULE
#undef FALLBACK_MODULE
#endif
#define FALLBACK_MODULE SONOFF_POW_R2

// Tasmota-IrTemperatura Template
//#define USER_TEMPLATE "{\"NAME\":\"Tasmota-IrTemperatura\",\"GPIO\":[32,608,640,1056,1,0,0,0,224,320,1,0,0,0],\"FLAG\":0,\"BASE\":1}"

// -- Timezone ------------------------------------
#ifdef APP_TIMEZONE
#undef APP_TIMEZONE
#endif
#define APP_TIMEZONE -4

// -- LED_STATUS ----------------------------------
#ifdef APP_LEDSTATE
#undef APP_LEDSTATE
#endif
#define APP_LEDSTATE LED_POWER

// -- Setup your own Wifi settings  ---------------
#undef STA_SSID1
 #define STA_SSID1         "m2m-ufms"             // [Ssid1] Wifi SSID

#undef STA_PASS1
 #define STA_PASS1         "XXXXXX"     // [Password1] Wifi password

// -- Setup your own MQTT settings  ---------------
#undef MQTT_HOST
#define MQTT_HOST "smartmqtt.ledes.net" // [MqttHost]

#undef MQTT_USER
#define MQTT_USER "XXXXXX" // [MqttUser] Optional user

#undef MQTT_PASS
#define MQTT_PASS "XXXXXC" // [MqttPassword] Optional password

// -- Habilitar suporte a dispositivos I2C -----------
#ifdef USE_I2C
#define USE_BMP          // Sensor de pressão e temperatura
#undef USE_VEML6070_RSET // VEML6070, Rset in Ohm used on PCB board, default 270K = 270000ohm, range for this sensor: 220K ... 1Meg
#undef USE_VEML6070_SHOW_RAW
#undef USE_DISPLAY_MODES1TO5 // Enable display mode 1 to 5 in addition to mode 0
#undef USE_DISPLAY_LCD       // [DisplayModel 1] [I2cDriver3] Enable Lcd display (I2C addresses 0x27 and 0x3F) (+6k code)
#undef USE_DISPLAY_SSD1306   // [DisplayModel 2] [I2cDriver4] Enable SSD1306 Oled 128x64 display (I2C addresses 0x3C and 0x3D) (+16k code)
#undef USE_DISPLAY_MATRIX    // [DisplayModel 3] [I2cDriver5] Enable 8x8 Matrix display (I2C adresseses see below) (+11k code)
#undef MTX_ADDRESS1          // [DisplayAddress1] I2C address of first 8x8 matrix module
#undef MTX_ADDRESS2          // [DisplayAddress2] I2C address of second 8x8 matrix module
#undef MTX_ADDRESS3          // [DisplayAddress3] I2C address of third 8x8 matrix module
#undef MTX_ADDRESS4          // [DisplayAddress4] I2C address of fourth 8x8 matrix module
#undef MTX_ADDRESS5          // [DisplayAddress5] I2C address of fifth 8x8 matrix module
#undef MTX_ADDRESS6          // [DisplayAddress6] I2C address of sixth 8x8 matrix module
#undef MTX_ADDRESS7          // [DisplayAddress7] I2C address of seventh 8x8 matrix module
#undef MTX_ADDRESS8          // [DisplayAddress8] I2C address of eigth 8x8 matrix module
#undef USE_DISPLAY_SEVENSEG
#undef USE_ADE7953
#undef USE_APDS9960_GESTURE   // Enable APDS9960 Gesture feature (+2k code)
#undef USE_APDS9960_PROXIMITY // Enable APDS9960 Proximity feature (>50 code)
#undef USE_APDS9960_COLOR     // Enable APDS9960 Color feature (+0.8k code)
#undef USE_APDS9960_STARTMODE
#undef MGS_SENSOR_ADDR
#endif

// -- Power monitoring sensors --------------------
#undef USE_ENERGY_MARGIN_DETECTION // Add support for Energy Margin detection (+1k6 code)
#undef USE_ENERGY_POWER_LIMIT      // Add additional support for Energy Power Limit detection (+1k2 code)
#undef USE_ENERGY_DUMMY            // Add support for dummy Energy monitor allowing user values (+0k7 code)
#undef USE_HLW8012                 // Add support for HLW8012, BL0937 or HJL-01 Energy Monitor for Sonoff Pow and WolfBlitz
#undef USE_CSE7766                 // Add support for CSE7766 Energy Monitor for Sonoff S31 and Pow R2
#undef USE_PZEM004T                // Add support for PZEM004T Energy monitor (+2k code)
#undef USE_PZEM_AC                 // Add support for PZEM014,016 Energy monitor (+1k1 code)
#undef USE_PZEM_DC                 // Add support for PZEM003,017 Energy monitor (+1k1 code)
#undef USE_MCP39F501               // Add support for MCP39F501 Energy monitor as used in Shelly 2 (+3k1 code)
#undef SDM72_SPEED            // SDM72-Modbus RS485 serial speed (default: 9600 baud)
#undef SDM120_SPEED           // SDM120-Modbus RS485 serial speed (default: 2400 baud)
#undef SDM230_SPEED           // SDM230-Modbus RS485 serial speed (default: 9600 baud)
#undef SDM630_SPEED           // SDM630-Modbus RS485 serial speed (default: 9600 baud)
#undef DDS2382_SPEED          // Hiking DDS2382 Modbus RS485 serial speed (default: 9600 baud)
#undef DDSU666_SPEED          // Chint DDSU666 Modbus RS485 serial speed (default: 9600 baud)
#undef SOLAXX1_SPEED          // Solax X1 Modbus RS485 serial speed (default: 9600 baud)
#undef LE01MR_SPEED           // LE-01MR modbus baudrate (default: 9600)
#undef LE01MR_ADDR              // LE-01MR modbus address (default: 0x01)
#undef USE_BL09XX                  // Add support for various BL09XX Energy monitor as used in Blitzwolf SHP-10 or Sonoff Dual R3 v2 (+1k6 code)
#undef IEM3000_SPEED         // iEM3000-Modbus RS485 serial speed (default: 19200 baud)
#undef IEM3000_ADDR              // iEM3000-Modbus modbus address (default: 0x01)

// -- Low level interface devices -----------------
#undef USE_DHT // Add support for DHT11, AM2301 (DHT21, DHT22, AM2302, AM2321) and SI7021 Temperature and Humidity sensor (1k6 code)

#undef MAX31865_PTD_WIRES // PTDs come in several flavors, pick yours. Specific settings per sensor possible with MAX31865_PTD_WIRES1..MAX31865_PTD_WIRES6
#undef MAX31865_PTD_RES // Nominal PTD resistance at 0°C (100Ω for a PT100, 1000Ω for a PT1000, YMMV!). Specific settings per sensor possible with MAX31865_PTD_RES1..MAX31865_PTD_RES6
#undef MAX31865_REF_RES // Reference resistor (Usually 430Ω for a PT100, 4300Ω for a PT1000). Specific settings per sensor possible with MAX31865_REF_RES1..MAX31865_REF_RES6
#undef MAX31865_PTD_BIAS  // To calibrate your not-so-good PTD. Specific settings per sensor possible with MAX31865_PTD_BIAS1..MAX31865_PTD_BIAS6

// -- Habilitar suporte a códigos IR -----------------
// #undef USE_IR_REMOTE
//#define USE_IR_REMOTE_FULL

#endif // _USER_CONFIG_OVERRIDE_H_
