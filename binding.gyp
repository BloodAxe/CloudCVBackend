{

    'target_defaults': {
        'default_configuration': 'Release',
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

                "src/node/node_helpers.hpp", 
                "src/node/node_helpers.cpp",

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

                    'configurations': {

                        'Debug': {
                            'defines': [ 'DEBUG', '_DEBUG' ],
                            'msvs_settings': {
                                'VCCLCompilerTool': {
                                    'RuntimeLibrary': 2, # static debug
                                },
                            },
                        },

                        'Release': {
                            'defines': [ 'NDEBUG' ],
                            'msvs_settings': {
                                'VCCLCompilerTool': {
                                    'RuntimeLibrary': 2, # static release
                                },
                            },
                        }
                    },

                    'defines': [
                        'TARGET_PLATFORM_WINDOWS',
                    ],

                    'variables': { 
                        'opencv_root': '<!(echo %OPENCV_ROOT%)' 
                    },

                    'include_dirs': [
                        '<(opencv_root)/include'
                    ],

                    'libraries': [
                        "<(opencv_root)/x86/vc11/staticlib/opencv_calib3d247.lib",
                        "<(opencv_root)/x86/vc11/staticlib/opencv_contrib247.lib",
                        "<(opencv_root)/x86/vc11/staticlib/opencv_core247.lib",
                        "<(opencv_root)/x86/vc11/staticlib/opencv_features2d247.lib",
                        "<(opencv_root)/x86/vc11/staticlib/opencv_flann247.lib",
                        "<(opencv_root)/x86/vc11/staticlib/opencv_highgui247.lib",
                        "<(opencv_root)/x86/vc11/staticlib/opencv_imgproc247.lib",
                        "<(opencv_root)/x86/vc11/staticlib/opencv_legacy247.lib",
                        "<(opencv_root)/x86/vc11/staticlib/opencv_ml247.lib",
                        "<(opencv_root)/x86/vc11/staticlib/opencv_nonfree247.lib",
                        "<(opencv_root)/x86/vc11/staticlib/opencv_objdetect247.lib",
                        "<(opencv_root)/x86/vc11/staticlib/opencv_photo247.lib",
                        "<(opencv_root)/x86/vc11/staticlib/opencv_stitching247.lib",
                        "<(opencv_root)/x86/vc11/staticlib/opencv_superres247.lib",
                        "<(opencv_root)/x86/vc11/staticlib/opencv_ts247.lib",
                        "<(opencv_root)/x86/vc11/staticlib/opencv_video247.lib",
                        "<(opencv_root)/x86/vc11/staticlib/opencv_videostab247.lib",
                        "<(opencv_root)/x86/vc11/staticlib/libjpeg.lib",
                        "<(opencv_root)/x86/vc11/staticlib/libtiff.lib",
                        "<(opencv_root)/x86/vc11/staticlib/libpng.lib",
                        "<(opencv_root)/x86/vc11/staticlib/zlib.lib"
                    ]
                }],

                ['OS=="mac"', {
                
                    'defines': [
                        'TARGET_PLATFORM_MAC',
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
