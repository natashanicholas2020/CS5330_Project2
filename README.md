Name: Natasha Nicholas
Date: Feb. 11, 2026
Project: Content-based Image Retrieval

Overview
    This project implements a Content-Based Image Retrieval system in C++ using OpenCV and Qt6.  
    The system compares a target image against a database of images and returns the N most similar images, using a variety of feature extraction and matching techniques.
    The process follows these steps:
        1. Compute features for the target image (T)
        2. Compute features for all database images (B)
        3. Compute distances between T and each image in B
        4. Return the top N matches sorted by smallest distance

Files
    CMakeLists.txt
        Builds the project using C++17, OpenCV, and Qt6

    main.cpp
        Initializes the Qt application and opens the GUI window

    Project2Window.h / Project2Window.cpp
        Implements the GUI:
            Extract tab:
                Choose image directory
                Extract features for all images
            Match tab:
                Choose target image filename
                Choose feature CSV file
                Enter N
                Display top N matches

    feature_utils.h / feature_utils.cpp
        Feature extraction functions:
            BASELINE
                7x7 center pixel RGB vector
            COLOR
                RG chromaticity histogram (16x16 bins)
            MULTIHIST
                Top & bottom half RGB histograms
            COLOR_TEXTURE
                RGB histogram + Sobel magnitude histogram
            CUSTOM
                Same as COLOR_TEXTURE (user-defined)
            DNN_EMB
                512-D ResNet18 embedding read from CSV

    matcher_utils.h / matcher_utils.cpp
        Matching functions:
            SSD (for BASELINE)
            Histogram Intersection (for COLOR, MULTIHIST, COLOR_TEXTURE, CUSTOM)
            Cosine Distance (for DNN_EMB)

Usage
    Build
        mkdir build
        cd build
        cmake ..
        make

    Run (GUI)
        ./Project2App

    GUI Workflow
        Extract Features (complete this first):
            1. Click “Choose Image Directory”
            2. Select image folder
            3. Click “Extract Features (All)”
            4. CSV files are generated:
                baseline.csv
                hist.csv
                multihist.csv
                ct.csv
                custom.csv

        Run Match (complete this second):
            1. Enter target image filename from the included images
            2. Enter feature CSV filename from the generated options
            3. Enter N for number of top matches
            4. Click “Run Match”
            5. Results list displays top N matches excluding the target image