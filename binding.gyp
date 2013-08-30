{
  "targets": [
    {
      "target_name": "cloudcv",
      "sources": [ "main.cpp" ],

      'conditions': [
          ['OS=="linux"', {
            'include_dirs': [ '~/lib/opencv/include/' ],
            'link_settings': {
                        'libraries':    ['-lopencv_core -lopencv_features2d -lopencv_contrib'],
                        'library_dirs': ['~/lib/opencv/lib/'],
                       },
            }
        ],
      ]
    }
  ]
}