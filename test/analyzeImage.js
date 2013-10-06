var assert = require('assert');
var fs = require('fs');

var cv = require("../build/Release/cloudcv");

exports['test cv.analyzeImage'] = function() {

	var imageData = fs.readFileSync("example.jpg");
	assert.isNotNull(imageData);

	cv.analyzeImage(imageData, function(result) {

		console.log(result);
		assert.isNotNull(result);
	});

};