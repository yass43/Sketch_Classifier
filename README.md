# Sketch Classifier with Time Warping Distance
This repository contains a C program that implements a sketch classifier based on the k-nearest neighbors (KNN) algorithm using the Time Warping Distance (TWD) as the distance metric. The TWD is computed using a dynamic programming approach.

## Project Description

In this project, we aim to build a sketch classifier capable of classifying sketches into one of ten predefined classes. The classification process is based on the KNN algorithm, which determines the class of a sketch by comparing it to its k-nearest neighbors in the training dataset. To measure the similarity between sketches, we use the Time Warping Distance (TWD) as a distance metric.

The key components of this project include:

1. **TWD Calculation**: The TWD between two sketches is computed using a dynamic programming approach. This distance metric is suitable for comparing sketches that may vary in terms of speed, scale, and rotation.

2. **KNN Classification**: For each test sketch, we find its k-nearest neighbors in the training dataset based on the TWD. The class label that appears most frequently among these neighbors is assigned as the predicted class for the test sketch.

NB : This project was done for the data structure and algorithms lecture a long time ago.

