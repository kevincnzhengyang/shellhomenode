# Simple  Switch

Simple Switch in state on/off

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

