var assert = require("assert")
var fs     = require('fs');

var cloudcv = require("../lib/cloudcv");


describe('cv', function() {

    describe('analyzeImage', function() {
        it('should run without error', function(done) {
            var imageData = fs.readFileSync("test/opencv-logo.jpg");
            cloudcv.analyzeImage(imageData, function(error, result) {    
                assert.notStrictEqual(result, undefined);
                assert.equal(4, result.dominantColors.length);
                done();
            });
        });
    });
});