import pandas as pd
import math
from sklearn.metrics import accuracy_score

filepath = 'C:/Users/ASUS/Downloads/Imagine Cup test/Unshackled/datacollection/datacsv.csv'

data = pd.read_csv(filepath)

data = data[~data['timestamp'].isna()]

notremor = data[data['tremor']=='no']
tremor = data[data['tremor']=='yes']

sensors = ['accelX', 'accelY', 'accelZ', 'gyroX', 'gyroY', 'gyroZ']
FluctuationsTremor = {}
FluctuationsNonTremor = {}

for sensor in sensors:
    lastValue = 0
    totalFluc = 0
    i = 0
    for value in tremor[sensor]:
        if value!=0:
            fluctuation = abs(value-lastValue)/abs(value)
            totalFluc+=fluctuation;i+=1
            lastValue = value
    FluctuationsTremor[sensor] = totalFluc/i

for sensor in sensors:
    lastValue = 0
    totalFluc = 0
    i = 0
    for value in notremor[sensor]:
        if value!=0:
            fluctuation = abs(value-lastValue)/abs(value)
            totalFluc+=fluctuation;i+=1
            lastValue = value
    FluctuationsNonTremor[sensor] = totalFluc/i

print (f"Tremor Fluctuations = {FluctuationsTremor}")
print (f"Tremor Fluctuations = {FluctuationsNonTremor}")

def predict(fluctuations):
    tremor_diff = 0
    non_tremor_diff = 0
    for sensor in sensors:
        tremor_diff += abs(fluctuations[sensor] - FluctuationsTremor[sensor])
        non_tremor_diff += abs(fluctuations[sensor] - FluctuationsNonTremor[sensor])
    tremor_diff = tremor_diff/len(sensors)
    non_tremor_diff /= len(sensors)    
    if tremor_diff < non_tremor_diff:
        return "yes"
    else:
        return "no"

last_values = {sensor: 0 for sensor in sensors}

def calculate_fluctuations(row):
    fluctuations = {}
    for sensor in sensors:
        current_value = row[sensor]
        if current_value != 0:
            fluctuations[sensor] = abs(current_value - last_values[sensor]) / abs(current_value)
            last_values[sensor] = current_value
        else:
            fluctuations[sensor] = 0
    return fluctuations

data['fluctuation'] = data.apply(calculate_fluctuations, axis=1)
data['predicted_tremor'] = data['fluctuation'].apply(predict)
accuracy = accuracy_score(data['tremor'], data['predicted_tremor'])
print(f"Accuracy Score: {round(accuracy * 100, 2)}%")
