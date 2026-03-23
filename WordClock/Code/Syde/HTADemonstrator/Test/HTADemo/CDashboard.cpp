#include "CDashboard.h"

/**
 * @brief Constructor for CDashboard.
 * Initializes the RemoteXY interface for WiFi control.
 */
CDashboard::CDashboard() {
    RemoteXY_Init();
}

/**
 * @brief Handles RemoteXY input and updates the interface.
 *
 * - If the zero button is pressed, tare the sensor.
 * - If the enter button is pressed, calibrate using the reference mass.
 * - Continuously update the online graph with live sensor data.
 *
 * @param sensor Reference to the CHX711Sensor object used for calibration and data access.
 */
void CDashboard::update(CHX711Sensor& sensor) {
    RemoteXY_Handler();

    if (RemoteXY.buttonZero == 1) {
        sensor.getZero();  ///< Perform tare (zeroing)
    }

    if (RemoteXY.buttonEnter == 1) {
        sensor.getEnter(); ///< Perform calibration
    }

    RemoteXY.onlineGraphHX711 = sensor.getRaw(); ///< Update live graph
}
