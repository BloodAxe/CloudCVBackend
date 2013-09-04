{
    
    
    "targets": [
        {
            "target_name": "cloudcv",
            "sources": [ "main.cpp" ],
            'libraries': [
                '<!@(pkg-config --libs opencv)'
            ]
        }
    ]
}
