#!/bin/bash

# Definining the log directory
LOG_DIR="/home/COIS/3380/lab1"

# Defining the output file for top 10 IPs
top_10_IPs_output_file="top_10_IPs.txt"

# Check if log file specification is provided as a command-line argument
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <log_file_specification>"
    exit 1
fi

# Log file specification from command-line argument
LOG_FILE_SPEC="$1"

# Extracting and counting the IP addresses that are associated with "404" page requests
grep ' 404 ' $LOG_DIR/$LOG_FILE_SPEC | grep -o '[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}' | sort | uniq -c | sort -nr | head -n 10  > "$top_10_IPs_output_file"

# Displaying the top 10 IP addresses
echo "Top 10 IP addresses with the most 404 page requests:"
head -n 10 "$top_10_IPs_output_file"

# Retrieving the geolocation information for the top 2 IP addresses
top_2_ips=($(awk '{print $2}' "$top_10_IPs_output_file" | head -n 2))
for ip_address in "${top_2_ips[@]}"; do
  echo "Retrieving the geolocation for IP: $ip_address"
  wget -qO- http://ipinfo.io/"$ip_address"
  echo -e "\n"
done

