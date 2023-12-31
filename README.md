# shellhomenode
Smart node in Shell Home


# Message Format

json

## Sample
``` json
// [hello] term -> node
{
    "term": "Panel_3D6FC9",
    "ds": "2dc991354a3cd5fal13n9f9bckw",
    "index": -1,
    "seq": 23,
    "cmd": "hello",
    "body": {
        "text": "Mini Panel A",
        "auth": "9f1f3kf9cdkf02fde"
    }
}

// [hello] node -> term
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
                "entry": "switch-on",
                "desc": "turn on switch"
            },
            {
                "entry": "switch-off",
                "desc": "turn off switch"
            }
        ]
    }
}

// [forget] term -> node
{
    "term": "Panel_3D6FC9",
    "ds": "2dc991354a3cd5fal13n9f9bckw",
    "index": 0,
    "seq": 24,
    "cmd": "forget",
    "body": {
        "text": "Mini Panel A",
        "auth": "9f1f3kf9cdkf02fde"
    }
}

// [forget] node -> term
{
    "term": "Panel_3D6FC9",
    "seq": 24,
    "cmd": "forget",
    "status": 0,
    "body": {
        "node": "SHN_8DFC84",
        "index": 0,
        "text": "Node 8DFC84",
        "auth": "f3082c23fea"
    }
}

// [request] term -> node
{
    "term": "Panel_3D6FC9",
    "ds": "2dc991354a3cd5fal13n9f9bckw",
    "index": 0,
    "seq": 25,
    "cmd": "request",
    "body": {
        "entry": "switch",
        "params": {
            "slots": [
                {
                    "index": 0,
                    "value": true
                }
            ]
        }
    }
}

// [request] node -> term
{
    "term": "Panel_3D6FC9",
    "seq": 25,
    "cmd": "request",
    "status": 0,
    "body": {
        "entry": "switch",
        "result": {
            "slots": [
                {
                    "index": 0,
                    "value": true
                },
                {
                    "index": 1,
                    "value": false
                },
                {
                    "index": 2,
                    "value": true
                },
                {
                    "index": 3,
                    "value": false
                }
            ]
        }
    }
}


```


# build

``` bash

# IDF Component Registry
idf.py add-dependency "espressif/qrcode^0.1.0"

idf.py add-dependency "espressif/mdns^1.2.1"

idf.py add-dependency "espressif/button^3.0.1"

idf.py add-dependency "espressif/led_strip^2.4.3"

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
