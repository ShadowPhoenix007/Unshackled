from sklearn.pipeline import Pipeline
from sklearn.tree import DecisionTreeClassifier
from sklearn.metrics import accuracy_score
from sklearn.model_selection import train_test_split
import pandas as pd

filepath = 'C:/Users/ASUS/Downloads/Imagine Cup test/Unshackled/datacollection/datacsv.csv'

data = pd.read_csv(filepath)

X = data[~data['timestamp'].isna()][['flex1', 'flex2', 'flex3', 'flex4', 'flex5', 'piezo', 
                             'accelX', 'accelY', 'accelZ', 'gyroX', 'gyroY', 'gyroZ']]
y = data[~data['timestamp'].isna()][['tremor']]

xTrain, xTest, yTrain, yTest = train_test_split(X, y, train_size=0.8, test_size=0.2, random_state=69)

piper = Pipeline([('DTC', DecisionTreeClassifier())])

def evaluate(model, x_test, y_test):
    predictions = model.predict(x_test)
    acc = accuracy_score(predictions, y_test)
    return round(acc*100, 3)

piper.fit(xTrain, yTrain)
print(f"Accuracy = {evaluate(piper, xTest, yTest)}")