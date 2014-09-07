#include "NanCheck.hpp"

typedef std::function<bool(const v8::Arguments&) > InitFunction;

class NanMethodArgumentAdaptor;
class NanMethodArgumentHeler;

NanMethodArgumentAdaptor::NanMethodArgumentAdaptor(int index, NanMethodArgumentHeler& parent)
	: m_index(index)
	, m_parent(parent)
{
}

NanMethodArgumentAdaptor& NanMethodArgumentAdaptor::IsBuffer()
{
	auto bind = [this](const v8::Arguments& args) 
	{ 
		if (!node::Buffer::HasInstance(args[m_index]))
			throw ArgumentMismatchException(std::string("Argument ") + lexical_cast(m_index) + " violates IsBuffer check");
		return true;
	};

	m_parent.AddAndClause(bind);
	return * this;
}

NanMethodArgumentAdaptor& NanMethodArgumentAdaptor::IsFunction()
{
	auto bind = [this](const v8::Arguments& args) 
	{ 
		if (!args[m_index]->IsFunction())
			throw ArgumentMismatchException(std::string("Argument ") + lexical_cast(m_index) + " violates IsFunction check");

		return true;
	};
	m_parent.AddAndClause(bind);
	return *this;
}

NanMethodArgumentAdaptor& NanMethodArgumentAdaptor::IsString()
{
    auto bind = [this](const v8::Arguments& args)
    {
        if (!args[m_index]->IsString() && !args[m_index]->IsStringObject())
            throw ArgumentMismatchException(std::string("Argument ") + lexical_cast(m_index) + " violates IsString check");

        return true;
    };
    m_parent.AddAndClause(bind);
    return *this;
}

NanMethodArgumentAdaptor& NanMethodArgumentAdaptor::NotNull()
{
	auto bind = [this](const v8::Arguments& args) 
	{ 
		if (args[m_index]->IsNull())
			throw ArgumentMismatchException(std::string("Argument ") + lexical_cast(m_index) + " violates NotNull check");

		return true;
	};
	m_parent.AddAndClause(bind);
	return *this;
}

NanMethodArgumentHeler::NanMethodArgumentHeler(const v8::Arguments& args)
: m_args(args)
, m_init([](const v8::Arguments& args) { return true; })
{
}

NanMethodArgumentHeler::NanMethodArgumentHeler(const v8::Arguments& args, InitFunction fn)
: m_args(args)
, m_init(fn)
{
}


NanMethodArgumentHeler& NanMethodArgumentHeler::ArgumentsCount(int count)
{
	return AddAndClause([count](const v8::Arguments& args) 
	{ 
		if (args.Length() != count)
			throw ArgumentMismatchException(args.Length(), count); 

		return true;
	});
}

NanMethodArgumentHeler& NanMethodArgumentHeler::ArgumentsCount(int argsCount1, int argsCount2)
{
	return AddAndClause([argsCount1, argsCount2](const v8::Arguments& args)
	{
		if (args.Length() != argsCount1 || args.Length() != argsCount2)
			throw ArgumentMismatchException(args.Length(), { argsCount1, argsCount2 });

		return true;
	});
}

NanMethodArgumentAdaptor NanMethodArgumentHeler::Argument(int index)
{
	return NanMethodArgumentAdaptor(index, *this);
}

/**
 * Unwind all fluent calls
 */
NanMethodArgumentHeler::operator bool() const
{
	return m_init(m_args);
}

NanMethodArgumentHeler& NanMethodArgumentHeler::AddAndClause(InitFunction rightCondition)
{
	InitFunction prevInit = m_init;
	InitFunction newInit = [prevInit, rightCondition](const v8::Arguments& args) {
		return prevInit(args) && rightCondition(args);
	};
	m_init = newInit;
	return *this;
}


NanMethodArgumentHeler NanCheck(const v8::Arguments& args)
{
	return std::move(NanMethodArgumentHeler(args));
}