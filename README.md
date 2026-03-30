# Electrical & Instrumentation Section

## Overview
This repository serves as the central codebase for the baja vehicle's software systems. Which currently covers data acquisition and the 4WD/2WD drivetrain actuation.

## Core Systems
1. Drivetrain Actuator Control (Arduino)
- Purpose : Manages the shifting mechanism between 4WD and 2WD using an electrical linear actuator.
- Control Logic : Uses hall effect feedback to ensure the actuator reaches the correct stroke length for gear engagement.

2. DAQ Data Logger (Arduino)
- Purpose : Collects real-time CVT primary and secondary speeds to provide useful insights for the Powertrain Section
- Sensors : 1. Primary/Secondary RPM (Hall Effect) 2. CVT Belt Temp (MLX90614 IR Sensor)
- Storage : Logs to FAT32 formatted SD Card as .CSV using a latching circuit switch.
