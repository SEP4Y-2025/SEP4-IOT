// device_config.h
#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H


#define DEVICE_ID "pot_1"

#define NUM_TOPICS 2
#define MQTT_TOPIC_ACTIVATE "/" DEVICE_ID "/activate"
#define MQTT_TOPIC_DEACTIVATE "/" DEVICE_ID "/deactivate"

/////// We don't subscribe to these, we just send ///////
#define MQTT_TOPIC_AUTOMATIC_READINGS "/" DEVICE_ID "/sensors"

extern const char *mqtt_topic_strings[NUM_TOPICS];

//TODO: add more topics here (in both .c and .h files) and increment NUM_TOPICS when doing so
#endif // DEVICE_CONFIG_H
