#include "ImageSource.hpp"
#include <nan.h>

using namespace v8;
using namespace node;

namespace cloudcv
{
    class FileImageSource : public ImageSource
    {
    public:
        FileImageSource(const std::string& imagePath, int prefferedColorspace = cv::IMREAD_COLOR)
            : mFilePath(imagePath)
            , mFlags(prefferedColorspace)
        {            
        }

        virtual ~FileImageSource()
        {
        }

        cv::Mat getImage()
        {
            return cv::imread(mFilePath, mFlags);
        }

    private:
        std::string mFilePath;
        int         mFlags;
    };

    class BufferImageSource : public ImageSource
    {
    public:
        BufferImageSource(Local<Object> imageBuffer, int prefferedColorspace = cv::IMREAD_COLOR)
            : mFlags(prefferedColorspace)
        {
            NanAssignPersistent(mImageBuffer, imageBuffer);

            mImageData    = Buffer::Data(imageBuffer);
            mImageDataLen = Buffer::Length(imageBuffer);
        }

        virtual ~BufferImageSource()
        {
            NanDisposePersistent(mImageBuffer);
        }

        cv::Mat getImage()
        {
            return cv::imdecode(cv::_InputArray(mImageData, mImageDataLen), mFlags);
        }

    private:
        Persistent<Object>       mImageBuffer;
        char                   * mImageData;
        int                      mImageDataLen;
        int                      mFlags;
    };

    ImageSourcePtr CreateImageSource(const std::string& filepath)
    {
        return ImageSourcePtr(new FileImageSource(filepath));
    }

    ImageSourcePtr CreateImageSource(v8::Local<v8::Object> imageBuffer)
    {
        return ImageSourcePtr(new BufferImageSource(imageBuffer));
    }

}