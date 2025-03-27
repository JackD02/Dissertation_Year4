import pandas as pd
from sklearn.model_selection import train_test_split, cross_val_score
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score, classification_report, confusion_matrix
from sklearn.preprocessing import LabelEncoder

### FILE LOCATIONS FOR EACH EXPERIMENT
# PIEZO CHAIR EXP ("C:\UNI\Y4\Dissertation\D2 Deliverable\Data for Diss\Piezo Chair Experiment\PC_Combined.csv")
# PIEZO BED EXP ("C:\UNI\Y4\Dissertation\D2 Deliverable\Data for Diss\Piezo Bed Experiment\PBTCombined.csv")
# PIEZO + FLEX BED EXP ("C:\UNI\Y4\Dissertation\D2 Deliverable\Data for Diss\Flex Bed Experiment\FTB_Combined.csv")
# RANGEFINDER BED EXP ()
# FULL ARRAY TESTING ()

data = pd.read_csv(r"C:\UNI\Y4\Dissertation\D2 Deliverable\Data for Diss\3 Flex Bed Experiment\FTB_Combined.csv")
external_set = pd.read_csv(r"C:\UNI\Y4\Dissertation\D2 Deliverable\Data for Diss\3 Flex Bed Experiment\FBT4.csv", encoding='ISO-8859-1')

encoder = LabelEncoder()
encoder.fit(data["Label"])

#timestamp values have no value in the ML aspect of this
X = data[[" Piezo Value Left", " Piezo Value Right", " Flex Value Middle"]] #keep only sensor values
Y = encoder.transform(data["Label"]) #left, right, middle

X_train, X_test, Y_train, Y_test = train_test_split(X, Y, test_size=0.2)#,random_state=42

clf = RandomForestClassifier(n_estimators=100,max_depth = 5) # random_state=42
clf.fit(X_train, Y_train)

#Make prediction
Y_pred = clf.predict(X_test)

Y_pred_labels = encoder.inverse_transform(Y_pred).astype(str)
Y_test_labels = encoder.inverse_transform(Y_test).astype(str)

#Model Accuracy
accuracy = accuracy_score(Y_test, Y_pred)
print(f"\nModel Accuracy: {accuracy:.2f}")

#confusion matrix checking for true positives/false negatives etc...
print("\nConfusion Matrix: ")
print(confusion_matrix(Y_test_labels, Y_pred_labels))

print("\nClassification Report: ")
print(classification_report(Y_test_labels, Y_pred_labels, zero_division=1))

#checking feature importance (which input columns are most important)
importances = clf.feature_importances_
print("\nImportances: ")
for i, feature in enumerate([" Piezo Value Left", " Piezo Value Right", " Flex Value Middle"]):
    print(f"{feature}: {importances[i]:.4f}")

### EXTERNAL SET
# Preprocess external data (apply the same preprocessing as the training data)
external_X = external_set[[" Piezo Value Left", " Piezo Value Right", " Flex Value Middle"]]
external_Y = encoder.transform(external_set["Label"])

# Evaluate on the external validation set
external_accuracy = clf.score(external_X, external_Y)
print(f"\nExternal Validation Accuracy: {external_accuracy:.2f}")

# Make predictions on the external validation data
external_Y_pred = clf.predict(external_X)

# Convert predictions back to string labels
# without this errors will occure (labels containing mix of data types)
external_Y_labels = encoder.inverse_transform(external_Y)
external_Y_pred_labels = encoder.inverse_transform(external_Y_pred)

# Print the classification report on external validation set
external_Y = external_Y.astype(str)
external_Y_pred = external_Y_pred.astype(str)

print("\nClassification Report for External Validation Set: ")
print(classification_report(external_Y_labels, external_Y_pred_labels, labels= encoder.classes_, zero_division=1))

# Confusion Matrix for External Data
print("\nConfusion Matrix for External Validation Set: ")
print(confusion_matrix(external_Y_labels, external_Y_pred_labels))