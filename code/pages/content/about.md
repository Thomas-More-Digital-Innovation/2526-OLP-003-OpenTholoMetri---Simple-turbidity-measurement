# OpenTholoMetri

OpenTholoMetri is heavily inspired by the [OpenOBS-328](https://github.com/tedlanghorst/OpenOBS-328/tree/main) project by Ted Langhorst. Many thanks to [Olpas](https://www.olpas.tech/) for their support in making this project a reality and to Thomas More Digital Innovation for introducing me to this opportunity.

## System Overview

The OpenTholoMetri is a data logging system designed for turbidity measurement using optical sensors. The system operates in a low-power mode, periodically waking up to take sensor readings, logging the data to an SD card with timestamps, and then returning to sleep.

## Why does this project exist?

While the OpenOBS-328 is great, I found the documentation quite difficult to read. The fact of having to order PCBs (with minimum order quantities) made it hard to justify as a one-off sensor, which makes the accessibility of the sensor lower. That's why: to make water quality measurements accessible to anyone, even if they only want to build a couple. If you need to build these in bulk, I'd strongly recommend looking at the OpenOBS-328 instead.

![OpenTholoMetri with Olpas' other sensors](/assets/OpenTholoMetriWithOlpas.jpg)

## Getting started

 - View the materials list [here](/materials-list.html)
 - View the build instructions [here](/build.html)
 - View the code documentation [here](/documentation.html)

## Features

- **Low Power Operation**: Efficient power management for extended battery life
- **Accurate Sensing**: Optical turbidity measurement with proximity and ambient light sensors
- **Data Logging**: Timestamps and sensor readings stored on SD card
- **CSV Export**: Easy-to-analyze data format

## Tools

This project includes tools to help you work with the OpenTholoMetri:

- **<a href="csv-viewer/" target="_blank">CSV Viewer</a>**: A browser-based basic tool to visualize your sensor data

## Data Format

The device generates CSV files with the following format:
- Headers: `Timestamp,Proximity,Ambient Light,Battery Voltage,Temperature (C),Turbidity Raw,Turbidity Voltage`
- Timestamp format: `YYYY-MM-DD HH:MM:SS`
- All measurements are logged at regular intervals
