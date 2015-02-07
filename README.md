# SleepQA
The Sleep Quality Assessor is a device that measures the quality of a person's sleep using Arduino boards. The device consists of two units, a main board holding light, temperature, and sounds sensors as well as an SD card slot. The other holds a vibration sensor and wirelessly sends data to the main unit through XBee. The room conditions are compared to the vibration data (attached to the person's bed) to determine quality of sleep.

The main board runs the ConsolidatedSensors.ino program and the secondary board runs the SenderUno.ino program.
