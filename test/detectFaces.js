var assert = require('assert');
var fs = require('fs');

var cv = require("../build/Release/cloudcv");

exports['test cv.detectFaces'] = function() {

    var imageData = fs.readFileSync("test/lena.png");
    assert.isNotNull(imageData);

    cv.detectFaces(imageData, function(result) {

        console.log(result);
        assert.isNotNull(result);
    });

};