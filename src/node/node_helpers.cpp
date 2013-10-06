#include "node_helpers.hpp"

#include <iterator>
#include <iostream>

using namespace v8;
using namespace node;

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};

static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};


void base64_encode(const std::vector<unsigned char>& data, std::vector<unsigned char>& encoded_data)
{
    int input_length = data.size();
    int output_length = 4 * ((input_length + 2) / 3);
    encoded_data.resize(output_length);

    for (int i = 0, j = 0; i < input_length;)
    {
        uint32_t octet_a = i < input_length ? data[i++] : 0;
        uint32_t octet_b = i < input_length ? data[i++] : 0;
        uint32_t octet_c = i < input_length ? data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[output_length - 1 - i] = '=';
}

//////////////////////////////////////////////////////////////

ObjectBuilder::ObjectBuilder(NodeObject& obj)
    : m_object(obj)
{
}

ObjectBuilder& ObjectBuilder::Set(const char * name, int value)
{
    m_object->Set(String::NewSymbol(name), Int32::New(value)); 
    return *this;
}

ObjectBuilder& ObjectBuilder::Set(const char * name, size_t value)
{
    m_object->Set(String::NewSymbol(name), Uint32::New(value)); 
    return *this;
}

ObjectBuilder& ObjectBuilder::Set(const char * name, float value)
{ 
    m_object->Set(String::NewSymbol(name), Number::New(value)); 
    return *this;
}

ObjectBuilder& ObjectBuilder::Set(const char * name, double value)
{
    m_object->Set(String::NewSymbol(name), Number::New(value));
    return *this;
}

ObjectBuilder& ObjectBuilder::Set(const char * name, const char * value)
{ 
    m_object->Set(String::NewSymbol(name), String::New(value)); 
    return *this;
}

ObjectBuilder& ObjectBuilder::Set(const char * name, const std::string& value)
{
    m_object->Set(String::NewSymbol(name), String::New(value.c_str(), value.size()));
    return *this;
}




ObjectBuilder& ObjectBuilder::Set(const char * name, const cv::Size& size)
{
    HandleScope scope;

    Local<Object> sizeObj = Object::New();

    ObjectBuilder sizeBuilder(sizeObj);
    sizeBuilder.Set("width",  size.width);
    sizeBuilder.Set("height", size.height);

    m_object->Set(String::NewSymbol(name), sizeObj);
    return *this;
}

//////////////////////////////////////////////////////////////

/*
unsigned char *base64_decode(const char *data,
                             size_t input_length,
                             size_t *output_length) {

    if (decoding_table == NULL) build_decoding_table();

    if (input_length % 4 != 0) return NULL;

    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;

    unsigned char *decoded_data = malloc(*output_length);
    if (decoded_data == NULL) return NULL;

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6)
        + (sextet_b << 2 * 6)
        + (sextet_c << 1 * 6)
        + (sextet_d << 0 * 6);

        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return decoded_data;
}
**/

std::string toDataUri(const cv::Mat& img, const char * imageMimeType)
{
    std::vector<unsigned char> buf, encoded;
    cv::imencode(".png", img, buf);
    
    base64_encode(buf, encoded);

    std::ostringstream os;
    os << "data:" << imageMimeType << ";base64,";
    std::copy(encoded.begin(), encoded.end(), std::ostream_iterator<unsigned char>(os));
    return os.str();
}

std::string humanSize(size_t sizeInBytes)
{
    std::ostringstream str;

    str << sizeInBytes;

    return str.str();
}
