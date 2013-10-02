{
    "targets": [
        {
            "target_name": "cloudcv",
            "sources": [ 
                "main.cpp", 
                
                "src/node/node_helpers.hpp", 
                "src/node/node_helpers.cpp",

                "src/modules/analyze/analyze.cpp", 
                "src/modules/analyze/analyze.hpp", 
                "src/modules/analyze/binding.hpp", 
                "src/modules/analyze/binding.cpp", 
            ],
            'include_dirs': [
              'src/',
            ],

                                    'libraries': [
                                        "/usr/local/lib/libopencv_contrib.a",
                                        "/usr/local/lib/libopencv_stitching.a",
                                        "/usr/local/lib/libopencv_nonfree.a",
                                        "/usr/local/lib/libopencv_ts.a", 
                                        "/usr/local/lib/libopencv_videostab.a",
                                        "/usr/local/lib/libopencv_gpu.a", 
                                        "/usr/local/lib/libopencv_legacy.a", 
                                        "/usr/local/lib/libopencv_ml.a", 
                                        "/usr/local/lib/libopencv_objdetect.a", 
                                        "/usr/local/lib/libopencv_calib3d.a", 
                                        "/usr/local/lib/libopencv_photo.a", 
                                        "/usr/local/lib/libopencv_video.a", 
                                        "/usr/local/lib/libopencv_features2d.a", 
                                        "/usr/local/lib/libopencv_highgui.a", 
                                        "/usr/local/share/OpenCV/3rdparty/lib/liblibjasper.a", 
                                        "/usr/local/share/OpenCV/3rdparty/lib/liblibtiff.a", 
                                        "/usr/local/share/OpenCV/3rdparty/lib/liblibpng.a", 
                                        "/usr/local/share/OpenCV/3rdparty/lib/liblibjpeg.a", 
                                        "/usr/local/lib/libopencv_flann.a", 
                                        "/usr/local/lib/libopencv_imgproc.a", 
                                        "/usr/local/lib/libopencv_core.a", 
                                        "/usr/local/share/OpenCV/3rdparty/lib/libzlib.a"
                                        ]

        }
    ]
}
