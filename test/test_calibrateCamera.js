var assert = require("assert")
var fs     = require('fs');
var inspect = require('util').inspect;

var cloudcv = require("../build/Release/cloudcv");
//var cloudcv = require("../build/Debug/cloudcv");

describe('cv', function() {

    describe('calibrateCamera', function() {
        it('detectChessboardGrid', function(done) {
            var imageData = fs.readFileSync('test/grid-pattern.png');
            assert.notEqual(null, imageData);

            var patternSize = { width:9, height:6 };

            var retVal = cloudcv.calibrationPatternDetect(imageData, patternSize, 'CHESSBOARD', function(error, result) {

                assert.equal(null, error);
                assert.notEqual(null, result);
                done();
            });

            //assert.equal(undefined, retVal);
        });

        it('detectCirclesGrid', function(done) {
            var imageData = fs.readFileSync('test/circles_pattern.jpg');
            assert.notEqual(null, imageData);
            var patternSize = { width:8, height:9 };

            var retVal = cloudcv.calibrationPatternDetect(imageData, patternSize, 'CIRCLES_GRID', function(error, result) {
                assert.equal(null, error);
                assert.notEqual(null, result);
                done();
            }); 

            //assert.equal(undefined, retVal);
        });

        it('detectAsymetricCirclesGrid', function(done) {
            var imageData = fs.readFileSync('test/acircles1.png');
            assert.notEqual(null, imageData);
            var patternSize = { width:8, height:9 };

            var retVal = cloudcv.calibrationPatternDetect(imageData, patternSize, 'ACIRCLES_GRID', function(error, result) {
                assert.equal(null, error);
                assert.notEqual(null, result);
                done();
            }); 

            //assert.equal(undefined, retVal);
        });
    });
});