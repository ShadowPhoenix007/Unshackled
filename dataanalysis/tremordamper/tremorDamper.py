import serial
import time
import numpy as np
import pandas as pd

arduino = serial.Serial('COM15', 115200, timeout=1)
time.sleep(2)

prev_x, prev_y, prev_gyro_x, prev_gyro_y = 0, 0, 0,0 

def runMotors(accel_x, accel_y, gyro_x, gyro_y):
    global prev_x, prev_y, prev_gyro_x, prev_gyro_y

    # Define a threshold to ignore minor fluctuations (adjust as needed)
    threshold_accel = 2500000  # Minimum change to consider movement
    threshold_gyro = 10000000  # Minimum change in gyroscope

    # Compute acceleration and gyroscope deltas
    delta_x = accel_x - prev_x
    delta_y = accel_y - prev_y
    delta_gyro_x = gyro_x - prev_gyro_x
    delta_gyro_y = gyro_y - prev_gyro_y

    # Update previous values
    prev_x, prev_y = accel_x, accel_y
    prev_gyro_x, prev_gyro_y = gyro_x, gyro_y

    bist = []

    # Determine Motor A (x-axis motion)
    if abs(delta_x) > threshold_accel or abs(delta_gyro_x) > threshold_gyro:
        if delta_x > 0 or delta_gyro_x > 0:
            bist.append ("AMotorFOR")
        elif delta_x < 0 or delta_gyro_x < 0:
            bist.append ("AMotorREV")
    else:
        bist.append ("AMotorOff")

    # Determine Motor B (y-axis motion)
    if abs(delta_y) > threshold_accel or abs(delta_gyro_y) > threshold_gyro:
        if delta_y > 0 or delta_gyro_y > 0:
            bist.append ("BMotorFOR")
        elif delta_y < 0 or delta_gyro_y < 0:
            bist.append ("BMotorREV")
    else:
        bist.append ("BMotorOff")
    
    return (f"{bist[0]}\n{bist[1]}")


while True:
    try:
        data = arduino.readline().decode().strip()
        
        # Ensure data is not empty and starts with a numeric value
        if data and data[0].isdigit():
            dataList = data.split(', ')

            # Ensure we have at least 5 values before proceeding
            if len(dataList) >= 5:
                try:
                    # Convert string values to float
                    accel_x = float(dataList[0])
                    accel_y = float(dataList[1])
                    gyro_x = float(dataList[3])
                    gyro_y = float(dataList[4])

                    #print (f"{accel_x-prev_x}, {accel_y-prev_y}, {gyro_x-prev_gyro_x}, {gyro_y-prev_gyro_y}")

                    # Call motor control function
                    bunbotors = runMotors(accel_x, accel_y, gyro_x, gyro_y)
                    print(bunbotors)
                    arduino.write(f"{bunbotors}\n".encode())

                except ValueError:
                    print("Error: Could not convert MPU data to float.")

    except Exception as e:
        print("Error:", e)

    time.sleep(0.05)  # Simulated delay for real-time processing

