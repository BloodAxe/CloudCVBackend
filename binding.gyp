{

    'target_defaults': {
        'default_configuration': 'Release',

        # Replace with if you want to have debug build 
        # 'opencv_lib_suffix' : 'd',

        'variables': { 
            'opencv_lib_suffix': ''
        }
    },

    'targets': [
        {
            'target_name': "cloudcv",

            'sources': [ 
                "main.cpp", 
                
                "src/framework/marshal/marshal.hpp",                
                "src/framework/marshal/marshal.cpp",

                "src/framework/marshal/stl.hpp",                
                "src/framework/marshal/stl.cpp",
                
                "src/framework/marshal/opencv.hpp",                
                "src/framework/marshal/opencv.cpp",
                
                "src/framework/marshal/primitives.hpp",                
                "src/framework/marshal/primitives.cpp",

                "src/framework/marshal/node_object_builder.hpp",
                "src/framework/marshal/node_object_builder.cpp",
                
                "src/framework/Image.hpp",                
                "src/framework/Image.cpp",

                "src/framework/Job.hpp",                
                "src/framework/Job.cpp",

                "src/framework/NanCheck.hpp",
                "src/framework/NanCheck.cpp",
                
                "src/modules/common/Numeric.cpp", 
                "src/modules/common/Numeric.hpp",                 

                "src/modules/common/Color.hpp", 
                "src/modules/common/ScopedTimer.hpp", 

                "src/modules/common/ImageUtils.hpp", 
                "src/modules/common/ImageUtils.cpp", 

                "src/modules/analyze/analyze.cpp", 
                "src/modules/analyze/analyze.hpp", 
                "src/modules/analyze/binding.hpp", 
                "src/modules/analyze/binding.cpp", 
                "src/modules/analyze/dominantColors.hpp", 
                "src/modules/analyze/dominantColors.cpp", 

                "src/modules/buildInformation/buildInformation.cpp", 
                "src/modules/buildInformation/buildInformation.hpp", 

                "src/modules/cameraCalibration/CameraCalibrationBinding.cpp", 
                "src/modules/cameraCalibration/CameraCalibrationBinding.hpp", 
            ],

            'include_dirs': [
              'src/',
              "<!(node -e \"require('nan')\")"
            ],

            'conditions': [
            
                ['OS=="win"', {

                    'variables': { 
                        'opencv_root':          '$(OPENCV_ROOT)',
                    },


                    'configurations': {

                        'Debug': {

                            'defines': [ 'DEBUG', '_DEBUG' ],
                            'msvs_settings': {
                                'VCCLCompilerTool': {
                                    'ExceptionHandling': 1,
                                    'RuntimeLibrary': 3, # dynamic debug
                                },
                            },

                        },

                        'Release': {

                            'defines': [ 'NDEBUG' ],
                            'msvs_settings': {
                                'VCCLCompilerTool': {
                                    'ExceptionHandling': 1,
                                    'RuntimeLibrary': 2, # static release
                                },
                            },
                        }
                    },

                    'defines': [
                        'TARGET_PLATFORM_WINDOWS',
                    ],

                    'include_dirs': [
                        '<(opencv_root)/include'
                    ],

                    'libraries': [
                                "<(opencv_root)/x64/vc12/staticlib/opencv_calib3d300>(opencv_lib_suffix).lib",
                                "<(opencv_root)/x64/vc12/staticlib/opencv_contrib300>(opencv_lib_suffix).lib",
                                "<(opencv_root)/x64/vc12/staticlib/opencv_core300>(opencv_lib_suffix).lib",
                                "<(opencv_root)/x64/vc12/staticlib/opencv_features2d300>(opencv_lib_suffix).lib",
                                "<(opencv_root)/x64/vc12/staticlib/opencv_flann300>(opencv_lib_suffix).lib",
                                "<(opencv_root)/x64/vc12/staticlib/opencv_highgui300>(opencv_lib_suffix).lib",
                                "<(opencv_root)/x64/vc12/staticlib/opencv_imgproc300>(opencv_lib_suffix).lib",
                                "<(opencv_root)/x64/vc12/staticlib/opencv_legacy300>(opencv_lib_suffix).lib",
                                "<(opencv_root)/x64/vc12/staticlib/opencv_ml300>(opencv_lib_suffix).lib",
                                "<(opencv_root)/x64/vc12/staticlib/opencv_nonfree300>(opencv_lib_suffix).lib",
                                "<(opencv_root)/x64/vc12/staticlib/opencv_objdetect300>(opencv_lib_suffix).lib",
                                "<(opencv_root)/x64/vc12/staticlib/opencv_photo300>(opencv_lib_suffix).lib",
                                "<(opencv_root)/x64/vc12/staticlib/opencv_stitching300>(opencv_lib_suffix).lib",
                                "<(opencv_root)/x64/vc12/staticlib/opencv_superres300>(opencv_lib_suffix).lib",
                                "<(opencv_root)/x64/vc12/staticlib/opencv_ts300>(opencv_lib_suffix).lib",
                                "<(opencv_root)/x64/vc12/staticlib/opencv_video300>(opencv_lib_suffix).lib",
                                "<(opencv_root)/x64/vc12/staticlib/opencv_videostab300>(opencv_lib_suffix).lib",
                                "<(opencv_root)/x64/vc12/staticlib/libjpeg>(opencv_lib_suffix).lib",
                                "<(opencv_root)/x64/vc12/staticlib/libpng>(opencv_lib_suffix).lib",
                                "<(opencv_root)/x64/vc12/staticlib/zlib>(opencv_lib_suffix).lib"
                            ]

                }],

                ['OS=="mac"', {
                
                    'defines': [
                        'TARGET_PLATFORM_MAC',
                    ],

                    'variables': { 
                        'opencv_root':          '<!(echo $OPENCV_ROOT)',
                        'opencv_libs':          '<!(echo $OPENCV_ROOT)/lib',
                        'opencv_3rdparty_libs': '<!(echo $OPENCV_ROOT)/share/OpenCV/3rdparty/lib',
                    },

                    'include_dirs': [
                        '<(opencv_root)/include'
                    ],
                    
                    'libraries': [
                        "<(opencv_libs)/libopencv_calib3d.a",
                        "<(opencv_libs)/libopencv_contrib.a",
                        "<(opencv_libs)/libopencv_core.a",
                        "<(opencv_libs)/libopencv_features2d.a",
                        "<(opencv_libs)/libopencv_flann.a",
                        "<(opencv_libs)/libopencv_highgui.a",
                        "<(opencv_libs)/libopencv_imgproc.a",
                        "<(opencv_libs)/libopencv_legacy.a",
                        "<(opencv_libs)/libopencv_ml.a",
                        "<(opencv_libs)/libopencv_nonfree.a",
                        "<(opencv_libs)/libopencv_objdetect.a",
                        "<(opencv_libs)/libopencv_optim.a",
                        "<(opencv_libs)/libopencv_photo.a",
                        "<(opencv_libs)/libopencv_shape.a",
                        "<(opencv_libs)/libopencv_softcascade.a",
                        "<(opencv_libs)/libopencv_stitching.a",
                        "<(opencv_libs)/libopencv_superres.a",
                        "<(opencv_libs)/libopencv_ts.a",
                        "<(opencv_libs)/libopencv_video.a",
                        "<(opencv_libs)/libopencv_videostab.a",

                        "<(opencv_3rdparty_libs)/liblibpng.a", 
                        "<(opencv_3rdparty_libs)/liblibjpeg.a", 
                        "<(opencv_3rdparty_libs)/libzlib.a"
                    ],

                    'xcode_settings': {
                        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                        'OTHER_CFLAGS': [ '-g', '-mmacosx-version-min=10.7', '-std=c++11', '-stdlib=libc++', '-O3', '-D__STDC_CONSTANT_MACROS', '-D_FILE_OFFSET_BITS=64', '-D_LARGEFILE_SOURCE', '-Wall' ],
                        'OTHER_CPLUSPLUSFLAGS': [ '-g', '-mmacosx-version-min=10.7', '-std=c++11', '-stdlib=libc++', '-O3', '-D__STDC_CONSTANT_MACROS', '-D_FILE_OFFSET_BITS=64', '-D_LARGEFILE_SOURCE', '-Wall' ]
                    }
                }],

                
                ['OS=="linux"', {
                
                    'defines': [
                        'TARGET_PLATFORM_LINUX',
                    ],

                    'include_dirs': [
                        '/usr/local/include'
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
                        "/usr/local/share/OpenCV/3rdparty/lib/liblibtiff.a", 
                        "/usr/local/share/OpenCV/3rdparty/lib/liblibpng.a", 
                        "/usr/local/share/OpenCV/3rdparty/lib/liblibjpeg.a", 
                        "/usr/local/lib/libopencv_flann.a", 
                        "/usr/local/lib/libopencv_imgproc.a", 
                        "/usr/local/lib/libopencv_core.a", 
                        "/usr/local/share/OpenCV/3rdparty/lib/libzlib.a"
                    ]
                }]       
            ]
        }
    ]
}
