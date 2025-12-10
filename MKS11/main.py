import paho.mqtt.client as mqtt
import json
import base64

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connected with result code {reason_code}")
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("v3/mpc-mks@ttn/devices/devkit-72/up")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    try:
        payload_str = msg.payload.decode("utf-8")
        pld = json.loads(payload_str)

        uplink = pld.get("uplink_message", {})
        frm_payload = uplink.get("frm_payload", "")
        decoded_msg = base64.b64decode(frm_payload).decode() if frm_payload else "N/A"

        time = uplink.get("settings", {}).get("time", "N/A")
        frequency = uplink.get("settings", {}).get("frequency")

        if frequency is not None:
            try:
                frequency = float(frequency)
                frequency_mhz = frequency / 1_000_000
            except ValueError:
                frequency_mhz = "N/A"
        else:
            frequency_mhz = "N/A"

        print("\n📡 New Uplink Message")
        print(f"⏰ Time: {time}")
        print(f"📶 Frequency: {frequency_mhz} MHz")
        print(f"💬 Decoded Message: {decoded_msg}")

    except Exception as e:
        print(f"⚠️ Error processing message: {e}")
        print(msg.payload)

mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
mqttc.on_connect = on_connect
mqttc.on_message = on_message
mqttc.username_pw_set("mpc-mks@ttn", "NNSXS.B75L4YCIWKRIXUGEDS2NNGXTNAV4PHW7POSNS2Y.EE42KQNJKP36YCSLRH5SIU4I5XXC2KOQGJPIT3PRK4PKNMPDNIVQ")

mqttc.connect("eu1.cloud.thethings.network", 1883, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
mqttc.loop_forever()