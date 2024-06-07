#!/bin/bash

# Definining the log directory
LOG_DIR="/home/COIS/3380/lab1"

# The Output file for all matches and their counts
OUTPUT_FILE="all_invalid_pages.txt"

# The Output file for the top 10 most-requested invalid pages
TOP_TEN_FILE="top_ten_404_pages.txt"

# Check if log file specification is provided as a command-line argument
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <log_file_specification>"
    exit 1
fi

# Log file specification from command-line argument
LOG_FILE_SPEC="$1"

# Search for non-existing pages (404 errors) and count unique occurrences
grep ' 404 ' $LOG_DIR/$LOG_FILE_SPEC  | awk '{print $6 $7}' | sort | uniq -c | sort -nr  > "$OUTPUT_FILE"

# Extract the top 10 most-requested invalid pages
head -n 10 "$OUTPUT_FILE" | awk '{print $1 $2}' > "$TOP_TEN_FILE"

# Display a message indicating the completion of the analysis
echo "Web log analysis completed. Results are stored in $OUTPUT_FILE and $TOP_TEN_FILE"
