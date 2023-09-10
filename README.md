# shellhomenode
Smart node in Shell Home





# build

``` bash

# IDF Component Registry
idf.py add-dependency "espressif/qrcode^0.1.0"

idf.py add-dependency "espressif/mdns^1.2.1"

idf.py add-dependency "espressif/button^3.0.1"

```


``` bash
avahi-browse -tpk -r _shnode._udp
```
