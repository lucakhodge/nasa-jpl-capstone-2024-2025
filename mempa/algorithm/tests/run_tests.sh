#!/bin/bash

filePath=$1

# Add debug output
set -x

DEM_FILE="/Users/adamcarlson/nasa-jpl-capstone-2024-2025-2/mempa/algorithm/data/Mars_HRSC_MOLA_BlendDEM_Global_200mp_v2.tif"
if [ ! -f "$DEM_FILE" ]; then
    echo "Error: DEM file not found at $DEM_FILE"
    exit 1
fi

# Directory setup
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_DIR="${SCRIPT_DIR}/../test_results"
mkdir -p "$TEST_DIR"

echo "Starting Mars Rover path tests..."

# Verify simulator exists
if [ ! -f "${SCRIPT_DIR}/../simulator" ]; then
  echo "Error: simulator not found. Please build it first."
  exit 1
fi

# Check for test coordinates file
if [ ! -f "${SCRIPT_DIR}/test_coordinates.json" ]; then
  echo "Error: test_coordinates.json not found"
  exit 1
fi

# Read and process test cases using jq
while IFS= read -r test_case; do
    startX=$(echo "$test_case" | jq -r '.start.x')
    startY=$(echo "$test_case" | jq -r '.start.y')
    endX=$(echo "$test_case" | jq -r '.end.x')
    endY=$(echo "$test_case" | jq -r '.end.y')
    name=$(echo "$test_case" | jq -r '.name')
    
    echo "Running test '$name': ($startX,$startY) to ($endX,$endY)"
    
    # Run simulator with coordinates
    cd "$SCRIPT_DIR/.." && ./simulator "$startX" "$startY" "$endX" "$endY" > "$TEST_DIR/test_${startX}_${startY}_${endX}_${endY}.log"
    
    if [ -f "rover_path.txt" ]; then
        mv "rover_path.txt" "$TEST_DIR/path_${startX}_${startY}_${endX}_${endY}.txt"
        echo "Test '$name' completed and saved"
    else
        echo "Warning: rover_path.txt not generated for test '$name'"
    fi
done < <(jq -c '.test_cases[]' "${SCRIPT_DIR}/test_coordinates.json")

echo "All tests completed. Results in $TEST_DIR/"

# Display test summary
if ls "${TEST_DIR}"/test_*.log 1>/dev/null 2>&1; then
  echo -e "\nTest Summary:"
  echo "=============="
  grep "Maximum Slope:" "${TEST_DIR}"/test_*.log
else
  echo "No test results found"
fi
