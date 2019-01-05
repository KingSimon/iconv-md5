#include <nan.h>
#include "iconv.h"
#include "Md5.h"
#include <stdarg.h>
#include <string>
#include <vector>

using namespace std;
using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;
using Nan::GetFunction;
using Nan::HandleScope;
using Nan::New;
using Nan::Null;
using Nan::Set;
using Nan::To;
using v8::Function;
using v8::FunctionTemplate;
using v8::Local;
using v8::Number;
using v8::String;
using v8::Value;

/**
 * 功能：格式化字符串
 * 参数：
 *  @pszFmt，格式描述
 *  @...，不定参数
 * 返回值：格式化的结果字符串
 */

#ifdef _WIN32
std::string format(const char *pszFmt, ...)
{
    std::string str;
    va_list args;
    va_start(args, pszFmt);
    {
        int nLength = _vscprintf(pszFmt, args);
        nLength += 1; //上面返回的长度是包含\0，这里加上
        std::vector<char> vectorChars(nLength);
        _vsnprintf(vectorChars.data(), nLength, pszFmt, args);
        str.assign(vectorChars.data());
    }
    va_end(args);
    return str;
}
#else
int _vscprintf(const char *format, va_list pargs)
{
    int retval;
    va_list argcopy;
    va_copy(argcopy, pargs);
    retval = vsnprintf(NULL, 0, format, argcopy);
    va_end(argcopy);
    return retval;
}
std::string format(const char *pszFmt, ...)
{
    std::string str;
    va_list args;
    va_start(args, pszFmt);
    {

        int nLength = _vscprintf(pszFmt, args);
        nLength += 1; //上面返回的长度是包含\0，这里加上
        std::vector<char> vectorChars(nLength);
        vsnprintf(vectorChars.data(), nLength, pszFmt, args);
        str.assign(vectorChars.data());
    }
    va_end(args);
    return str;
}
#endif

//编码转换，source_charset是源编码，to_charset是目标编码
std::string code_convert(ICONV_CONST char *source_charset, char *to_charset, const std::string &sourceStr) //sourceStr是源编码字符串
{
    iconv_t cd = iconv_open(to_charset, source_charset); //获取转换句柄，void类型
    if (cd == 0)
        return "";

    size_t inlen = sourceStr.size();
    size_t outlen = inlen * 2;
    ICONV_CONST char *inbuf = sourceStr.c_str();
    //    char outbuf[255]; //这里实在不知道需要多少个字节，这是个问题
    char *outbuf = new char[outlen]; //另外outbuf不能在堆上分配内存，否则转换失败，猜测跟iconv函数有关
    memset(outbuf, 0, outlen);

    char *poutbuf = outbuf; //多加这个转换是为了避免iconv这个函数出现char()[255]类型的实参与char**类型的形参不兼容
    if (iconv(cd, &inbuf, &inlen, &poutbuf, &outlen) == -1)
        return "";

    std::string strTemp(outbuf); //此时的strTemp为转换编码之后的字符串
    iconv_close(cd);
    return strTemp;
}

// Simple synchronous access to the `Estimate()` function
NAN_METHOD(convertMD5)
{

    Nan::Utf8String input_str(info[0]);
    Nan::Utf8String from_str(info[1]);
    Nan::Utf8String to_str(info[2]);
    int bit = To<int>(info[3]).FromJust();

    char *input = *input_str;
    char *from = *from_str;
    char *to = *to_str;
    string output = code_convert(from, to, input);

    int i;
    int len = output.size() + 1;
    unsigned char *encrypt = new unsigned char[len];
    unsigned char decrypt[16];
    for (i = 0; i < output.length(); i++)
        encrypt[i] = output[i];
    encrypt[i] = '\0';

    MD5_CTX md5;

    MD5Init(&md5);
    MD5Update(&md5, encrypt, (int)strlen((char *)encrypt)); //只是个中间步骤
    MD5Final(&md5, decrypt);                                //32位
    string md5_str = "";
    if (bit == 16)
    {
        for (i = 4; i < 12; i++)
        {
            md5_str += format("%02x", decrypt[i]);
        }
    }
    else
    {
        for (i = 0; i < 16; i++)
        {
            md5_str += format("%02x", decrypt[i]);
        }
    }

    info.GetReturnValue().Set(Nan::New(md5_str).ToLocalChecked());
}

// Expose synchronous and asynchronous access to our
// Estimate() function
NAN_MODULE_INIT(InitAll)
{
    Set(target, New<String>("convertMD5").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(convertMD5)).ToLocalChecked());
}

NODE_MODULE(addon, InitAll)
