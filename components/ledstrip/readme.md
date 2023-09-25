# LED Strip

LED strip

# Request JSON

```json
// [request] term -> node
{
    "term": "Panel_3D6FC9",
    "ds": "2dc991354a3cd5fal13n9f9bckw",
    "index": 0,
    "seq": 25,
    "cmd": "request",
    "body": {
        "entry": "dimmable_light",
        "params": {
            "on": true,
            "r": 0,
            "g": 0,
            "b": 255
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
        "entry": "dimmable_light",
        "result": {
            "on": true
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
        "entry": "marquee",
        "params": {
            "on": true,
            "period": 20
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
        "entry": "marquee",
        "result": {
            "on": true
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
        "entry": "breathing_light",
        "params": {
            "on": true,
            "period": 100
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
        "entry": "breathing_light",
        "result": {
            "on": true
        }
    }
}
```