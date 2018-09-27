# BLE Temperature Sensor

## Overview

This is a skeleton for an Apache Mynewt project implementing a temperature sensor.
You will need the Apache Newt tool, as documented in the [Getting Started Guide](http://mynewt.apache.org/os/get_started/introduction/).

## Code

The source files are located in [apps/ble_temp_sensor](apps/ble_temp_sensor):

  * pkg.yml contains the base definition of the app
  * syscfg.yml contains setting definitions and overrides
  * src/main.c is the main entry point of the application
  * src/thermal_log.c implements the temperature sensing / circular buffer task
  * src/gatt_svr.c implements the basic BLE GATT server functionality

The available targets are specified in [targets/](targets/).

## Build

1. Download and install Apache Newt tool.

2. Download the Apache Mynewt core and package dependencies:

```no-highlight
    $ newt install
```

3. To build a given application target:

```no-highlight
    $ newt build <target>
```

4. To create an image suitable for flashing a target board:

```no-highlight
    $  newt create-image <target> 1.0.0
```

## Run

To flash the target board:

```no-highlight
    $  newt load <target>
```


![Alt text](Advertising_Martians_inc.png?raw=true "Advertising")

![Alt text](GATT_read_two_characteristics.png?raw=true "Two GATT characteristics supported")

![Alt text](Wireshark_advertising.jpg?raw=true "Wireshark advertising")

