var assert = require('assert')
var cv = require("../build/Release/cloudcv");

exports['test cv.getBuildInformation'] = function() {
    assert.isNotNull(cv.buildInformation());
};