import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.ensemble import RandomForestClassifier


data = pd.read_csv("sensor_data.csv")

X = data.drop(columns=["label"]) #sensor values
Y = data["label"] #lying, moving position etc

X_train, X_test, Y_train, Y_test = train_test_split(X, Y, test_size=0.2, random_state=42)

scaler = StandardScaler()
X_train = scaler.fit_transform(X_train)
X_test = scaler.transform(X_test)

clf = RandomForestClassifier(n_estimators=100, random_state=42)
clf.fit(X_train, Y_train)

acc = clf.score(X_test, Y_test)
print(f"Model Accuracy: {acc:.2f}")
