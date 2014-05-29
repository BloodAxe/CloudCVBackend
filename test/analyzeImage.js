var assert = require('assert');
var fs = require('fs');

var cv = require("../build/Release/cloudcv");

exports['test cv.analyze'] = function() {

	var imageData = fs.readFileSync("test/opencv-logo.jpg");
	assert.isNotNull(imageData);

	cv.analyze(imageData, function(error, result) {

		assert.isNotNull(result);
        assert.equal(4, result.color.dominantColors.length);
	});
};