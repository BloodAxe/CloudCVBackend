#pragma once 

#include <node.h>
#include <nan.h>
#include <v8.h>
#include <node_buffer.h>
#include <functional>
#include <iostream>
#include <sstream>
#include <array>
#include <map>

#include <framework/marshal/marshal.hpp>



class ArgumentMismatchException
{
public:

    ArgumentMismatchException(const std::string& msg)
        : mMessage(msg)
    {
    }

    ArgumentMismatchException(int actual, int expected)
        : mMessage("Invalid number of arguments passed to a function")
    {
    }

    ArgumentMismatchException(int actual, const std::initializer_list<int>& expected)
        : mMessage("Invalid number of arguments passed to a function")
    {
    }

    virtual const char * what() const
    {
        return mMessage.c_str();
    }

    virtual ~ArgumentMismatchException()
    {
    }

private:
    std::string mMessage;
};

typedef std::function<bool(_NAN_METHOD_ARGS_TYPE) > InitFunction;

class NanMethodArgBinding;
class NanCheckArguments;

template <typename EnumType>
class NanArgStringEnum;

//////////////////////////////////////////////////////////////////////////

class NanCheckArguments
{
public:
    NanCheckArguments(_NAN_METHOD_ARGS_TYPE args);
    NanCheckArguments(_NAN_METHOD_ARGS_TYPE args, InitFunction fn);

    NanCheckArguments& ArgumentsCount(int count);
    NanCheckArguments& ArgumentsCount(int argsCount1, int argsCount2);

    NanMethodArgBinding Argument(int index);

    /**
     * Unwind all fluent calls
     */
    operator bool() const;

    NanCheckArguments& AddAndClause(InitFunction rightCondition);

private:
    _NAN_METHOD_ARGS_TYPE m_args;
    InitFunction         m_init;
};

//////////////////////////////////////////////////////////////////////////

template <typename EnumType>
class NanArgStringEnum
{
public:
    explicit NanArgStringEnum(
        std::initializer_list< std::pair<const char*, EnumType> > possibleValues, 
        NanMethodArgBinding& owner,
        int argIndex);

    NanCheckArguments& Bind(EnumType& value);
protected:

    bool TryMatchStringEnum(const char * key, EnumType& outValue) const;

private:
    std::map<const char*, EnumType>     mPossibleValues;
    NanMethodArgBinding&                mOwner;
    int                                 mArgIndex;
};

//////////////////////////////////////////////////////////////////////////

/**
 * @brief This class wraps particular positional argument 
 */
class NanMethodArgBinding
{
public:

    template <typename EnumType>
    friend class NanArgStringEnum;

    NanMethodArgBinding(int index, NanCheckArguments& parent);

    NanMethodArgBinding& IsBuffer();
    NanMethodArgBinding& IsFunction();
    NanMethodArgBinding& IsString();
    NanMethodArgBinding& NotNull();

    template <typename T>
    NanArgStringEnum<T> StringEnum(std::initializer_list< std::pair<const char*, T> > possibleValues);

    template <typename T>
    NanCheckArguments& Bind(v8::Local<T>& value);

    template <typename T>
    NanCheckArguments& Bind(T& value);

private:
    int                 mArgIndex;
    NanCheckArguments&  mParent;
};

//////////////////////////////////////////////////////////////////////////

NanCheckArguments NanCheck(_NAN_METHOD_ARGS_TYPE args);

//////////////////////////////////////////////////////////////////////////
// Template functions implementation

template <typename T>
NanCheckArguments& NanMethodArgBinding::Bind(v8::Local<T>& value)
{
    return mParent.AddAndClause([this, &value](_NAN_METHOD_ARGS_TYPE args) {
        value = args[mArgIndex].As<T>();
        return true;
    });
}


template <typename T>
NanCheckArguments& NanMethodArgBinding::Bind(T& value)
{
    return mParent.AddAndClause([this, &value](_NAN_METHOD_ARGS_TYPE args) {
        MarshalToNative(args[mArgIndex], value);
        return true;
    });
}

template <typename T>
NanArgStringEnum<T> NanMethodArgBinding::StringEnum(std::initializer_list< std::pair<const char*, T> > possibleValues)
{
    return std::move(NanArgStringEnum<T>(possibleValues, IsString(), mArgIndex));
}

//////////////////////////////////////////////////////////////////////////

template <typename T>
NanArgStringEnum<T>::NanArgStringEnum(
    std::initializer_list< std::pair<const char*, T> > possibleValues, 
    NanMethodArgBinding& owner, int argIndex)
: mPossibleValues(possibleValues.begin(), possibleValues.end())
, mOwner(owner)
, mArgIndex(argIndex)
{
}

template <typename T>
NanCheckArguments& NanArgStringEnum<T>::Bind(T& value)
{
    return mOwner.mParent.AddAndClause([this, &value](_NAN_METHOD_ARGS_TYPE args) {
        std::string key;
        MarshalToNative(args[mArgIndex], key);
        return TryMatchStringEnum(key.c_str(), value);
    });
}

template <typename T>
bool NanArgStringEnum<T>::TryMatchStringEnum(const char * key, T& outValue) const
{
    auto it = mPossibleValues.find(key);
    if (it != mPossibleValues.end())
    {
        outValue = it->second;
        return true;
    }

    return false;
}