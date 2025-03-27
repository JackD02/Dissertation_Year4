import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import classification_report, confusion_matrix

### FILE LOCATIONS FOR EACH EXPERIMENT
# PIEZO CHAIR EXP ("C:\UNI\Y4\Dissertation\D2 Deliverable\Data for Diss\Piezo Chair Experiment\PC_Combined.csv")
# PIEZO BED EXP ("C:\UNI\Y4\Dissertation\D2 Deliverable\Data for Diss\Piezo Bed Experiment\PBTCombined.csv")
# PIEZO + FLEX BED EXP ()
# RANGEFINDER BED EXP ()
# FULL ARRAY TESTING ()

data = pd.read_csv(r"C:\UNI\Y4\Dissertation\D2 Deliverable\Data for Diss\Piezo Bed Experiment\PBTCombined.csv")

X = data.drop(columns=["Timestamp (ms)", "Label"]) #keep only sensor values
Y = data["Label"] #empty or sitting

X_train, X_test, Y_train, Y_test = train_test_split(X, Y, test_size=0.2, random_state=42)

clf = RandomForestClassifier(n_estimators=100, class_weight ="balanced", random_state=42)
clf.fit(X_train, Y_train)

#checking accuracy
acc = clf.score(X_test, Y_test)
print(f"\nModel Accuracy: {acc:.2f}")

train_acc = clf.score(X_train, Y_train)
test_acc = clf.score(X_test, Y_test)

print(f"\nTraining Accuracy: {train_acc:.2f}")
print(f"Test Accuracy: {test_acc:.2f}")

#checking feature importance (which input columns are most important)
importances = clf.feature_importances_
f_names = X.columns
print("\nImportances: ")
for name, importance in zip(f_names, importances):
    print(f"{name}: {importance:.4f}")

#confusion matrix checking for true positives/false negatives etc...
Y_pred = clf.predict(X_test)
print("\nConfusion Matrix: ")
print(confusion_matrix(Y_test, Y_pred))

print("\nClassification Report: ")
print(classification_report(Y_test, Y_pred))
