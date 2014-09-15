#include "NanCheck.hpp"

typedef std::function<bool(_NAN_METHOD_ARGS_TYPE) > InitFunction;

class NanMethodArgBinding;
class NanCheckArguments;

NanMethodArgBinding::NanMethodArgBinding(int index, NanCheckArguments& parent)
	: mArgIndex(index)
	, mParent(parent)
{
}

NanMethodArgBinding& NanMethodArgBinding::IsBuffer()
{
	auto bind = [this](_NAN_METHOD_ARGS_TYPE args) 
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
	auto bind = [this](_NAN_METHOD_ARGS_TYPE args) 
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
    auto bind = [this](_NAN_METHOD_ARGS_TYPE args)
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
	auto bind = [this](_NAN_METHOD_ARGS_TYPE args) 
	{ 
		if (args[mArgIndex]->IsNull())
			throw ArgumentMismatchException(std::string("Argument ") + lexical_cast(mArgIndex) + " violates NotNull check");

		return true;
	};
	mParent.AddAndClause(bind);
	return *this;
}

NanCheckArguments::NanCheckArguments(_NAN_METHOD_ARGS_TYPE args)
: m_args(args)
, m_init([](_NAN_METHOD_ARGS_TYPE args) { return true; })
{
}

NanCheckArguments::NanCheckArguments(_NAN_METHOD_ARGS_TYPE args, InitFunction fn)
: m_args(args)
, m_init(fn)
{
}


NanCheckArguments& NanCheckArguments::ArgumentsCount(int count)
{
	return AddAndClause([count](_NAN_METHOD_ARGS_TYPE args) 
	{ 
		if (args.Length() != count)
			throw ArgumentMismatchException(args.Length(), count); 

		return true;
	});
}

NanCheckArguments& NanCheckArguments::ArgumentsCount(int argsCount1, int argsCount2)
{
	return AddAndClause([argsCount1, argsCount2](_NAN_METHOD_ARGS_TYPE args)
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
	InitFunction newInit = [prevInit, rightCondition](_NAN_METHOD_ARGS_TYPE args) {
		return prevInit(args) && rightCondition(args);
	};
	m_init = newInit;
	return *this;
}


NanCheckArguments NanCheck(_NAN_METHOD_ARGS_TYPE args)
{
	return std::move(NanCheckArguments(args));
}