import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score, classification_report, confusion_matrix
from sklearn.preprocessing import StandardScaler, LabelEncoder

### FILE LOCATIONS FOR EACH EXPERIMENT
# PIEZO CHAIR EXP ("C:\UNI\Y4\Dissertation\D2 Deliverable\Data for Diss\Piezo Chair Experiment\PC_Combined.csv")
# PIEZO BED EXP ("C:\UNI\Y4\Dissertation\D2 Deliverable\Data for Diss\Piezo Bed Experiment\PBTCombined.csv")
# PIEZO + FLEX BED EXP ()
# RANGEFINDER BED EXP ()
# FULL ARRAY TESTING ()

data = pd.read_csv(r"C:\UNI\Y4\Dissertation\D2 Deliverable\Data for Diss\Piezo Bed Experiment\PBTCombined.csv")

encoder = LabelEncoder()
encoder.fit(data[" Label"])

X = data[[" Piezo Value Left", " Piezo Value Right"]] #keep only sensor values
Y = encoder.transform(data[" Label"]) #left or right

X_train, X_test, Y_train, Y_test = train_test_split(X, Y, test_size=0.2, random_state=42)

scaler = StandardScaler()
X_train = scaler.fit_transform(X_train)
X_test = scaler.transform(X_test)


clf = RandomForestClassifier(n_estimators=100, random_state=42)
clf.fit(X_train, Y_train)

#Make prediction
Y_pred = clf.predict(X_test)

Y_pred_labels = encoder.inverse_transform(Y_pred).astype(str)
Y_test_labels = encoder.inverse_transform(Y_test).astype(str)

#Model Accuracy
accuracy = accuracy_score(Y_test, Y_pred)
print(f"\nModel Accuracy: {accuracy:.2f}")

#confusion matrix checking for true positives/false negatives etc...
cm = confusion_matrix(Y_test_labels, Y_pred_labels)
print("\nConfusion Matrix: ")
print(cm)

print("\nClassification Report: ")
print(classification_report(Y_test_labels, Y_pred_labels, zero_division=1))

#checking feature importance (which input columns are most important)
importances = clf.feature_importances_
print("\nImportances: ")
print(f"Left Piezo= {importances[0]}, Right Piezo= {importances[1]}")
