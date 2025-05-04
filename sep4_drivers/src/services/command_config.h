#ifndef COMMAND_CONFIG_H
#define COMMAND_CONFIG_H

/**  
 * Register all your MQTT topic → handler mappings.  
 * Call this once, before `command_service_init()`.  
 */
void command_config_register_all(void);

#endif // COMMAND_CONFIG_H
