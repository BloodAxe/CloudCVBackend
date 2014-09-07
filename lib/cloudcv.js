var cvBackend = require('../build/Release/cloudcv')
//var cvBackend = require('../build/Debug/cloudcv')

exports.analyzeImage = function (image, callback) {

    console.log('Calling native code');
    return cvBackend.analyzeImage(image, callback);
}

exports.buildInformation = function() {
    return cvBackend.buildInformation();
}