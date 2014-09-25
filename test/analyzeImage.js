var assert = require("assert")
var fs     = require('fs');
var inspect = require('util').inspect;

var cv = require("../build/Release/cloudcv");


describe('cv', function() {

    describe('analyzeImage', function() {
        it('should run without error', function(done) {
            var imageData = fs.readFileSync("test/opencv-logo.jpg");
            cv.analyzeImage(imageData, function(error, result) { 
                //console.log(error, result);
                   
                assert.notStrictEqual(result, undefined);
                assert.equal(4, result.dominantColors.length);
                
                result.quantizedColors.asJpegStream(function(err, jpegImg) {
                    fs.writeFile('quantizedColors.jpg', new Buffer(jpegImg));
                    done();
                });
                /**/
            });
        });
    });
});