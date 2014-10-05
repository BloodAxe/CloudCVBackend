{

    'target_defaults': {
        'default_configuration': 'Release',
    },

    'targets': [
        {
            'target_name': "cloudcv",

            'sources': [ 
                "src/cloudcv.cpp", 
                "src/cloudcv.hpp",

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
                "src/modules/analyze/binding.cpp", 
                "src/modules/analyze/dominantColors.hpp", 
                "src/modules/analyze/dominantColors.cpp", 

                "src/modules/buildInformation/buildInformation.cpp", 

                "src/modules/cameraCalibration/CameraCalibrationBinding.cpp", 
            ],

            'include_dirs': [
              'src/',
              "<!(node -e \"require('nan')\")",
              "<!(node -e \"require('opencv').include_dirs()\")"
            ],

            'conditions': [
            
                ['OS=="mac"', {
                
                    'defines': [
                        'TARGET_PLATFORM_MAC',
                    ],

                    'include_dirs': [
                    ],
                    
                    'libraries': [
                        "<!(node -e \"require('opencv').libraries()\")"
                    ],

                    'xcode_settings': {
                        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                        'OTHER_CFLAGS': [ '-g', '-mmacosx-version-min=10.7', '-std=c++11', '-stdlib=libc++', '-O3', '-Wall' ],
                        'OTHER_CPLUSPLUSFLAGS': [ '-g', '-mmacosx-version-min=10.7', '-std=c++11', '-stdlib=libc++', '-O3', '-Wall' ]
                    }
                }],

                
                ['OS=="linux"', {
                
                    'defines': [
                        'TARGET_PLATFORM_LINUX',
                    ],

                    'include_dirs': [
                        '/usr/local/include'
                    ],

                    "cflags": [
                        "-std=c++11", "-stdlib=libc++"
                    ],
                    
                    'libraries': [
                    ]
                }]       
            ]
        }
    ]
}
