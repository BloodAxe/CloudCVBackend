var assert = require("assert")
var fs     = require('fs');
var inspect = require('util').inspect;

var cloudcv = require("../cloudcv.js");

describe('cv', function() {

    describe('analyzeImage', function() {
        
        it('process (Buffer)', function(done) {
            var imageData = fs.readFileSync("test/opencv-logo.jpg");
            cloudcv.analyzeImage(imageData, function(error, result) { 
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

        it('process (File)', function(done) {
            cloudcv.analyzeImage("test/opencv-logo.jpg", function(error, result) { 
                   
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