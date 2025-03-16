import numpy as np
import pandas as pd
import serial
import time
import scipy.signal as signal
import matplotlib.pyplot as plt
from scipy.fftpack import fft
from fastdtw import fastdtw
from scipy.spatial.distance import euclidean

# === PARAMETERS ===
SERIAL_PORT = "COM11"  # Change for your Arduino port (e.g., "/dev/ttyUSB0" for Linux)
BAUD_RATE = 115200
REALTIME = True  # Set to True for real-time detection

# === FUNCTION TO READ CSV DATA ===
def read_csv(filename):
    df = pd.read_csv(filename)
    return df["accelX"], df["accelY"], df["accelZ"], df["gyroX"], df["gyroY"], df["gyro"], df["timestamp"]


arduino = serial.Serial("COM11", 115200, timeout=1)
time.sleep(2)  # Allow time for connection
print("Reading data from Arduino...")
# === FUNCTION TO READ SERIAL DATA FROM ARDUINO ===
def read_serial(serial_port, baud_rate, duration=10):
    global proBool  # Ensure we are using the global variable
    proBool = False
    ax, ay, az, gx, gy, gz, timestamps = [], [], [], [], [], [], []
    start_time = time.time()
   
    while time.time() - start_time < duration:
        try:
            data = arduino.readline().decode('utf-8').strip()
            if data and data[0].isdigit():
                dataList = data.split(', ')
                # Ensure we have at least 5 values before proceeding
                if len(dataList) >= 6:
                    proBool = True
                    accel_x = float(dataList[0])
                    accel_y = float(dataList[1])
                    accel_z = float(dataList[2])
                    gyro_x = float(dataList[3])
                    gyro_y = float(dataList[4])
                    gyro_z = float(dataList[5])
                    timestamps.append(time.time())
                    ax.append(accel_x)
                    ay.append(accel_y)
                    az.append(accel_z)
                    gx.append(gyro_x)
                    gy.append(gyro_y)
                    gz.append(gyro_z)
                    print(f"{ax[-1]}, {ay[-1]}, {az[-1]}, {gx[-1]}, {gy[-1]}, {gz[-1]}")
            else:
                proBool = False         
        except Exception as e:
            print(f"Error reading serial: {e}")
            continue
    return np.array(ax), np.array(ay), np.array(az), np.array(gx), np.array(gy), np.array(gz), np.array(timestamps)

# === BANDPASS FILTER FOR TREMOR FREQUENCIES (3-12 Hz) ===
def bandpass_filter(data, lowcut=3, highcut=12, fs=100, order=4):
    nyquist = 0.5 * fs
    low, high = lowcut / nyquist, highcut / nyquist
    b, a = signal.butter(order, [low, high], btype="band")
    return signal.filtfilt(b, a, data)

