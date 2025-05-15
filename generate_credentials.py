import csv
import os

# Path to output file
output_path = os.path.join("src", "config", "wifi_credentials.h")

with open('credentials.csv') as csvfile:
    reader = csv.DictReader(csvfile)
    row = next(reader)
    
    # Ensure the output directory exists
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    
    # Write the header file
    with open(output_path, 'w') as header:
        header.write(f'#define WIFI_SSID "{row["ssid"]}"\n')
        header.write(f'#define WIFI_PASSWORD "{row["password"]}"\n')
        header.write(f'#define BROKER_IP "{row["broker_ip"]}"\n')

print(f"✅ credentials.h generated at: {output_path}")
