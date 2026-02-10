# OpenTholoMetri

## System Overview

The OpenTholoMetri is a data logging system designed for turbidity measurement using optical sensors. The system operates in a low-power mode, periodically waking up to take sensor readings, logging the data to an SD card with timestamps, and then returning to sleep.

## Features

- **Low Power Operation**: Efficient power management for extended battery life
- **Accurate Sensing**: Optical turbidity measurement with proximity and ambient light sensors
- **Data Logging**: Timestamps and sensor readings stored on SD card
- **CSV Export**: Easy-to-analyze data format

## Tools

This project includes several tools to help you work with the OpenTholoMetri:

- **[CSV Viewer](csv-viewer/)**: A browser-based tool to visualize your sensor data
  - Load CSV files directly from the SD card
  - View interactive plots of your measurements
  - No server required - everything runs locally

## Getting Started

1. Power on your OpenTholoMetri device
2. The device will automatically start taking measurements
3. Retrieve the SD card to access your data
4. Use the CSV Viewer to analyze your results

## Data Format

The device generates CSV files with the following format:
- Headers: `Timestamp,Proximity,Ambient Light,Battery Voltage`
- Timestamp format: `YYYY-MM-DD HH:MM:SS`
- All measurements are logged at regular intervals
