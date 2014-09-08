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

	ArgumentMismatchException(const std::string msg)
		: m_what(msg)
	{
	}

	ArgumentMismatchException(int actual, int expected)
		: m_what("Invalid number of arguments passed to a function")
	{
	}

	ArgumentMismatchException(int actual, const std::initializer_list<int>& expected)
		: m_what("Invalid number of arguments passed to a function")
	{
	}

	virtual const char * what() const
	{
		return m_what.c_str();
	}

	virtual ~ArgumentMismatchException()
	{
	}

private:
	std::string m_what;
};

typedef std::function<bool(const v8::Arguments&) > InitFunction;

class NanMethodArgumentAdaptor;
class NanMethodArgumentHeler;

template <typename EnumType>
class NanArgStringEnum;

//////////////////////////////////////////////////////////////////////////

class NanMethodArgumentHeler
{
public:
	NanMethodArgumentHeler(const v8::Arguments& args);
	NanMethodArgumentHeler(const v8::Arguments& args, InitFunction fn);

	NanMethodArgumentHeler& ArgumentsCount(int count);
	NanMethodArgumentHeler& ArgumentsCount(int argsCount1, int argsCount2);

	NanMethodArgumentAdaptor Argument(int index);

	/**
	 * Unwind all fluent calls
	 */
	operator bool() const;

	NanMethodArgumentHeler& AddAndClause(InitFunction rightCondition);

private:
	const v8::Arguments& m_args;
	InitFunction         m_init;
};

//////////////////////////////////////////////////////////////////////////

template <typename EnumType>
class NanArgStringEnum
{
public:
    explicit NanArgStringEnum(
        std::initializer_list< std::pair<const char*, EnumType> > possibleValues, 
        NanMethodArgumentAdaptor& owner,
        int argIndex);

    NanMethodArgumentHeler& Bind(EnumType& value);
protected:

    bool tryMatchStringEnum(const char * key, EnumType& outValue) const;

private:
    std::map<const char*, EnumType>     m_possibleValues;
    NanMethodArgumentAdaptor&           m_owner;
    int                                 m_argIndex;
};

//////////////////////////////////////////////////////////////////////////

class NanMethodArgumentAdaptor
{
public:

    template <typename EnumType>
    friend class NanArgStringEnum;

	NanMethodArgumentAdaptor(int index, NanMethodArgumentHeler& parent);

	NanMethodArgumentAdaptor& IsBuffer();
	NanMethodArgumentAdaptor& IsFunction();
    NanMethodArgumentAdaptor& IsString();
    NanMethodArgumentAdaptor& NotNull();

    template <typename T>
    NanArgStringEnum<T> StringEnum(std::initializer_list< std::pair<const char*, T> > possibleValues);

	template <typename T>
	NanMethodArgumentHeler& Bind(v8::Local<T>& value);

    template <typename T>
	NanMethodArgumentHeler& Bind(T& value);

private:
	int						 m_index;
	NanMethodArgumentHeler&  m_parent;
};

//////////////////////////////////////////////////////////////////////////

NanMethodArgumentHeler NanCheck(const v8::Arguments& args);

//////////////////////////////////////////////////////////////////////////
// Template functions implementation

template <typename T>
NanMethodArgumentHeler& NanMethodArgumentAdaptor::Bind(v8::Local<T>& value)
{
	return m_parent.AddAndClause([this, &value](const v8::Arguments& args) {
		value = args[m_index].As<T>();
		return true;
	});
}


template <typename T>
NanMethodArgumentHeler& NanMethodArgumentAdaptor::Bind(T& value)
{
	return m_parent.AddAndClause([this, &value](const v8::Arguments& args) {
		MarshalToNative(args[m_index], value);
		return true;
	});
}

template <typename T>
NanArgStringEnum<T> NanMethodArgumentAdaptor::StringEnum(std::initializer_list< std::pair<const char*, T> > possibleValues)
{
    return std::move(NanArgStringEnum<T>(possibleValues, IsString(), m_index));
}

//////////////////////////////////////////////////////////////////////////

template <typename T>
NanArgStringEnum<T>::NanArgStringEnum(
    std::initializer_list< std::pair<const char*, T> > possibleValues, 
    NanMethodArgumentAdaptor& owner, int argIndex)
: m_possibleValues(possibleValues.begin(), possibleValues.end())
, m_owner(owner)
, m_argIndex(argIndex)
{
}

template <typename T>
NanMethodArgumentHeler& NanArgStringEnum<T>::Bind(T& value)
{
    return m_owner.m_parent.AddAndClause([this, &value](const v8::Arguments& args) {
        std::string key;
        MarshalToNative(args[m_argIndex], key);
        return tryMatchStringEnum(key.c_str(), value);
    });
}

template <typename T>
    bool NanArgStringEnum<T>::tryMatchStringEnum(const char * key, T& outValue) const
    {
        auto it = m_possibleValues.find(key);
        if (it != m_possibleValues.end())
        {
            outValue = it->second;
            return true;
        }

        return false;
    }