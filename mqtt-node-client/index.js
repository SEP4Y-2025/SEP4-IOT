const mqtt = require("mqtt");
const client = mqtt.connect("mqtt://localhost:1883");

client.subscribe("sensor/telemetry", (err) => {
  if (err) {
    console.error("Subscription error:", err);
    process.exit(1);
  }
  console.log("Subscribed to sensor/telemetry");
});

client.on("message", (topic, message) => {
  console.log("▶️ Topic:", topic);
  console.log("📨 Payload:", message.toString());
});
