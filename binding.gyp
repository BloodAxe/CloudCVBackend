{
    "targets": [
        {
            "target_name": "cloudcv",
            "sources": [ "main.cpp" ],
            "include_dirs": [ "/var/lib/stickshift/5218f7a84382ec173a0002ae/app-root/data/lib/opencv/include" ],
            "link_settings": {
                        'library_dirs': ["/var/lib/stickshift/5218f7a84382ec173a0002ae/app-root/data/lib/opencv/lib/"],
                        'libraries':    ['libopencv_core opencv_features2d opencv_contrib']
                       }
        }
    ]
}