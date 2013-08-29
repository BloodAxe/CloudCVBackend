{
  "targets": [
    {
      "target_name": "cloudcv-backend",
      "sources": [ "main.cpp" ],

      'libraries': [
          '<!@(pkg-config --libs opencv)'
        ]
      
    }
  ]
}