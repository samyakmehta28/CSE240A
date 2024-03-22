#!/bin/bash

# Specify the path to your C executable
EXECUTABLE="./predictor.exe"

# List of zipped files
ZIPPED_FILES=("fp_1.bz2" "fp_2.bz2" "int_1.bz2"  "int_2.bz2" "mm_1.bz2" "mm_2.bz2")

# List of parameters to pass to the executable
PARAMS=("8:8" "16:8" "32:8" "64:7" "128:6")

# Make the executable
# echo "Make the Predictor executable"
# make

# Loop through each zipped file
for ((i=0; i<${#ZIPPED_FILES[@]}; i++)); do
    echo "Executing on ${ZIPPED_FILES[i]}"
    for ((j=0; j<${#PARAMS[@]}; j++)); do
        # Print a message indicating the start of execution
        echo "Predictor: --custom:${PARAMS[j]}"
        # Run the executable with the corresponding parameter
        bunzip2 -kc "../traces/${ZIPPED_FILES[i]}" | $EXECUTABLE --custom:"${PARAMS[j]}"
        # Print a newline for better separation of outputs
        echo
    done
    echo
    echo
done