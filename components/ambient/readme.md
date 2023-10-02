# ambient

ambient sensor

# Request JSON

```json
// ambient light
// [request] term -> node
{
    "term": "Panel_3D6FC9",
    "ds": "2dc991354a3cd5fal13n9f9bckw",
    "index": 0,
    "seq": 25,
    "cmd": "request",
    "body": {
        "entry": "sal_get"
    }
}

// [request] node -> term
{
    "term": "Panel_3D6FC9",
    "seq": 25,
    "cmd": "request",
    "status": 0,
    "body": {
        "entry": "sal_get",
        "result": {
            "lux": 45
        }
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
        "entry": "sal_sub",
        "params": {
            "mqtt": {
                "ip": "192.168.1.100",
                "port": 1883,
                "topic": "livingroom-light"
            },
            "udp": {
                "ip": "192.168.1.100",
                "port": 10883
            },
            "period": 10
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
        "entry": "sal_sub",
        "result": {
            "lux": 45
        }
    }
}

```