# === FFT ANALYSIS TO FIND DOMINANT FREQUENCIES ===
def compute_fft(signal_data, fs):
    n = len(signal_data)
    freqs = np.fft.fftfreq(n, d=1/fs)
    fft_values = np.abs(fft(signal_data))[:n//2]
    return freqs[:n//2], fft_values

def dtw_match(signal_data, reference_signal):
    signal_data = np.array(signal_data, dtype=np.float64).flatten()
    reference_signal = np.array(reference_signal, dtype=np.float64).flatten()

    print("Inside dtw_match:")
    print("signal_data shape:", signal_data.shape, "dtype:", signal_data.dtype)
    print("reference_signal shape:", reference_signal.shape, "dtype:", reference_signal.dtype)

    distance, path = fastdtw(signal_data, reference_signal, dist=euclidean)
    return distance

def normalize_signal(signal_data):
    min_val = np.min(signal_data)
    max_val = np.max(signal_data)
    return (signal_data - min_val) / (max_val - min_val) * 2 - 1 

# === MAIN FUNCTION ===
def detect_tremor():
    fs = 100  # Adjust this based on your Arduino sampling rate

    if REALTIME:
        ax, ay, az, gx, gy, gz, timestamps = read_serial(SERIAL_PORT, BAUD_RATE)
        print(f"proBool = {proBool}, Data Points Collected: {len(ax)}")
    else:
        ax, ay, az, gx, gy, gz, timestamps = read_csv("C:/Users/ASUS/Downloads/Imagine Cup test/Unshackled/datacollection/datacsv.csv")

    # Combine acceleration (Ax, Ay, Az) into a single magnitude signal
    if proBool:
        acc_mag = np.sqrt(ax**2 + ay**2 + az**2)
        gyro_mag = np.sqrt(gx**2 + gy**2 + gz**2)
        print (f"acceleration magnitude = {acc_mag}, fs = {fs}")
        print (f"gyro magnitude = {gyro_mag}, fs = {fs}")
    # Apply Bandpass Filter
        acc_filt = bandpass_filter(acc_mag, fs=fs)
        gyro_filt = bandpass_filter(gyro_mag, fs=fs)

        # Compute FFT
        freqs_acc, fft_acc = compute_fft(acc_filt, fs)
        freqs_gyro, fft_gyro = compute_fft(gyro_filt, fs)

        # Reference tremor signal (5 Hz sine wave)
        tremor_freq = 5  # Hz
        tremor_signal = np.sin(2 * np.pi * tremor_freq * np.linspace(0, len(acc_filt)/fs, len(acc_filt)))
        min_len = min(len(acc_filt), len(tremor_signal))
        acc_filt = acc_filt[:min_len]
        tremor_signal = tremor_signal[:min_len]

        acc_filt = np.array(acc_filt).reshape(-1)
        tremor_signal = np.array(tremor_signal).reshape(-1)

        acc_filt_normalized = normalize_signal(acc_filt)
        tremor_signal_normalized = normalize_signal(tremor_signal)

        # # Perform DTW
        # acc_dtw = dtw_match(acc_filt_normalized, tremor_signal_normalized)
        # gyro_dtw = dtw_match(gyro_filt, tremor_signal_normalized)

        print(tremor_signal)

        # Create the plot
        plt.figure(figsize=(10, 5))

        # Plot input data (acc_filt)
        plt.plot(acc_filt_normalized, label="Input Data (acc_filt)", linestyle='-', color='blue')

        # Plot reference data (tremor_signal)
        plt.plot(tremor_signal_normalized, label="Reference Data (tremor_signal)", linestyle='--', color='red')

        # Labels and title
        plt.xlabel("Sample Index")
        plt.ylabel("Amplitude")
        plt.title("Comparison of Input Data and Reference Data")
        plt.legend()
        plt.grid()

        # Show the plot
        plt.show()


        print (f"{acc_dtw}, {gyro_dtw}")

        # Tremor Detection Result
        if acc_dtw < 50 or gyro_dtw < 50:  # Threshold for tremor detection
            print("🚨 Tremor detected!")

        else:
            print("✅ No tremor detected.")

# Run the detection
detect_tremor()
# arduino = serial.Serial('COM11', 115200, timeout=1)
# time.sleep(2)

# prev_x, prev_y, prev_gyro_x, prev_gyro_y = 0, 0, 0,0 

# def runMotors(accel_x, accel_y, gyro_x, gyro_y):
#     global prev_x, prev_y, prev_gyro_x, prev_gyro_y

#     # Define a threshold to ignore minor fluctuations (adjust as needed)
#     threshold_accel = 2500000  # Minimum change to consider movement
#     threshold_gyro = 10000000  # Minimum change in gyroscope

#     # Compute acceleration and gyroscope deltas
#     delta_x = accel_x - prev_x
#     delta_y = accel_y - prev_y
#     delta_gyro_x = gyro_x - prev_gyro_x
#     delta_gyro_y = gyro_y - prev_gyro_y

#     # Update previous values
#     prev_x, prev_y = accel_x, accel_y
#     prev_gyro_x, prev_gyro_y = gyro_x, gyro_y

#     bist = []

#     # Determine Motor A (x-axis motion)
#     if abs(delta_x) > threshold_accel or abs(delta_gyro_x) > threshold_gyro:
#         if delta_x > 0 or delta_gyro_x > 0:
#             bist.append ("AMotorFOR")
#         elif delta_x < 0 or delta_gyro_x < 0:
#             bist.append ("AMotorREV")
#     else:
#         bist.append ("AMotorOff")

#     # Determine Motor B (y-axis motion)
#     if abs(delta_y) > threshold_accel or abs(delta_gyro_y) > threshold_gyro:
#         if delta_y > 0 or delta_gyro_y > 0:
#             bist.append ("BMotorFOR")
#         elif delta_y < 0 or delta_gyro_y < 0:
#             bist.append ("BMotorREV")
#     else:
#         bist.append ("BMotorOff")
    
#     return (f"{bist[0]}\n{bist[1]}")


# while True:
#     try:
#         data = arduino.readline().decode().strip()
        
#         # Ensure data is not empty and starts with a numeric value
#         if data and data[0].isdigit():
#             dataList = data.split(', ')

#             # Ensure we have at least 5 values before proceeding
#             if len(dataList) >= 5:
#                 try:
#                     # Convert string values to float
#                     accel_x = float(dataList[0])
#                     accel_y = float(dataList[1])
#                     gyro_x = float(dataList[3])
#                     gyro_y = float(dataList[4])

#                     #print (f"{accel_x-prev_x}, {accel_y-prev_y}, {gyro_x-prev_gyro_x}, {gyro_y-prev_gyro_y}")

#                     # Call motor control function
#                     bunbotors = runMotors(accel_x, accel_y, gyro_x, gyro_y)
#                     print(bunbotors)
#                     arduino.write(f"{bunbotors}\n".encode())

#                 except ValueError:
#                     print("Error: Could not convert MPU data to float.")

#     except Exception as e:
#         print("Error:", e)

#     time.sleep(0.05)  # Simulated delay for real-time processing

