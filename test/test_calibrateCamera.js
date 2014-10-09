var assert = require("assert")
var fs     = require('fs');
var inspect = require('util').inspect;

var cloudcv = require("../cloudcv.js");

describe('cv', function() {

    describe('calibrateCamera', function() {
        
        it('detectChessboardGrid (Buffer)', function(done) {
            var imageData = fs.readFileSync('test/grid-pattern.png');
            assert.notEqual(null, imageData);

            var patternSize = { width:9, height:6 };

            cloudcv.calibrationPatternDetect(imageData, patternSize, 'CHESSBOARD', function(error, result) {

                assert.equal(null, error);
                assert.notEqual(null, result);
                done();
            });
        });

        it('detectChessboardGrid (File)', function(done) {
            var patternSize = { width:9, height:6 };

            cloudcv.calibrationPatternDetect('test/grid-pattern.png', patternSize, 'CHESSBOARD', function(error, result) {

                assert.equal(null, error);
                assert.notEqual(null, result);
                done();
            });
        });

        it('detectCirclesGrid', function(done) {
            var imageData = fs.readFileSync('test/circles_pattern.jpg');
            assert.notEqual(null, imageData);
            var patternSize = { width:8, height:9 };

            cloudcv.calibrationPatternDetect(imageData, patternSize, 'CIRCLES_GRID', function(error, result) {
                assert.equal(null, error);
                assert.notEqual(null, result);
                done();
            }); 
        });

        it('detectAsymetricCirclesGrid', function(done) {
            var imageData = fs.readFileSync('test/acircles1.png');
            assert.notEqual(null, imageData);
            var patternSize = { width:8, height:9 };

            cloudcv.calibrationPatternDetect(imageData, patternSize, 'ACIRCLES_GRID', function(error, result) {
                assert.equal(null, error);
                assert.notEqual(null, result);
                done();
            }); 
        });
        
        it('calibrateChessboard', function(done) {

            this.timeout(10000);

            var images = [
                'test/calibration_1/grid_01.jpg',
                'test/calibration_1/grid_02.jpg',
                'test/calibration_1/grid_03.jpg',
                'test/calibration_1/grid_04.jpg',
                'test/calibration_1/grid_05.jpg',
                'test/calibration_1/grid_06.jpg',
                'test/calibration_1/grid_07.jpg',
                'test/calibration_1/grid_08.jpg',
                'test/calibration_1/grid_09.jpg',
                'test/calibration_1/grid_10.jpg',
                'test/calibration_1/grid_11.jpg',
            ];
            var patternSize = { width:9, height:6 };
            cloudcv.calibrateCamera(images, patternSize, 'CHESSBOARD', function(error, result) {
                assert.equal(null, error);
                assert.notEqual(null, result);
                console.log(result.intrinsic.asObject());
                console.log(result.distCoeffs.asObject());
                done();
            });
        });
        /**/
        
        it('calibrateCirclesGrid', function(done) {
            var images = [
                'test/calibration_2/grid_01.png',
                'test/calibration_2/grid_02.png',
                'test/calibration_2/grid_03.png',
                'test/calibration_2/grid_04.png',
                'test/calibration_2/grid_05.png',
                'test/calibration_2/grid_06.png',
                'test/calibration_2/grid_07.png',
                'test/calibration_2/grid_08.png',
                'test/calibration_2/grid_09.png',
                'test/calibration_2/grid_10.png',
            ];
            var patternSize = { width:4, height:11 };
            cloudcv.calibrateCamera(images, patternSize, 'ACIRCLES_GRID', function(error, result) {
                assert.equal(null, error);
                assert.notEqual(null, result);

                console.log(result.intrinsic.asObject());
                console.log(result.distCoeffs.asObject());
                done();
            });
        });
        /**/
    });
});