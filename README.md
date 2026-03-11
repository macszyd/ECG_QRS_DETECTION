# ECG R-Peak Detector

A simple and efficient R-peak detector for ECG signals written in C++. The algorithm loads the raw signal, processes it using digital filters to enhance QRS complexes, and then detects peaks using adaptive thresholding. The project also includes an evaluation module that compares detected peaks with a reference file and calculates basic performance metrics.

## Main Features

* **Multi-stage signal filtering**: Uses differentiation, smoothing (moving average), and squaring to maximize the signal-to-noise ratio (SNR) for QRS complexes.
* **Adaptive thresholding**: The algorithm dynamically adjusts the detection threshold based on the amplitudes of previously detected peaks, allowing it to handle variable ECG signal amplitudes.
* **Classifier evaluation (Confusion Matrix)**: Automatic calculation of detection accuracy taking into account a margin of error:
  * **TP (True Positives)** - correctly detected R-peaks.
  * **FP (False Positives)** - incorrectly detected peaks (noise interpreted as an R-peak).
  * **FN (False Negatives)** - missed R-peaks.
* **PhysioNet standard support**: A parser adapted to read classic labels (e.g., `N`, `V`, `L`, `R`) used in databases like the MIT-BIH Arrhythmia Database.
* **ASCII visualization**: A simple `Draw` function allowing for a quick preview of the processed signal directly in the console.

## Algorithm Overview

Signal processing takes place in three main steps:
1. **Differential filtering** - highlights the steep slopes characteristic of QRS complexes (`y1[n] = x[n + Nd] - x[n]`).
2. **Moving average** - smooths the signal and eliminates fast-changing artifacts by averaging `Ng` previous samples.
3. **Non-linear squaring** - squaring the samples amplifies high amplitudes (QRS) and attenuates lower ones (P, T waves).

## Required Files

For the program to work correctly, the following input files must be present in its working directory:
* `samples_111.csv` - a file with the raw ECG signal. The script skips the first two header rows and reads the signal from the second column (values are multiplied by 200 by default).
* `annotations_111m.txt` - a text file with reference annotations. The expected format is: `[sample_number],[rhythm_type]`, e.g., `1244,N`.

## Compilation and Execution

The project has no external dependencies other than the standard C++ library (`STL`). You can compile it using any modern compiler.

### Linux / macOS (GCC/Clang)
```bash
g++ -std=c++11 main.cpp -o rpeak_detector
./rpeak_detector
