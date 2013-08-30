{
    'conditions': [
        [ 'OS=="linux"', { 
            'variables': {
                'opencv_root%': '/var/lib/stickshift/5218f7a84382ec173a0002ae/app-root/data/lib/opencv'
            }, 
        }]
    ],
    
    "targets": [
        {
            "target_name": "cloudcv",
            "sources": [ "main.cpp" ],
            "include_dirs": [ "<(opencv_root)/include" ],
            "link_settings": {
                        'libraries': [
                            "<(opencv_root)/lib/libopencv_bioinspired.a",
                            "<(opencv_root)/lib/libopencv_contrib.a",
                            "<(opencv_root)/lib/libopencv_features2d.a",
                            "<(opencv_root)/lib/libopencv_highgui.a",
                            "<(opencv_root)/lib/libopencv_legacy.a",
                            "<(opencv_root)/lib/libopencv_objdetect.a",
                            "<(opencv_root)/lib/libopencv_photo.a",
                            "<(opencv_root)/lib/libopencv_stitching.a",
                            "<(opencv_root)/lib/libopencv_ts.a",
                            "<(opencv_root)/lib/libopencv_videostab.a",
                            "<(opencv_root)/lib/libopencv_calib3d.a",
                            "<(opencv_root)/lib/libopencv_core.a",
                            "<(opencv_root)/lib/libopencv_flann.a",
                            "<(opencv_root)/lib/libopencv_imgproc.a",
                            "<(opencv_root)/lib/libopencv_ml.a",
                            "<(opencv_root)/lib/libopencv_optim.a",
                            "<(opencv_root)/lib/libopencv_softcascade.a",
                            "<(opencv_root)/lib/libopencv_superres.a",
                            "<(opencv_root)/lib/libopencv_video.a"
                            ]
                       }
        }
    ]
}