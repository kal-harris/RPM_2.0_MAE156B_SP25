from Phidget22.Phidget import *
from Phidget22.Devices.Accelerometer import *
from Phidget22.Devices.TemperatureSensor import *
from Phidget22.Devices.Spatial import *

import matplotlib.pyplot as plt
import matplotlib.animation as animation
from datetime import datetime
import numpy as np
import time
import csv
import sys
import statistics

# --- Data Storage ---
timestamps = []
x_vals, y_vals, z_vals = [], [], []
temperature_vals = []
latest_temperature = [0]
x_mean, y_mean, z_mean, norm = [0], [0], [0], [0]




# --- CSV Setup ---
filename = f"accel_data_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"
csv_file = open(filename, mode='w', newline='')
csv_writer = csv.writer(csv_file)
csv_writer.writerow(["Timestamp", "Accel_X", "Accel_Y", "Accel_Z", "Temperature"])

def wait_for_temperature_stabilization(sensor):
    print("Waiting for temperature to reach at least 50°C...")
    while True:
        current_temp = sensor.getTemperature()
        print(f"Current temperature: {current_temp:.2f}°C")
        if current_temp >= 50:
            print(f"Temperature reached {current_temp:.2f}°C")
            return
        time.sleep(0.25)


# --- Phidget Event Handlers ---
def onAccelerationChange(self, acceleration, timestamp):
    ts = datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]
    x, y, z = acceleration
    temp = latest_temperature[0]

    x_vals.append(x)
    y_vals.append(y)
    z_vals.append(z)
    timestamps.append(ts)
    temperature_vals.append(temp)

    x_mean.append(((x_mean[-1]*(len(x_vals)-1))+x_vals[-1])/len(x_vals))
    y_mean.append(((y_mean[-1]*(len(y_vals)-1))+y_vals[-1])/len(y_vals))
    z_mean.append(((z_mean[-1]*(len(z_vals)-1))+z_vals[-1])/len(z_vals))
    norm.append(np.sqrt(x_mean[-1]**2 + y_mean[-1]**2 + z_mean[-1]**2))

    # Trim to last 100 points
    if len(x_vals) > 100:
        x_vals.pop(0)
        y_vals.pop(0)
        z_vals.pop(0)
        temperature_vals.pop(0)
        timestamps.pop(0)
        x_mean.pop(0)
        y_mean.pop(0)
        z_mean.pop(0)
        norm.pop(0)

    csv_writer.writerow([ts, x, y, z, temp])

def onTemperatureChange(self, temperature):
    latest_temperature[0] = temperature

# --- Phidget Setup ---
accelerometer = Accelerometer()
temp_sensor = TemperatureSensor()
spatial = Spatial()

accelerometer.setDeviceSerialNumber(721166)
temp_sensor.setDeviceSerialNumber(721166)
spatial.setDeviceSerialNumber(721166)

accelerometer.setOnAccelerationChangeHandler(onAccelerationChange)
temp_sensor.setOnTemperatureChangeHandler(onTemperatureChange)


accelerometer.openWaitForAttachment(5000)
temp_sensor.openWaitForAttachment(5000)
spatial.openWaitForAttachment(5000)

spatial.setHeatingEnabled(True)

# Optional: Faster data rate
accelerometer.setDataInterval(accelerometer.getMinDataInterval())
wait_for_temperature_stabilization(temp_sensor)

# --- Plot Setup ---
fig, ax = plt.subplots(2, 1, figsize=(10, 6))
fig.tight_layout(pad=3)

def animate(i):
    if not x_vals:
        print("No data yet.")
        return

    ax[0].clear()
    ax[0].plot(x_vals, label='X')
    ax[0].plot(y_vals, label='Y')
    ax[0].plot(z_vals, label='Z')
    ax[0].set_title("Accelerometer")
    ax[0].legend()
    ax[0].grid(True)

    ax[1].clear()
    ax[1].plot(temperature_vals, label='Temperature', color='orange')
    ax[1].set_title("Temperature")
    ax[1].legend()
    ax[1].grid(True)
'''
    ax[2].clear()
    ax[2].plot(x_mean, label='X Mean')
    ax[2].plot(y_mean, label='Y Mean')
    ax[2].plot(z_mean, label='Z Mean')
    ax[2].set_title("Accelerometer Mean")
    ax[2].legend()
    ax[2].grid(True)

    ax[3].clear()
    ax[3].plot(norm, label='Norm', color='purple')
    ax[3].set_title("Accelerometer Norm")
    ax[3].legend()
    ax[3].grid(True)
'''
# --- Start Live Plot ---
ani = animation.FuncAnimation(fig, animate, interval=500, cache_frame_data=False)
sys.modules[__name__].ani = ani  # Prevent garbage collection

plt.show()

# --- Cleanup ---
accelerometer.close()
temp_sensor.close()
csv_file.close()
