var assert = require('assert');
var cloudcv = require("../cloudcv.js");

describe('cv', function() {
    it('buildInformation', function(done) {
        var info = cloudcv.buildInformation();
        assert.notEqual(null, info);
        done();
    });

    it('version check', function(done) {
        var ver = cloudcv.version();
        assert.notEqual(null, ver);
        done();
    });
});