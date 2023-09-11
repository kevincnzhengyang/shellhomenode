# shellhomenode
Smart node in Shell Home



# Message Format

json

## Sample
``` json
// term -> node
{
    "term": "Panel_3D6FC9",
    "token": "2dc991354a3cd5fal13n9f9bckw",
    "index": -1,
    "seq": 23,
    "cmd": "hello",
    "body": {
        "text": "Mini Panel A",
        "auth": "9f1f3kf9cdkf02fde"
    }
}

// node -> term
{
    "term": "Panel_3D6FC9",
    "seq": 23,
    "cmd": "hello",
    "status": 0,
    "body": {
        "node": "SHN_8DFC84",
        "index": 0,
        "seq": 47,
        "text": "Node 8DFC84",
        "auth": "f3082c23fea",
        "api": [
            {
                "cmd": "switch-on",
                "desc": "turn on switch"
            },
            {
                "cmd": "switch-off",
                "desc": "turn off switch"
            }
        ]
    }
}
```


# build

``` bash

# IDF Component Registry
idf.py add-dependency "espressif/qrcode^0.1.0"

idf.py add-dependency "espressif/mdns^1.2.1"

idf.py add-dependency "espressif/button^3.0.1"

```


``` bash
avahi-browse -tpk -r _shnode._udp

#Status (+ or =)
#Network interface (enx00000000b000)
#IP type (4 or 6)
#Service name ("sybo")
#Service type ("Remote Disk Management")
#Domain ("local")
```
