#include "cpps/cpps.h"

namespace cpps
{
	

	cpps_number cpps_to_number(const cpps_value& src)
	{
		const cpps_value& obj = src.real();
		cpps_number ret = 0;
		if (obj.tt == CPPS_TNUMBER)
		{
			ret = obj.value.number;
		}
		else if (obj.tt == CPPS_TINTEGER)
		{
			ret = cpps_integer2number(obj.value.integer);
		}
		else if (obj.tt == CPPS_TSTRING)
		{

			std::string* tmpStr = cpps_get_string(obj);

			cpps_str2d(tmpStr->c_str(), &ret);
		}
		return ret;
	}

	cpps_integer cpps_to_integer(const cpps_value& src)
	{
		const cpps_value& obj = src.real();
		cpps_integer ret = 0;
		if (obj.tt == CPPS_TNUMBER)
		{
			ret = cpps_number2integer(obj.value.number);
		}
		else if (obj.tt == CPPS_TINTEGER)
		{
			ret = obj.value.integer;
		}
		else if (obj.tt == CPPS_TSTRING)
		{
			std::string* tmpStr = cpps_get_string(obj);
			cpps_str2i64(tmpStr->c_str(), &ret);
		}
		return ret;
	}
	std::string* cpps_get_string(const cpps_value& src)
	{
		const cpps_value& obj = src.real();
		if (obj.tt == CPPS_TSTRING)
		{
			cpps_cppsclassvar* cppsclassvar = (cpps_cppsclassvar*)obj.value.domain;
			cpps::string* tmpStr = (cpps::string*)cppsclassvar->getclsptr();
			return &tmpStr->__str;
		}

		return NULL;
	}
	std::string cpps_to_string(const cpps_value& src)
	{
		const cpps_value& obj = src.real();
		std::stringstream strStream;
		if (obj.tt == CPPS_TNUMBER)
		{
			strStream << obj.value.number;
		}
		else if (obj.tt == CPPS_TINTEGER)
		{
			strStream << obj.value.integer;
		}
		else if (obj.tt == CPPS_TSTRING)
		{
			cpps_cppsclassvar* cppsclassvar = (cpps_cppsclassvar*)obj.value.domain;
			cpps::string* tmpStr = (cpps::string*)cppsclassvar->getclsptr();
			return tmpStr->__str;
		}
		else if (obj.tt == CPPS_TBOOLEAN)
		{
			strStream << (obj.value.b == 0 ? "false" : "true");
		}

		return strStream.str();
	}


	cpps_vector* cpps_to_cpps_vector(const cpps_value& src)
	{
		const cpps_value& obj = src.real();
		if (!obj.isdomain()) return NULL;
		cpps_cppsclassvar* cppsclassvar = (cpps_cppsclassvar*)obj.value.domain;
		cpps_vector* ret = (cpps_vector*)cppsclassvar->getclsptr();
		return ret;
	}
	cpps_map* cpps_to_cpps_map(const cpps_value& src)
	{
		const cpps_value& obj = src.real();
		if (!obj.isdomain()) return NULL;
		cpps_cppsclassvar* cppsclassvar = (cpps_cppsclassvar*)obj.value.domain;
		cpps_map* ret = (cpps_map*)cppsclassvar->getclsptr();
		return ret;
	}
	cpps_set* cpps_to_cpps_set(const cpps_value& src)
	{
		const cpps_value& obj = src.real();
		if (!obj.isdomain()) return NULL;
		cpps_cppsclassvar* cppsclassvar = (cpps_cppsclassvar*)obj.value.domain;
		cpps_set* ret = (cpps_set*)cppsclassvar->getclsptr();
		return ret;
	}

	cpps::cpps_pair* cpps_to_cpps_pair(const cpps_value& src)
	{
		const cpps_value& obj = src.real();
		if (!obj.isdomain()) return NULL;
		cpps_cppsclassvar* cppsclassvar = (cpps_cppsclassvar*)obj.value.domain;
		cpps_pair* ret = (cpps_pair*)cppsclassvar->getclsptr();
		return ret;
	}

	cpps_cppsclass* cpps_to_cpps_cppsclass(const cpps_value& src)
	{
		const cpps_value& obj = src.real();
		if (!obj.isdomain()) return NULL;
		cpps_cppsclass* cppsclassvar = (cpps_cppsclass*)obj.value.domain;
		return cppsclassvar;
	}

