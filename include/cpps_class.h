#ifndef CPPS_CLASS_CPPS_HEAD_
#define CPPS_CLASS_CPPS_HEAD_

//===================================
//@Author		:	Johnson
//@QQ			:	88481106
//@Email		:	jiang_4177@163.com
//@Date			:	2015/12/10 (yy/mm/dd)
//@Module		:	CPPS_CLASS
//@Description	:	Cpps、C++类注册
//@website		:	http://cpps.wiki
//==================================


namespace cpps
{
	struct cpps_cppsclassvar;
	template <class C>
	struct cpps_classvar;
	struct cpps_cppsclass : public cpps_domain
	{
		cpps_cppsclass(std::string _classname, Node *_o, cpps_domain* p, char type)
		:cpps_domain(p,type,_classname)
		{
			o = _o;
			classname = _classname;
		}
		virtual cpps_cppsclassvar *	create(C* c, bool alloc = true)
		{
			return (new cpps_cppsclassvar(getClassName(), this, cpps_domain_type_classvar, alloc));
		}
		virtual bool	iscppsclass() { return true; }
		std::string		getClassName()
		{
			return classname;
		}
		std::vector<cpps_cppsclass*>& parentClassList() { return _parentClassList; }
		Node		*o; //定义的变量
		std::string classname;
		std::vector<cpps_cppsclass*> _parentClassList;
	};

	template <class T>
	struct cpps_class : public cpps_cppsclass
	{
		cpps_class(std::string _classname,cpps_domain* p, char type)
			:cpps_cppsclass(_classname, NULL, p, type)
		{
		}
		virtual bool	iscppsclass() { return false; }
		virtual cpps_cppsclassvar* create(C* c, bool alloc = true)
		{
			cpps_classvar<T>* v = new cpps_classvar<T>(getClassName(), this, cpps_domain_type_classvar, alloc);
			if (alloc)
			{
				cpps_cppsclassvar* class_var = (cpps_cppsclassvar* )v;
				c->_class_map_classvar.insert(std::unordered_map<void*, cpps_cppsclassvar*>::value_type(v->_class, class_var));
			}
			return v;
		}
	};
	template <class T>
	struct cpps_class_singleton
	{
		cpps_class_singleton()
		{
			v = NULL;
		}
		void		setsls(cpps_cppsclass *_v)
		{
			v = _v;
		}
		cpps_cppsclass *getcls()
		{
			return v;
		}

		static cpps_class_singleton<T>* getSingletonPtr()
		{
			static cpps_class_singleton<T> t;
			return &t;
		}
		cpps_cppsclass	*v;
	};

	
}

#endif // CPPS_CLASS_CPPS_HEAD_