#ifndef CPPS_OBJECT_CPPS_HEAD_
#define CPPS_OBJECT_CPPS_HEAD_

//===================================
//@Author		:	Johnson
//@QQ			:	88481106
//@Email		:	88481106@qq.com
//@Date			:	2015/11/24 (yy/mm/dd)
//@Module		:	CPPS_OBJECT
//@Description	:	CppsתC++
//@website		:	http://cppscript.org
//==================================

#include "cpps_array.h"
#include "cpps_map.h"

namespace cpps
{
	struct cpps_domain;
	struct cpps_vector;
	struct cpps_map;
	struct cpps_map;
	
	template<class T>
	inline void		newclass(C* c, T** ret,cpps_value *ret_value);
	struct object
	{

		object();
		object(const object& k);
		object(cpps_value v);

		static object real(object o);
		struct vector
		{
		public:
			static		vector							create(C* c);
		public:
			vector(object obj);
			cpps_std_vector::iterator					begin();
			cpps_std_vector::iterator					end();
			void										push_back(object v);
			void										erase(cpps_integer idx);
			object										toobject();
			object										operator[](const cpps_integer k);
			cpps_std_vector&							realvector();
		private:
			cpps_vector* _vec;
			cpps_value _src_value;
		};
		
		struct map
		{
		public:
			static map																create(C* c);
		public:

			map(C* cstate, object obj);
			cpps_hash_map::iterator													begin();
			cpps_hash_map::iterator													end();
			void																	insert(object key, object value);
			object																	toobject();
			template<class T>
			bool																	has(const T k) {
				cpps_value key = cpps_cpp_to_cpps_converter<T>::apply(c, k);
				return _map->has(key);
			}
			template<class T>
			void																	erase(const T k) {
				cpps_value key = cpps_cpp_to_cpps_converter<T>::apply(c, k);
				_map->erase(key);
			}
			template<class T>
			object																	operator[](const T k) {
				cpps_value key = cpps_cpp_to_cpps_converter<T>::apply(c, k);
				cpps_value& value = _map->cpps_find(key);
				return cpps_value(&value);
			}
			cpps_hash_map&															realmap();
		private:
			cpps_value _src_value;
			cpps_map* _map;
			C* c;
		};
		struct set
		{
		public:
			static set																create(C* c);
		public:

			set(C* cstate, object obj);
			cpps_hash_set::iterator													begin();
			cpps_hash_set::iterator													end();
			void																	insert(object key);
			object																	toobject();
			template<class T>
			bool																	has(const T k) {
				cpps_value key = cpps_cpp_to_cpps_converter<T>::apply(c, k);
				return _set->has(key);
			}
			cpps_hash_set&															realset();
		private:
			cpps_value _src_value;
			cpps_set* _set;
			C* c;
		};
		struct pair
		{
		public:
			static pair																create(C* c,object first,object second);
		public:
			pair(C* cstate, object obj);

			object																	first();
			object																	second();
			object																	toobject();

			cpps_pair*																_pair;
			cpps_value																_src_value;
			C* c;
		};
		


		// define global var.
		static void define(C* c, std::string varname, object v = object());
		//create object.
		//class C is required because the string needs GC.
		//
		static object	create_with_pair(C* c);
		static object	create_with_map(C* c);
		static object	create_with_set(C* c);
		static object	create_with_vector(C* c);
		static object	create_with_cppsclassvar(C* c,object __classobject);
		template<class T>
		static object	create_with_classvar(C* c, T** ptr) {
			object ret;
			newclass<T>(c, ptr,&ret.value);
			return ret;
		}
		template<class Type>
		static object	create(C*c, Type v) {
			return object(c, v);
		}
		object&		operator=(const cpps_value k);
		object&		operator=(const object& k);
		//_G root node.
		static object globals(C* c);


		//check
		bool					isunorderd_map();
		bool					ispair();
		bool					ismap();
		bool					isset();
		bool					isstring();
		bool					isvector();
		bool					isrange();
		bool					istuple();
		bool					isellipsis();
		bool					isint();
		bool					isnumber();
		bool					isnull();
		bool					isfunction();
		bool					isclass();
		bool					isclassvar();
		bool					isref();

		//convert
		std::string				tostring();
		cpps_integer			toint();
		cpps_number				tonumber();
		bool					tobool();
		object					toreal();
		cpps_value				ref();


		//vector ,map ,string .
		cpps_integer	size();
		//vector map  only.
		void			clear();
		bool			empty();
		object			operator[](const std::string k); // and domain
		
		//map  only.
		void		insert(object key,object val);
	
		//vector only.
		void		push_back(object& val);
		object		operator[](const cpps_integer k);

		cpps_value& getval();
		

		template<class Type>
		object(C* c, Type v)
		{
			if (!cpps_cpp_to_cpps_converter<Type>::match(c, v))
			{
				throw(cpps_error(__FILE__, __LINE__, 0, "%s is not defined to script, conversion failed.", typeid(Type).name()));
			}

			value = cpps_cpp_to_cpps_converter<Type>::apply(c, v);
		}
		cpps_value	value;
	};

	int32		type(object o);
	std::string type_s(object o);
}

#endif // CPPS_OBJECT_CPPS_HEAD_