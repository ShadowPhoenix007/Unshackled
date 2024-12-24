import pandas as pd
import matplotlib.pyplot as plt

filepath = 'C:/Users/ASUS/Downloads/Imagine Cup test/Unshackled/datacollection/datacsv.csv'

data = pd.read_csv(filepath)
tremor= data.loc[~data['timestamp'].isna()][data['name']=='prathiba shah']

print(tremor.head())

time_column = 'timestamp'
sensor_columns = ['flex1', 'flex2', 'flex3', 'flex4', 'flex5', 'accelX', 'accelY', 'accelZ', 'gyroX', 'gyroY', 'gyroZ']

plt.figure()
for sensor in sensor_columns:
    plt.plot(tremor[time_column], tremor[sensor], label=sensor)
plt.title('Sensor Values For PD Patient Prathiba Shah')
plt.legend(title='Sensors')
plt.show()