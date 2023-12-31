#### b-parasite ESPHome component

This is an esphome component based on the [original component](https://esphome.io/components/sensor/b_parasite.html) [(source)](https://github.com/esphome/esphome/tree/dev/esphome/components/b_parasite) with a few changes so it works with the newer versions of the b-parasite firmware. 

### limitations 

Mainly I followed the bthome.io documentation regarding the bthome protocol. It is noteworthy that this is a lazy implementation of the bthome protocol which only works if the values are passed in the same order is expected, however it will show errors in case that the order is not as expected. There is no backwards compatiblity for the original implemenation implemented.

### how to use

note that the component platform is called b_parasite_dev to avoid same-name issues with the existing (but for me broken) b-parasite component in ESPHome. The features and further use should however be mostly indentical to the ones of the [original component](https://esphome.io/components/sensor/b_parasite.html).

```yaml
external_components:
  - source: <root folder which contains this repository as a subfolder, see more on https://esphome.io/components/external_components.html>
sensor:
  - id: bparasite
    platform: b_parasite_dev
    mac_address: 12:34:56:78:90:AB
    humidity:
      name: 'b-parasite Air Humidity'
    temperature:
      name: 'b-parasite Air Temperature'
    moisture:
      name: 'b-parasite Soil Moisture'
    battery_voltage:
      name: 'b-parasite Battery Voltage'
    illuminance:
      name: 'b-parasite Illuminance'
```
