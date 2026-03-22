 # SPS30 Particulate Matter Sensor Driver for ESP-IDF

This is a component for the Espressif IoT Development Framework (ESP-IDF) to interface with the Sensirion SPS30 particulate matter sensor using I2C.

## Connection

Connect the sensor to your ESP board as follows:

*   **VDD** to **3.3V** or **5V**
*   **GND** to **GND**
*   **SCL** to your desired SCL pin
*   **SDA** to your desired SDA pin
*   **SEL** to **GND** (to select I2C interface)

For better reliability a 4.7K to 10K resistor between VDD and SCL and SDA is recommended.

## How to run the example

The provided example initializes the sensor, reads its metadata, and then continuously reads and prints measurement data.

1.  **Important:** Open `main/main.c` and configure the `SDA_GPIO` and `SCL_GPIO` pins according to your hardware connections.
2.  Build, flash, and monitor the project using the ESP-IDF command-line tool:
    ```bash
    idf.py build flash monitor
    ```

## Publishing

This component is published on the [ESP-IDF Component Registry](https://components.espressif.com/components/thebigboschi/sps30).

