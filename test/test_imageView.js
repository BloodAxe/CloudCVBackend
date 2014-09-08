var assert = require("assert")
var fs     = require('fs');

var cloudcv = require("../build/Release/cloudcv");

describe('cv', function() {

    describe('ImageView', function() {
        it('should run without error', function(done) {

            var imview = new cloudcv.ImageView("test/opencv-logo.jpg");
            assert.equal(599, imview.width());
            assert.equal(555, imview.height());
            done();
        });
    });
});