	cpps::cpps_cppsclassvar* cpps_to_cpps_cppsclassvar(const cpps_value& src)
	{
		const cpps_value& obj = src.real();
		if (!obj.isdomain()) return NULL;
		cpps_cppsclassvar* cppsclassvar = (cpps_cppsclassvar*)obj.value.domain;
		return cppsclassvar;
	}

	bool cpps_value::operator<(const cpps_value& right) const
	{
		if (tt == right.tt)
		{
			switch (tt)
			{
			case CPPS_TNUMBER: return value.number < right.value.number;
			case CPPS_TINTEGER: return value.integer < right.value.integer;
			case CPPS_TSTRING: {
				std::string* tmpStr = cpps_get_string(*this);
				std::string* tmpStr2 = cpps_get_string(right);
				return (*(tmpStr)) < (*(tmpStr2));
			}
			default:
				throw(cpps_error("unknow file", 0, 0, "<   ... It can't be used as a key.type: %d", right.tt));
			}
		}
		return tt < right.tt;
	}

	bool cpps_value::operator>(const cpps_value &right) const
	{
		if (tt == right.tt)
		{
			switch (tt)
			{
			case CPPS_TNUMBER: return value.number > right.value.number;
			case CPPS_TINTEGER: return value.integer > right.value.integer;
			case CPPS_TSTRING: {
				std::string* tmpStr = cpps_get_string(*this);
				std::string* tmpStr2 = cpps_get_string(right);
				return (*(tmpStr)) > (*(tmpStr2));
			}
			default:
				throw(cpps_error("unknow file", 0, 0, ">   ... It can't be used as a key. type: %d", right.tt));
			}
		}
		return tt > right.tt;
	}

	bool cpps_value::operator==(const cpps_value &right) const
	{
		if (tt == right.tt)
		{
			switch (tt)
			{
			case CPPS_TNUMBER: return value.number == right.value.number;
			case CPPS_TINTEGER: return value.integer == right.value.integer;
			case CPPS_TSTRING: {
				std::string* tmpStr = cpps_get_string(*this);
				std::string* tmpStr2 = cpps_get_string(right);
				return (*(tmpStr)) == (*(tmpStr2));
			}
			default:
				throw(cpps_error("unknow file", 0, 0, "==   ... It can't be used as a key. type: %d", right.tt));
			}
		}
		return false;
	}

	cpps_value::cpps_value(C* c, const char* s)
	{
		std::string* str = NULL;
		newclass<std::string>(c, &str, this);
		str->append(s);
	}

	cpps_value::cpps_value(C* c, const std::string& s)
	{
		cpps_value(c, s.c_str());
	}

	cpps_value::cpps_value(C* c, const std::string&& s)
	{
		cpps_value(c, s.c_str());
	}

	cpps_value::cpps_value(cpps_value* v)
	{
		tt = CPPS_TREF;
		value.value = v;
	}

	cpps_value::cpps_value(cpps_cppsclassvar* d)
	{
		tt = CPPS_TCLASSVAR;
		value.domain = (cpps_domain*)(d);
		incruse();
	}

	cpps_value::cpps_value(cpps_cppsclass* d)
	{
		tt = CPPS_TCLASS;
		value.domain = (cpps_domain*)(d);
	}

	cpps_value::cpps_value(cpps_domain* d)
	{
		tt = CPPS_TDOMAIN;
		value.domain = d;
	}

	cpps_value::cpps_value(cpps_function* f)
	{
		tt = CPPS_TFUNCTION;
		value.func = f;
	}

	cpps_value::cpps_value(const bool b)
	{
		tt = CPPS_TBOOLEAN;
		value.b = b;
	}

	cpps_value::cpps_value(const cpps_integer i)
	{
		tt = CPPS_TINTEGER;
		value.integer = i;
	}

	cpps_value::cpps_value(const cpps_number n)
	{
		tt = CPPS_TNUMBER;
		value.number = n;
	}

	cpps_value::cpps_value()
	{
		tt = CPPS_TNIL;
		value.integer = 0;
	}

	cpps_value::cpps_value(const cpps_value& v)
	{
		tt = v.tt;
		value = v.value;
		incruse();
	}
#ifdef _WIN32

	cpps_value::cpps_value(const unsigned __int64 i)
	{
		tt = CPPS_TINTEGER;
		value.integer = i;
	}
#else
	cpps_value::cpps_value(const long unsigned int i)
	{
		tt = CPPS_TINTEGER;
		value.integer = i;
	}

	

#endif
	cpps_value::cpps_value(char& strv)
	{
		tt = CPPS_TSTRINGV;
		value.strv = &strv;
	}
	cpps_value::~cpps_value()
	{
		decruse();
		tt = CPPS_TNIL;
		value.integer = 0;
	}

