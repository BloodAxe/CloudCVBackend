#include "NanCheck.hpp"

typedef std::function<bool(const v8::Arguments&) > InitFunction;

class NanMethodArgBinding;
class NanCheckArguments;

NanMethodArgBinding::NanMethodArgBinding(int index, NanCheckArguments& parent)
	: mArgIndex(index)
	, mParent(parent)
{
}

NanMethodArgBinding& NanMethodArgBinding::IsBuffer()
{
	auto bind = [this](const v8::Arguments& args) 
	{ 
		if (!node::Buffer::HasInstance(args[mArgIndex]))
			throw ArgumentMismatchException(std::string("Argument ") + lexical_cast(mArgIndex) + " violates IsBuffer check");
		return true;
	};

	mParent.AddAndClause(bind);
	return * this;
}

NanMethodArgBinding& NanMethodArgBinding::IsFunction()
{
	auto bind = [this](const v8::Arguments& args) 
	{ 
		if (!args[mArgIndex]->IsFunction())
			throw ArgumentMismatchException(std::string("Argument ") + lexical_cast(mArgIndex) + " violates IsFunction check");

		return true;
	};
	mParent.AddAndClause(bind);
	return *this;
}

NanMethodArgBinding& NanMethodArgBinding::IsString()
{
    auto bind = [this](const v8::Arguments& args)
    {
        if (!args[mArgIndex]->IsString() && !args[mArgIndex]->IsStringObject())
            throw ArgumentMismatchException(std::string("Argument ") + lexical_cast(mArgIndex) + " violates IsString check");

        return true;
    };
    mParent.AddAndClause(bind);
    return *this;
}

NanMethodArgBinding& NanMethodArgBinding::NotNull()
{
	auto bind = [this](const v8::Arguments& args) 
	{ 
		if (args[mArgIndex]->IsNull())
			throw ArgumentMismatchException(std::string("Argument ") + lexical_cast(mArgIndex) + " violates NotNull check");

		return true;
	};
	mParent.AddAndClause(bind);
	return *this;
}

NanCheckArguments::NanCheckArguments(const v8::Arguments& args)
: m_args(args)
, m_init([](const v8::Arguments& args) { return true; })
{
}

NanCheckArguments::NanCheckArguments(const v8::Arguments& args, InitFunction fn)
: m_args(args)
, m_init(fn)
{
}


NanCheckArguments& NanCheckArguments::ArgumentsCount(int count)
{
	return AddAndClause([count](const v8::Arguments& args) 
	{ 
		if (args.Length() != count)
			throw ArgumentMismatchException(args.Length(), count); 

		return true;
	});
}

NanCheckArguments& NanCheckArguments::ArgumentsCount(int argsCount1, int argsCount2)
{
	return AddAndClause([argsCount1, argsCount2](const v8::Arguments& args)
	{
		if (args.Length() != argsCount1 || args.Length() != argsCount2)
			throw ArgumentMismatchException(args.Length(), { argsCount1, argsCount2 });

		return true;
	});
}

NanMethodArgBinding NanCheckArguments::Argument(int index)
{
	return NanMethodArgBinding(index, *this);
}

/**
 * Unwind all fluent calls
 */
NanCheckArguments::operator bool() const
{
	return m_init(m_args);
}

NanCheckArguments& NanCheckArguments::AddAndClause(InitFunction rightCondition)
{
	InitFunction prevInit = m_init;
	InitFunction newInit = [prevInit, rightCondition](const v8::Arguments& args) {
		return prevInit(args) && rightCondition(args);
	};
	m_init = newInit;
	return *this;
}


NanCheckArguments NanCheck(const v8::Arguments& args)
{
	return std::move(NanCheckArguments(args));
}