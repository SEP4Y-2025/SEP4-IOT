
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mosquitto.h>
#include <cjson/cJSON.h>
#include <time.h>
#include <unistd.h>

const char *pot_id = "pot_1";

void on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg)
{
    cJSON *json = cJSON_Parse(msg->payload);
    if (!json)
        return;

    cJSON *cmd = cJSON_GetObjectItem(json, "command");
    cJSON *cid = cJSON_GetObjectItem(json, "correlation_id");
    cJSON *freq = cJSON_GetObjectItem(json, "watering_frequency");
    cJSON *dosage = cJSON_GetObjectItem(json, "water_dosage");
    cJSON *pid = cJSON_GetObjectItem(json, "pot_id");

    if (pid && strcmp(pid->valuestring, pot_id) == 0)
    {
        if (cmd && cid && strcmp(cmd->valuestring, "pots/add") == 0)
        {
            printf("Received registration for %s\n", pot_id);
            fflush(stdout);

            // Respond with status
            char topic[128], payload[128];
            snprintf(topic, sizeof(topic), "pots/%s/add/%s", pid->valuestring, cid->valuestring);
            snprintf(payload, sizeof(payload), "{\"status\": \"ok\", \"correlation_id\": \"%s\"}", cid->valuestring);

            mosquitto_publish(mosq, NULL, topic, strlen(payload), payload, 0, false);
            printf("Responded to %s\n", topic);
            fflush(stdout);
        }

        if (cmd && cid && strcmp(cmd->valuestring, "pots/delete") == 0)
        {
            printf("Received deletion request for %s\n", pot_id);
            fflush(stdout);

            // Respond with status
            char topic[128], payload[128];
            snprintf(topic, sizeof(topic), "pots/%s/delete/%s", pid->valuestring, cid->valuestring);
            snprintf(payload, sizeof(payload), "{\"status\": \"ok\", \"correlation_id\": \"%s\"}", cid->valuestring);

            mosquitto_publish(mosq, NULL, topic, strlen(payload), payload, 0, false);
            printf("Responded to %s\n", topic);
            fflush(stdout);
        }
    } else {
        printf("Received request for invalid pot_id: %s\n", pid ? pid->valuestring : "null");
        fflush(stdout);
    }

    cJSON_Delete(json);
}

int main()
{
    printf("Running...\n");
    fflush(stdout);
    mosquitto_lib_init();
    struct mosquitto *mosq = mosquitto_new(pot_id, true, NULL);

    int ret = mosquitto_connect(mosq, "mqtt", 1883, 60);
    if (ret != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr, "Unable to connect (%d)\n", ret);
        mosquitto_destroy(mosq);
        return 1;
    }

    printf("Connected to broker!\n");
    fflush(stdout);
    mosquitto_message_callback_set(mosq, on_message);

    char topic[128];
    snprintf(topic, sizeof(topic), "pots/%s/add", pot_id);
    mosquitto_subscribe(mosq, NULL, topic, 0);

    snprintf(topic, sizeof(topic), "pots/%s/delete", pot_id);
    mosquitto_subscribe(mosq, NULL, topic, 0);

    mosquitto_loop_forever(mosq, -1, 1);
    mosquitto_destroy(mosq);
    
    mosquitto_lib_cleanup();
    return 0;
}
