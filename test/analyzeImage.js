var assert = require('assert');
var fs = require('fs');

var cv = require("../build/Release/cloudcv");

exports['test cv.analyze'] = function() {

	var imageData = fs.readFileSync("test/example.jpg");
	assert.isNotNull(imageData);

	cv.analyze(imageData, function(result) {

		console.log(result);
		assert.isNotNull(result);
	});

};