{
    "targets": [
        {
            "target_name": "cloudcv",
            "sources": [ "main.cpp" ],
            "include_dirs": [ "/var/lib/stickshift/5218f7a84382ec173a0002ae/app-root/data/lib/opencv/include" ],
            "link_settings": {
                        'library_dirs': ["/var/lib/stickshift/5218f7a84382ec173a0002ae/app-root/data/lib/opencv/lib/"],
                        'libraries':    ['-lopencv_core -lopencv_features2d -lopencv_contrib']
                       }
        }
    ]
}