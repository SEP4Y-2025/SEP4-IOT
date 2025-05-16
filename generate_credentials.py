import csv
import os

output_path = os.path.join("src", "config", "wifi_credentials.h")

mqtt_broker_ip = "172.20.10.2"
mqtt_broker_port = 1883

with open('credentials.csv') as csvfile:
    reader = csv.DictReader(csvfile)
    rows = list(reader)

    os.makedirs(os.path.dirname(output_path), exist_ok=True)

    with open(output_path, 'w') as header:
        header.write("// This file is Auto Generated in platformio.ini file from credentials.csv\n")
        header.write('#include "wifi.h"\n')
        header.write('#include <stdint.h>\n\n')

        header.write("static const wifi_credential_t known_credentials[] = {\n")
        for row in rows:
            ssid = row["ssid"]
            password = row["password"]
            header.write(f'    {{.ssid = "{ssid}", .password = "{password}"}},\n')
        header.write("};\n\n")

        header.write(f"static const uint8_t known_credentials_len = sizeof(known_credentials) / sizeof(known_credentials[0]);")
        
        header.write(f'static const char *mqtt_broker_ip = "{mqtt_broker_ip}";\n')
        header.write(f'static const int mqtt_broker_port = {mqtt_broker_port};\n')

print(f"wifi_credentials.h generated at: {output_path}")
