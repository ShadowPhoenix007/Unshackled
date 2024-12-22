import serial
import time
import csv
import os

filepath = 'C:/Users/ASUS/Downloads/Imagine Cup test/Unshackled/datacollection/datacsv.csv'
#running locally on my computer

empty = True

name = input ('what is your name')
age = int(input ('what is your age'))
gender = input ('what is your gender. M/F')

def readserial(port, baudrate, timestamp=False):
    empty = not os.path.exists(filepath) or os.path.getsize(filepath) == 0

    with open(filepath, 'a', newline='') as csvfile:
        writer = csv.writer(csvfile)

        if empty:
            writer.writerow(['name', 'age', 'gender', 'timestamp', 'flex1', 'flex2', 'flex3', 'flex4', 'flex5', 'piezo', 
                             'AccelX', 'AccelY', 'AccelZ', 'GyroX', 'GyroY', 'GyroZ'])
    
        ser = serial.Serial(port, baudrate, timeout=0.1)         # 1/timeout is the frequency at which the port is read
        dataBool = False
        try:
            while True:
                try:
                    data = ser.readline().decode().strip()
                    dataList = data.split(', ')
                    if data and timestamp:
                        timestamp = time.strftime('%H:%M:%S')
                        dataList.insert(0, timestamp)
                        print(dataList)
                    elif data:
                        print(data)
                    dataList.insert(0, gender)
                    dataList.insert(0, age)
                    dataList.insert(0, name)
                    if dataBool and len(dataList)>4:
                        writer.writerow(dataList)
                    if len(dataList)>4:
                        if dataList[4] == 'flex1':
                            dataBool = True
                except Exception as e:
                    print(f'Error during serial communication: {e}')

        except KeyboardInterrupt:
                print('\nProgram stopped by user.')
        finally:
        # Ensure the serial port is closed
            ser.close()
            print('Serial port closed.')
            print('File closed successfully.')

if __name__ == '__main__':
    readserial('COM15', 115200, True)