	bool cpps_value::operator<=(const cpps_value right) const
	{
		if (tt == right.tt)
		{
			switch (tt)
			{
			case CPPS_TNUMBER: return value.number <= right.value.number;
			case CPPS_TINTEGER: return value.integer <= right.value.integer;
			case CPPS_TSTRING: {
				cpps_cppsclassvar* cppsclassvar = (cpps_cppsclassvar*)value.domain;
				std::string* tmpStr = (std::string*)cppsclassvar->getclsptr();

				cpps_cppsclassvar* cppsclassvar2 = (cpps_cppsclassvar*)right.value.domain;
				std::string* tmpStr2 = (std::string*)cppsclassvar2->getclsptr();

				return (*(tmpStr)) <= (*(tmpStr2));
			}
			default:
				throw(cpps_error("unknow file", 0, 0, "<=   ... It can't be used as a key.type: %d", right.tt));
			}
		}
		return tt <= right.tt;
	}

	cpps::cpps_value& cpps_value::operator=(const cpps_value& v)
	{
		// 避免自赋值
		assert(this != &v);
		if (v.tt == CPPS_TCLASSVAR || v.tt == CPPS_TSTRING || v.tt == CPPS_TLAMBDAFUNCTION || v.tt == CPPS_TTUPLE)
			v.value.domain->incruse(); //先增后减
		decruse();
		tt = v.tt;
		value = v.value;

		return *this;
	}


	bool cpps_value::operator>=(const cpps_value right) const
	{
		if (tt == right.tt)
		{
			switch (tt)
			{
			case CPPS_TNUMBER: return value.number >= right.value.number;
			case CPPS_TINTEGER: return value.integer >= right.value.integer;
			case CPPS_TSTRING: {
				cpps_cppsclassvar* cppsclassvar = (cpps_cppsclassvar*)value.domain;
				std::string* tmpStr = (std::string*)cppsclassvar->getclsptr();

				cpps_cppsclassvar* cppsclassvar2 = (cpps_cppsclassvar*)right.value.domain;
				std::string* tmpStr2 = (std::string*)cppsclassvar2->getclsptr();

				return (*(tmpStr)) >= (*(tmpStr2));
			}
			default:
				throw(cpps_error("unknow file", 0, 0, ">=   ... It can't be used as a key.type: %d", right.tt));
			}
		}
		return tt >= right.tt;
	}

	void cpps_value::decruse()
	{
		if (tt == CPPS_TNIL) return ;
		if (tt == CPPS_TCLASSVAR || tt == CPPS_TSTRING || tt == CPPS_TLAMBDAFUNCTION || tt == CPPS_TTUPLE)
			value.domain->decruse();
	}

	 void cpps_value::incruse()
	{
		 if (tt == CPPS_TNIL) return ;
		 if (tt == CPPS_TCLASSVAR || tt == CPPS_TSTRING || tt == CPPS_TLAMBDAFUNCTION || tt == CPPS_TTUPLE)
			value.domain->incruse();
	}

	bool cpps_value::isdomain() const
	{
		if (tt == CPPS_TNIL) return false;
		return tt == CPPS_TDOMAIN || tt == CPPS_TCLASS || tt == CPPS_TCLASSVAR || tt == CPPS_TSTRING || tt == CPPS_TTUPLE;
	}

	bool cpps_value::isref() const
	{
		return tt == CPPS_TREF;
	}
	cpps_value	cpps_value::ref()
	{
		return cpps_value(this);
	}


	const cpps::cpps_value& cpps_value::real() const
	{
		return isref() ? *value.value : *this;
	}

	size_t cpps_value::hash::operator()(const cpps_value& _Keyval) const
	{

		switch (_Keyval.tt)
		{
		case CPPS_TNUMBER: return std::hash<cpps_number>()(_Keyval.value.number);
		case CPPS_TINTEGER: return std::hash<cpps_integer>()(_Keyval.value.integer);
		case CPPS_TSTRING: {
			std::string* tmpStr = cpps_get_string(_Keyval);
			return std::hash<std::string>()(*(tmpStr));
		}
		default:
			throw(cpps_error("unknow file", 0, 0, "==   ... It can't be used as a HashKey. type: %d", _Keyval.tt));
		}
		return _Keyval.tt;
	}

}
