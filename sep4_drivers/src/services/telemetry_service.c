// services/telemetry_service.c
#include "services/telemetry_service.h"
#include "controllers/sensor_controller.h"
#include "services/mqtt_service.h"
#include "services/logger_service.h"
#include <stdio.h>
#include "controllers/network_controller.h"

#define JSON_BUF_SZ 128

static const char *_svc_topic;
static char _json_buf[JSON_BUF_SZ];

void telemetry_service_init(const char *topic)
{
    _svc_topic = topic;
    // make sure sensors are ready
    sensor_controller_init();
}

void telemetry_service_poll(void)
{
    // give the MQTT layer a chance to process incoming packets
    mqtt_service_poll();
}

bool telemetry_service_publish(void)
{
    sensor_controller_poll();
    uint8_t tmp_i = sensor_controller_get_temperature_integer();
    uint8_t tmp_d = sensor_controller_get_temperature_decimal();
    uint8_t hum_i = sensor_controller_get_humidity_integer();
    uint8_t hum_d = sensor_controller_get_humidity_decimal();
    uint16_t light = sensor_controller_get_light();
    uint8_t soil = sensor_controller_get_soil();

    // 3) Format JSON payload exactly as before
    int jlen = snprintf(_json_buf, JSON_BUF_SZ,
                        "{\"temperature\":%u.%u,"
                        "\"humidity\":%u.%u,"
                        "\"light\":%u,"
                        "\"soil\":%u}",
                        (unsigned)tmp_i, (unsigned)tmp_d,
                        (unsigned)hum_i, (unsigned)hum_d,
                        (unsigned)light,
                        (unsigned)soil);
    if (jlen < 0 || jlen >= JSON_BUF_SZ)
    {
        return false;
    }

    logger_service_log("Telemetry: JSON payload (%d bytes): %s", jlen, _json_buf);

    return mqtt_service_publish(
        _svc_topic,
        (const uint8_t *)_json_buf,
        (uint16_t)jlen,
        0,    // QoS level
        false // retain flag
    );
    // size_t pktlen = mqtt_controller_build_publish_packet(
    //     _svc_topic,
    //     (unsigned char *)_json_buf,
    //     (size_t)jlen);
    // if (pktlen == 0)
    // {
    //     return false;
    // }

    // // 6) Send it
    // if (!network_controller_tcp_send(_pkt_buf, pktlen))
    // {
    //     return false;
    // }

    // return true;
}

// bool telemetry_service_publish(void)
// {
//     // 1) Ensure Wi-Fi is up
//     if (!wifi_service_is_connected())
//     {
//         return false;
//     }

//     // 2) If needed, open TCP + send MQTT CONNECT
//     if (!svc_mqtt_connected)
//     {
//         if (!network_controller_tcp_open(svc_broker_ip, svc_broker_port))
//         {
//             return false;
//         }
//         size_t connlen = mqtt_controller_build_connect_packet(_pkt_buf, TEL_PKT_BUF_SZ);
//         if (connlen == 0)
//         {
//             return false;
//         }
//         if (!network_controller_tcp_send(_pkt_buf, connlen))
//         {
//             return false;
//         }
//         svc_mqtt_connected = true;
//         // CONNACK will be drained in the next poll()
//     }

//     // 3) Read latest sensors
//     sensor_controller_poll();
//     uint8_t hum_i = sensor_controller_get_humidity_integer();
//     uint8_t hum_d = sensor_controller_get_humidity_decimal();
//     uint8_t tmp_i = sensor_controller_get_temperature_integer();
//     uint8_t tmp_d = sensor_controller_get_temperature_decimal();
//     uint16_t light = sensor_controller_get_light();
//     uint8_t soil = sensor_controller_get_soil();

//     // 4) Format JSON payload without floats
//     //    e.g. {"temperature":27.8,"humidity":33.0,"light":502,"soil":123}
//     int jlen = snprintf(_json_buf, sizeof(_json_buf),
//                         "{\"temperature\":%u.%u,"
//                         "\"humidity\":%u.%u,"
//                         "\"light\":%u,"
//                         "\"soil\":%u}",
//                         (unsigned)tmp_i, (unsigned)tmp_d,
//                         (unsigned)hum_i, (unsigned)hum_d,
//                         (unsigned)light,
//                         (unsigned)soil);
//     if (jlen <= 0)
//     {
//         return false;
//     }

//     // 5) Build MQTT PUBLISH packet (QoS0, no retain)
//     size_t pktlen = mqtt_controller_build_publish_packet(
//         svc_topic,
//         (unsigned char *)_json_buf,
//         (size_t)jlen,
//         _pkt_buf,
//         TEL_PKT_BUF_SZ);
//     if (pktlen == 0)
//     {
//         return false;
//     }

//     // 6) Send it
//     if (!network_controller_tcp_send(_pkt_buf, pktlen))
//     {
//         return false;
//     }

//     return true;
// }
