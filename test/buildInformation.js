var assert = require('assert');
var cv = require("../build/Release/cloudcv");

describe('cv', function() {
    it('buildInformation', function(done) {
        var info = cv.buildInformation();
        assert.notEqual(null, info);
        done();
    });

    it('version check', function(done) {
        var ver = cv.version();
        assert.notEqual(null, ver);
        done();
    });
});