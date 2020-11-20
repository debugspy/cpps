#include "cpps.h"


namespace cpps
{

	//�ڲ�
	struct					cpps_value;
	struct					Node;
	struct					cpps_domain;
	struct					cpps_value;
	struct					cpps_func_params;
	void					cpps_gc_add_gen0(C* c, cpps_cppsclassvar* p);
	cpps_value				cpps_calculate_expression(C* c, cpps_domain* domain, Node* o, cpps_domain*& leftdomain);
	void					cpps_step(C* c, cpps_domain* domain, Node* d);
	cpps_value				cpps_step_callfunction(C* c, cpps_domain* domain, cpps_value func, Node* d, cpps_domain* leftdomain);
	void					cpps_step_all(C* c, int32 retType, cpps_domain* domain, Node* o);
	void					cpps_parse_dofunction(cpps_domain* domain, Node* o, Node* root, cppsbuffer& buffer);
	void					cpps_parse_expression(cpps_domain* domain, Node* o, Node* root, cppsbuffer& buffer);
	int32					cpps_parse_expression_step(cpps_domain* domain, Node* param, Node*& lastOpNode, Node* root, cppsbuffer& buffer);
	Node*					cpps_parse_param(cpps_domain* domain, Node* o, Node* root, cppsbuffer& buffer);
	Node*					cpps_parse_symbol(cpps_domain* domain, Node* o, cppsbuffer& buffer, bool leftsymbol = false);
	Node*					cpps_parse_string(cpps_domain* domain, Node* o, cppsbuffer& buffer, int8 endch);
	void					cpps_parse_var(cpps_domain* domain, Node* child, Node* root, cppsbuffer& buffer, int32 limit, int8 isconst);
	Node*					cpps_parse_var_param(cpps_domain* domain, Node* o, Node* root, cppsbuffer& buffer);
	Node*					cpps_parse_number(cpps_domain* domain, Node* o, cppsbuffer& buffer);
	void					cpps_parse_def_function(cpps_domain* domain, Node* right, Node* root, cppsbuffer& buffer);
	Node*					cpps_parse_line(cpps_domain* domain, Node* o, Node* root, cppsbuffer& buffer, int32 limit = 0);
	void					cpps_parse_builtin(cpps_domain* domain, Node* child, Node* root, cppsbuffer& buffer, int32 limit);
	void					cpps_parse_def(cpps_domain* domain, Node* child, Node* root, cppsbuffer& buffer, int32 limit);
	Node*					cpps_parse_new_setv(cpps_domain* domain, Node* o, Node* root, cppsbuffer& buffer);
	void					cpps_gc_add_barrier(C* c, cpps_regvar* v);
	void					cpps_gc_check_gen0(C* c);
	void					cpps_gc_check_gen1(C* c);
	bool					cpps_loadlibrary(C* c, std::string libname);


	void cpps_gc_check_step(C* c)
	{
		if ((c->getGen0size() + c->getGen1size() > (CPPS_GEN1_CHECKSIZE < c->getLastgensize() ? c->getLastgensize() * 1.2 : CPPS_GEN1_CHECKSIZE)))
		{
			cpps_gc_check_gen1(c);
		}
		else if ((c->getGen0size() > CPPS_GEN0_CHECKSIZE || c->getGen0()->size() > CPPS_GEN0_CHECKCOUNT))
		{
			cpps_gc_check_gen0(c);
		}

	}
	bool cpps_parse_isnumber(char ch)
	{
		return ch >= 48 && ch <= 57;
	}
	bool cpps_parse_isspace(char ch)
	{
		return ch == ' ' || ch == '	' || ch == '\t';
	}

	bool cpps_parse_isenter(char ch)
	{
		return ch == '\n' || ch == '\r';
	}
	void cpps_parse_rmspaceandenter(cppsbuffer& buffer)
	{
		while (cpps_parse_isenter(buffer.cur()) || cpps_parse_isspace(buffer.cur()))
			buffer.pop();
	}

	bool cpps_parse_issymbol(char ch)
	{
		return ch == '+' || ch == '-' || ch == '*'
			|| ch == '/' || ch == '|' || ch == '&'
			|| ch == '%' || ch == '!' || ch == '['
			|| ch == ']' || ch == '.' || ch == '='
			|| ch == '<' || ch == '>' || ch == '?';
	}

	bool cpps_parse_isnotvarname(char ch)
	{
		return ch == '~' || ch == '!' || ch == '@' || ch == '#' || ch == '$'
			|| ch == '%' || ch == '^' || ch == '&' || ch == '*' || ch == '('
			|| ch == ')' || ch == '+' || ch == '`' || ch == '{' || ch == '}'
			|| ch == ';' || ch == '"' || ch == ':' || ch == '\'' || ch == '\\'
			|| ch == '|' || ch == ',' || ch == '<' || ch == '.' || ch == '>'
			|| ch == '/' || ch == '?' || ch == '\0' || cpps_parse_isspace(ch)
			|| cpps_parse_issymbol(ch) || cpps_parse_isenter(ch) || ch == '\n';
	}
	bool cpps_parse_isint16(char ch)
	{
		ch = tolower(ch);
		return ch == 'a' || ch == 'b' || ch == 'c' || ch == 'd' || ch == 'e' || ch == 'f' || ch == '1' || ch == '2' || ch == '3' || ch == '4' || ch == '5' || ch == '6' || ch == '7' || ch == '8' || ch == '9' || ch == '0';
	}
	std::string cpps_parse_varname(cppsbuffer& buffer)
	{
		std::string ret = "";
		while (!cpps_parse_isnotvarname(buffer.cur()))
		{
			ret.push_back(buffer.pop());
		}
		return ret;
	}
	bool cpps_parse_isbuiltinname(std::string s)
	{
		return s == "if" || s == "const" || s == "try" || s == "throw" || s == "namespace" || s == "var" || s == "else" || s == "for" || s == "foreach" || s == "do" || s == "while" || s == "class" || s == "struct" || s == "break" || s == "continue" || s == "case" || s == "switch" || s == "enum" || s == "return" || s == "dofile" || s == "import" || s == "include" || s == "dostring";
	}
	bool cpps_is_not_use_var_name(std::string s)
	{
		return cpps_parse_isbuiltinname(s) || s == "true" || s == "catch" || s == "null" || s == "nil" || s == "NULL" || s == "false" || s == "map" || s == "vector" || s == "math" || s == "string" || s == "time" || s == "io" || s == "GC" || s == "unordered_map";
	}
	std::string cpps_parse_loadinterger16(cppsbuffer& buffer)
	{
		std::string ret = "";
		while (cpps_parse_isint16(buffer.cur()))
		{
			ret.push_back(buffer.pop());
		}
		return ret;

	}
	void cpps_parse_loadnumber(cppsbuffer& buffer, Node* str)
	{
		if (!cpps_parse_isnumber(buffer.realcur()))
		{
			throw(cpps_error(str->filename, buffer.line(), cpps_error_paramerror, "Not the expected number after the decimal point."));
		}
		while (cpps_parse_isnumber(buffer.realcur()))
		{
			char ch = buffer.pop();
			str->s.push_back(ch);
		}
	}
	char cpps_parse_transfer_character(char ch, cppsbuffer& buffer)
	{
		if (ch == '\\')
		{
			char ch2 = buffer.pop();
			switch (ch2)
			{
			case 'a': ch = 7; break;
			case 'b': ch = 8; break;
			case 'f': ch = 12; break;
			case 'n': ch = 10; break;
			case 'r': ch = 13; break;
			case 't': ch = 9; break;
			case 'v': ch = 11; break;
			case '\\': ch = 92; break;
			case '?': ch = 63; break;
			case '\'': ch = 39; break;
			case '"': ch = 34; break;
			case '0': ch = 0; break;
			case '{': ch = '{'; break;
			case '}': ch = '}'; break;
			default: ch = ch2; break;
			}
		}
		return ch;
	}
	Node* cpps_parse_string(cpps_domain* domain, Node* o, Node* root, cppsbuffer& buffer, int8 endch)
	{
		Node* str = new Node(o->filename, buffer.line());
		str->type = CPPS_OSTR;
		buffer.pop(); // pop '"'

		Node* laststr = NULL;

		while (buffer.realcur() != endch && !buffer.isend())
		{
			char ch = buffer.realpop();
			//todo ������б��
			ch = cpps_parse_transfer_character(ch, buffer);

			if (ch == '{')
			{

				Node* v = new Node(str, o->filename, buffer.line());
				v->type = CPPS_VARNAME;
				cpps_parse_expression(domain, v, root, buffer);

				if (buffer.cur() != '}')
					throw(cpps_error(str->filename, buffer.line(), cpps_error_varerror, "Unexpected end of '}' not detected when string defining variable value."));

				buffer.pop();//pop }
				laststr = NULL;
			}
			else
			{
				if (laststr == NULL) {

					laststr = new Node(o->filename, buffer.line());
					laststr->type = CPPS_OSTR;
					str->add(laststr);
				}
				laststr->s.push_back(ch);
			}
		}
		buffer.pop();

		return str;
	}


	void cpps_parse_def_function_param(cpps_domain* domain, Node* right, Node* root, cppsbuffer& buffer)
	{
		Node* typen = new Node(right, right->filename, buffer.line());

		typen->type = CPPS_VARNAME;

		typen->s = cpps_parse_varname(buffer);

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);

		Node* varname = new Node(typen, right->filename, buffer.line());
		varname->s = cpps_parse_varname(buffer);

		varname->type = CPPS_VARNAME;

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);

		if (buffer.cur() == '=')//��Ĭ��ֵ Ҳ���ǿ��Բ������
		{
			buffer.pop();

			//�޳��ո�
			cpps_parse_rmspaceandenter(buffer);

			cpps_parse_expression(domain, varname, root, buffer);
		}
	}


	void cpps_parse_def_function(cpps_domain* domain, Node* right, Node* root, cppsbuffer& buffer)
	{
		//������һ������
		cpps_domain* funcdomain = new cpps_domain(domain, cpps_domain_type_func, right->parent->s);
		right->setdomain(funcdomain);

		Node* params = new Node(right, right->filename, buffer.line());
		Node* context = new Node(right, right->filename, buffer.line());

		char ch = 0;
		//��ȡ����

		while (!buffer.isend())
		{
			//�޳��ո�
			cpps_parse_rmspaceandenter(buffer);

			ch = buffer.cur();
			if (ch == ')')
			{
				buffer.pop();
				break;
			}

			if (ch == ',')
			{
				buffer.pop();
				cpps_parse_rmspaceandenter(buffer);
			}

			cpps_parse_def_function_param(funcdomain, params, root, buffer);

		}

		//����{ }
		cpps_parse_rmspaceandenter(buffer);

		ch = buffer.pop();
		if (ch == '{')
		{
			while (!buffer.isend())
			{
				//�޳��س�.
				cpps_parse_rmspaceandenter(buffer);
				//�Ƿ�����ˡ�
				if (buffer.cur() == '}')
				{
					buffer.pop();
					return;
				}
				cpps_parse_line(funcdomain, context, root, buffer);
			}
		}
		else
		{
			throw(cpps_error(right->filename, buffer.line(), cpps_error_deffuncrror, "Definition function did not detect '{'"));
		}
		throw(cpps_error(right->filename, buffer.line(), cpps_error_deffuncrror, "Definition function did not detect '}'"));
	}


	void cpps_parse_var_right(cpps_domain* domain, Node* var, Node* root, cppsbuffer& buffer, int32 limit)
	{
		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);

		Node* right = new Node(var, var->filename, buffer.line());

		if (buffer.cur() == '=')
		{
			char ch = buffer.pop();
			if (limit & CPPS_NOT_DEFVAR)
			{
				throw("Prohibit defining variables");
			}
			//�Ǹ�����
			right->type = CPPS_ODEFVAR_VAR;
			cpps_parse_rmspaceandenter(buffer);
			cpps_parse_expression(domain, right, root, buffer);
		}
		else if (buffer.cur() == '(') //������һ������
		{
			char ch = buffer.pop();
			if (limit & CPPS_NOT_DEFFUNCTION)
			{
				throw("Function definition prohibited");
			}

			right->type = CPPS_ODEFVAR_FUNC;
			cpps_parse_def_function(domain, right, root, buffer);//������һ������
		}
		else if (buffer.cur() == ';')
		{
			right->type = CPPS_ODEFVAR_NIL;
		}
		else
		{
			right->type = CPPS_ODEFVAR_NIL;

			throw(cpps_error(var->filename, buffer.line(), cpps_error_varerror, "Error in defining variable. There should be '=' or '(' after the variable!"));
		}
	}

	void cpps_parse_var(cpps_domain* domain, Node* child, Node* root, cppsbuffer& buffer, int32 limit, int8 isconst)
	{
		//�޳��ո�
		child->type = isconst ? CPPS_ODEFCONSTVAR : CPPS_ODEFVAR;

		while (true)
		{
			cpps_parse_rmspaceandenter(buffer);

			Node* str = new Node(child, child->filename, buffer.line());
			str->type = CPPS_VARNAME;

			if (cpps_parse_isnumber(buffer.cur())) //��λ�Ǹ���ĸ
				throw(cpps_error(child->filename, buffer.line(), cpps_error_varnotnumber, "Variable cannot start with a number."));

			//��������
			str->s = cpps_parse_varname(buffer);
			if (cpps_is_not_use_var_name(str->s))
			{
				throw(cpps_error(child->filename, buffer.line(), cpps_error_varnotnumber, "Variable names cannot use reserved keywords."));
			}
			cpps_parse_var_right(domain, str, root, buffer, limit);

			cpps_parse_rmspaceandenter(buffer);

			if (buffer.cur() != ',')
			{
				return;
			}
			buffer.pop();
		}


	}
	void cpps_parse_namespace(cpps_domain* domain, Node* child, Node* root, cppsbuffer& buffer, int32 limit)
	{
		child->type = CPPS_ONAMESPACE;

		cpps_parse_rmspaceandenter(buffer);

		child->s = cpps_parse_varname(buffer);

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);

		if (buffer.cur() != '{')
			throw(cpps_error(child->filename, buffer.line(), cpps_error_varnotnumber, "'{' was detected for namespace. Please check."));
		buffer.pop();

		while (!buffer.isend())
		{
			//�޳��س�.
			cpps_parse_rmspaceandenter(buffer);

			//�Ƿ�����ˡ�
			if (buffer.cur() == '}')
			{
				break;
			}
			if (buffer.isend())
			{
				throw(cpps_error(child->filename, buffer.line(), cpps_error_classerror, "'}' was detected for namespace. Please check."));
			}
			cpps_parse_line(domain, child, root, buffer, CPPS_NOT_DEFASSEMBLE);
		}

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() != '}')
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_classerror, "'}' was detected for namespace. Please check."));
		}
		buffer.pop(); //pop }
	}
	void cpps_parse_trycatch(cpps_domain* domain, Node* child, Node* root, cppsbuffer& buffer, int32 limit)
	{
		cpps_domain* trycatchdomain = new cpps_domain(domain, cpps_domain_type_trycatch, "trycatch");//Ϊ���������һ���Լ�����ռ�
		child->setdomain(trycatchdomain);

		child->type = CPPS_OTRYCATCH;

		Node* func = new Node(child, child->filename, buffer.line());// 0 if�Ĳ���
		Node* catchfun = new Node(child, child->filename, buffer.line()); // true 

		cpps_parse_rmspaceandenter(buffer);

		cpps_parse_line(trycatchdomain, func, root, buffer);

		cpps_parse_rmspaceandenter(buffer);

		std::string el = cpps_parse_varname(buffer);
		if (el == "catch")
		{

			Node* catchvar = new Node(catchfun, child->filename, buffer.line()); // true 
			Node* catchstep = new Node(catchfun, child->filename, buffer.line()); // true 

			cpps_parse_rmspaceandenter(buffer);

			if (buffer.cur() != '(')
				throw(cpps_error(catchfun->filename, buffer.line(), cpps_error_trycatherror, "Catch did not detect '('"));
			buffer.pop();

			cpps_parse_rmspaceandenter(buffer);
			std::string ifvar = cpps_parse_varname(buffer);
			if (ifvar != "var")
				throw(cpps_error(catchfun->filename, buffer.line(), cpps_error_trycatherror, "'var' needs to be defined to receive variables."));

			cpps_parse_rmspaceandenter(buffer);
			catchvar->s = cpps_parse_varname(buffer);
			catchvar->type = CPPS_VARNAME;

			cpps_parse_rmspaceandenter(buffer);
			if (buffer.cur() != ')')
			{
				throw(cpps_error(catchfun->filename, buffer.line(), cpps_error_trycatherror, "Catch did not detect ')'"));
			}
			buffer.pop(); //pop }

			cpps_parse_rmspaceandenter(buffer);

			cpps_parse_line(domain, catchstep, root, buffer);

			cpps_parse_rmspaceandenter(buffer);


		}
		else
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_trycatherror, "Try needs corresponding catch."));
		}

	}

	Node* cpps_parse_var_param(cpps_domain* domain, Node* o, Node* root, cppsbuffer& buffer)
	{

		Node* param = new Node(o->filename, buffer.line());
		param->type = CPPS_VARNAME;


		if (cpps_parse_isnumber(buffer.cur())) //��λ�Ǹ���ĸ
			throw(cpps_error(o->filename, buffer.line(), cpps_error_varnotnumber, "Variable cannot start with a number."));

		param->s = cpps_parse_varname(buffer);

		if (param->s == "new") //new ѽ~
		{
			//�޳��ո�
			cpps_parse_rmspaceandenter(buffer);
			param->type = CPPS_ONEWVAR;
			param->s = cpps_parse_varname(buffer);

			if (param->s.empty())
			{
				throw(cpps_error(param->filename, buffer.line(), cpps_error_varnotnumber, "The object of new must have a type."));
			}
			Node* lastNode = param;
			//�Ƿ�ʹ�����ռ�
			while (!buffer.isend())
			{
				//�޳��س�.
				cpps_parse_rmspaceandenter(buffer);
				if (buffer.cur() == ':' && buffer.at(buffer.offset() + 1) == ':')
				{
					buffer.pop();
					buffer.pop();

					cpps_parse_rmspaceandenter(buffer);

					Node* child = new Node(lastNode, o->filename, buffer.line());
					child->s = cpps_parse_varname(buffer);
					child->type = CPPS_ONAMESPANCE_CHILD;
					lastNode = child;
				}
				else
				{
					break;
				}
			}
			cpps_parse_rmspaceandenter(buffer);
			if (param->s == "vector" && buffer.cur() == '[')
			{
				buffer.pop();
				Node* count = new Node(param, param->filename, buffer.line());

				cpps_parse_loadnumber(buffer, count);

				if (buffer.cur() != ']')
				{
					throw(cpps_error(param->filename, buffer.line(), cpps_error_varnotnumber, "']' was not detected when defining an array"));
				}
				buffer.pop();
			}
			else if (buffer.cur() == '(')
			{
				buffer.pop();




				Node* child = new Node(param->filename, buffer.line());
				child->type = CPPS_OCLASS_CONSTRUCTOR;
				cpps_domain* funcdomain = new cpps_domain(domain, cpps_domain_type_func, o->s);//Ϊ���������һ���Լ�����ռ�
				child->setdomain(funcdomain);

				cpps_parse_dofunction(funcdomain, child, root, buffer);
				param->addtoright(child);



				// var a = new A(){}
				cpps_parse_rmspaceandenter(buffer);
				if (buffer.cur() == '{') {
					Node* setv = cpps_parse_new_setv(domain, param, root, buffer);
					param->add(setv);
				}
			}
		}
		else if (param->s == "true" || param->s == "false")
		{
			//��˵��ʲô�� ˵�����Ǹ�bool..
			if (o->type == CPPS_ONIL)
				o->type = CPPS_OBOOLPARAM;

			param->type = CPPS_OBOOL;


		}
		else if (param->s == "nil" || param->s == "null" || param->s == "NULL")
		{
			param->type = CPPS_ONIL;
		}
		else
		{
			if (o->type == CPPS_ONIL)
				o->type = CPPS_OVARPARAM;

			//�޳��ո�
			cpps_parse_rmspaceandenter(buffer);
		}
		return param;
	}

	Node* cpps_parse_number(cpps_domain* domain, Node* o, cppsbuffer& buffer)
	{
		Node* str = new Node(o->filename, buffer.line());
		str->type = CPPS_OINTEGER;

		cpps_parse_loadnumber(buffer, str);

		int32 off = buffer.offset();

		if (buffer.cur() == '.')
		{
			char ch = buffer.pop();
			if (!cpps_parse_isnumber(buffer.cur()))
			{
				//����治������˵������С����
				buffer.seek(off);
			}
			else
			{
				str->type = CPPS_ONUMBER;

				//numberѽ.
				str->s.push_back(ch);

				cpps_parse_loadnumber(buffer, str);
			}
		}
		else if (buffer.cur() == 'x')
		{
			str->type = CPPS_OINTEGER16;
			buffer.pop();
			str->s += "x";
			str->s += cpps_parse_loadinterger16(buffer);

		}
		return str;
	}
	Node* cpps_parse_bracket(cpps_domain* domain, Node* o, Node* root, cppsbuffer& buffer)
	{
		Node* bracket = new Node(o->filename, buffer.line());
		bracket->type = CPPS_OBRACKET;

		buffer.pop(); // pop (

		cpps_parse_rmspaceandenter(buffer);


		cpps_parse_expression(domain, bracket, root, buffer);

		cpps_parse_rmspaceandenter(buffer);

		buffer.pop(); // pop )
		return bracket;
	}
	Node* cpps_parse_object(cpps_domain* domain, Node* o, Node* root, cppsbuffer& buffer)
	{
		Node* bracket = new Node(o->filename, buffer.line());
		bracket->type = CPPS_OOBJECT;

		buffer.pop(); // pop {

		cpps_parse_rmspaceandenter(buffer);


		while (!buffer.isend())
		{
			//�޳��س�.
			cpps_parse_rmspaceandenter(buffer);
			//�Ƿ�����ˡ�
			if (buffer.cur() == '}')
			{
				buffer.pop();
				return bracket;
			}


			Node* n = new Node(bracket, o->filename, buffer.line());
			Node* k = new Node(n, o->filename, buffer.line());
			k->s = cpps_parse_varname(buffer);

			//�޳��س�.
			cpps_parse_rmspaceandenter(buffer);

			if (buffer.cur() != ':')
				throw(cpps_error(bracket->filename, buffer.line(), cpps_error_arrayeerror, "Definition object not detected '}'."));

			buffer.pop();

			//�޳��س�.
			cpps_parse_rmspaceandenter(buffer);

			Node* v = new Node(n, o->filename, buffer.line());
			cpps_parse_expression(domain, v, root, buffer);
			//�Ƿ�����ˡ�
			if (buffer.cur() == ',')
			{
				buffer.pop();
			}
		}


		throw(cpps_error(bracket->filename, buffer.line(), cpps_error_arrayeerror, "Definition object not detected '}'"));
		return bracket;
	}
	Node* cpps_parse_new_setv(cpps_domain* domain, Node* o, Node* root, cppsbuffer& buffer)
	{
		Node* bracket = new Node(o->filename, buffer.line());
		bracket->type = CPPS_ONEW_SETV;

		buffer.pop(); // pop {

		cpps_parse_rmspaceandenter(buffer);


		while (!buffer.isend())
		{
			//�޳��س�.
			cpps_parse_rmspaceandenter(buffer);
			//�Ƿ�����ˡ�
			if (buffer.cur() == '}')
			{
				buffer.pop();
				return bracket;
			}


			Node* n = new Node(bracket, o->filename, buffer.line());
			Node* k = new Node(n, o->filename, buffer.line());
			k->s = cpps_parse_varname(buffer);

			//�޳��س�.
			cpps_parse_rmspaceandenter(buffer);

			if (buffer.cur() != '=')
				throw(cpps_error(bracket->filename, buffer.line(), cpps_error_arrayeerror, "'=' was not detected when setting the variable"));

			buffer.pop();

			//�޳��س�.
			cpps_parse_rmspaceandenter(buffer);

			Node* v = new Node(n, o->filename, buffer.line());
			cpps_parse_expression(domain, v, root, buffer);
			//�Ƿ�����ˡ�
			if (buffer.cur() == ',')
			{
				buffer.pop();
			}
		}


		throw(cpps_error(bracket->filename, buffer.line(), cpps_error_arrayeerror, "'}' was not detected when setting the variable"));
		return bracket;
	}
	Node* cpps_parse_array(cpps_domain* domain, Node* o, Node* root, cppsbuffer& buffer)
	{
		Node* bracket = new Node(o->filename, buffer.line());
		bracket->type = CPPS_OARRAY;

		buffer.pop(); // pop [

		cpps_parse_rmspaceandenter(buffer);


		while (!buffer.isend())
		{
			//�޳��س�.
			cpps_parse_rmspaceandenter(buffer);
			//�Ƿ�����ˡ�
			if (buffer.cur() == ']')
			{
				buffer.pop();
				return bracket;
			}
			Node* n = new Node(bracket, o->filename, buffer.line());
			cpps_parse_expression(domain, n, root, buffer);
			//�Ƿ�����ˡ�
			if (buffer.cur() == ',')
			{
				buffer.pop();
			}
		}


		throw(cpps_error(bracket->filename, buffer.line(), cpps_error_arrayeerror, "Definition array not detected '}'"));
		return bracket;
	}

	Node* cpps_parse_last_func(cppsbuffer& buffer, Node* o, Node* p, cpps_domain* domain, Node* root)
	{
		//�к���
		while (buffer.cur() == '[' || buffer.cur() == '.' || buffer.cur() == '(' || buffer.cur() == ':')
		{
			//
			if (buffer.cur() == '.')
				if (cpps_parse_isnotvarname(buffer.at(buffer.offset() + 1))) break;

			char symblo = buffer.pop();

			Node* geto = new Node(o->filename, buffer.line());

			geto->addtoleft(p);
			Node* child = NULL;

			if (symblo == '[')
			{
				geto->type = CPPS_OGETCHIILD;
				child = new Node(o->filename, buffer.line());
				cpps_parse_expression(domain, child, root, buffer);
				if (!child) throw(cpps_error(o->filename, buffer.line(), cpps_error_unknow, "'[' must be followed by a parameter"));
				if (buffer.cur() != ']')  throw(cpps_error(o->filename, buffer.line(), cpps_error_unknow, "']' parsing error"));
				buffer.pop();
			}
			else if (symblo == '.')
			{
				geto->type = CPPS_OGETOBJECT;
				child = cpps_parse_var_param(domain, geto, root, buffer);
				if (!child) throw(cpps_error(o->filename, buffer.line(), cpps_error_unknow, "The variable name is required later on '.'"));
			}
			else if (symblo == '(') //�ǵ��ú���
			{

				//��ȡ���� ����
				geto->type = CPPS_ODOFUNCTION;
				child = new Node(o->filename, buffer.line());
				cpps_domain* funcdomain = new cpps_domain(domain, cpps_domain_type_func, o->s);//Ϊ���������һ���Լ�����ռ�
				child->setdomain(funcdomain);

				cpps_parse_dofunction(funcdomain, child, root, buffer);

			}
			else if (symblo == ':')
			{
				if (buffer.cur() != ':')
					throw(cpps_error(o->filename, buffer.line(), cpps_error_unknow, "namespace requires double ':'"));

				buffer.pop();

				geto->type = CPPS_OGETOBJECT;
				child = cpps_parse_var_param(domain, geto, root, buffer);
				if (!child) throw(cpps_error(o->filename, buffer.line(), cpps_error_unknow, "namespace:: need a name is required later."));

			}

			geto->addtoright(child); //�ŵ�������

			p = geto;
		}
		return p;
	}

	Node* cpps_parse_param(cpps_domain* domain, Node* o, Node* root, cppsbuffer& buffer)
	{

		char ch = buffer.cur();
		if (ch == '"')
		{
			return cpps_parse_string(domain, o, root, buffer, '"');
		}
		else if (ch == '\'')
		{
			return cpps_parse_string(domain, o, root, buffer, '\'');
		}
		else if (ch == '{')
		{
			return cpps_parse_object(domain, o, root, buffer);
		}
		else if (ch == '[')
		{
			return cpps_parse_array(domain, o, root, buffer);
		}
		else if (cpps_parse_isnumber(ch)) //���ֲ���   s
		{
			return cpps_parse_number(domain, o, buffer);
		}
		else if (!cpps_parse_isnotvarname(ch)) //��������
		{
			Node* p = cpps_parse_var_param(domain, o, root, buffer);

			p = cpps_parse_last_func(buffer, o, p, domain, root);

			return p;
		}
		else if (ch == '(')
		{
			return cpps_parse_bracket(domain, o, root, buffer);
		}
		return NULL;
	}
	Node* cpps_parse_symbol(cpps_domain* domain, Node* o, cppsbuffer& buffer, bool leftsymbol)
	{
		std::string symbolStr;
		std::string tempStr;
		char ch = buffer.cur();
		int32 curoffset = buffer.offset();
		while (cpps_parse_issymbol(ch))
		{
			tempStr += ch;
			cpps_symbol* symbol = cpps_parse_getsymbol(tempStr, leftsymbol);
			if (!symbol)
			{
				if (symbolStr.empty() && !cpps_parse_issymbol(buffer.at(curoffset + 1)))
				{
					return NULL;
				}
			}
			else
			{
				symbolStr = tempStr;
			}
			curoffset += 1;
			ch = buffer.at(curoffset);
		}

		cpps_symbol* symbol = cpps_parse_getsymbol(symbolStr, leftsymbol);
		if (!symbol)
		{
			return NULL;
		}

		buffer.seek(buffer.offset() + (int32)symbolStr.size());
		Node* pNode = new Node(o->filename, buffer.line());
		pNode->type = CPPS_FUNCNAME;
		pNode->symbol = symbol;
		pNode->s = symbol->symbolfuncname;
		return pNode;
	}

	void cpps_parse_expression(cpps_domain* domain, Node* param, Node* root, cppsbuffer& buffer)
	{
		Node* lastOpNode = param;
		while (cpps_parse_expression_step(domain, param, lastOpNode, root, buffer) == CPPS_NOERROR);
	}
	std::string buildlambda()
	{
		std::string ret = "lamdba@";
		int i;
		int len = 20;
		for (i = 0; i < len; ++i)
			ret += 'A' + rand() % 26;
		return ret;
	}
	Node* cpps_get_root_node(Node* n)
	{
		if (n->parent)
			return cpps_get_root_node(n->parent);
		else
			return n;
	}
	int32 cpps_parse_expression_step(cpps_domain* domain, Node* param, Node*& lastOpNode, Node* root, cppsbuffer& buffer)
	{
		//������������
		if (buffer.cur() == '[' && buffer.at(buffer.offset() + 1) == ']' && buffer.at(buffer.offset() + 2) == '(')
		{
			buffer.seek(buffer.offset() + 3);


			Node* lamdbavar = new Node(param->filename, buffer.line());
			lamdbavar->type = CPPS_ODEFVAR;
			lamdbavar->setParent(root);
			root->l.insert(root->l.begin(), lamdbavar);

			Node* str = new Node(lamdbavar, param->filename, buffer.line());
			str->s = buildlambda();
			str->type = CPPS_VARNAME;


			Node* lambda = new Node(str, param->filename, buffer.line());
			lambda->type = CPPS_ODEFVAR_FUNC;
			cpps_parse_def_function(domain, lambda, root, buffer);//������һ������


			Node* lambdaparam = new Node(param->filename, buffer.line());
			lambdaparam->type = CPPS_VARNAME_LAMBDA;
			lambdaparam->s = str->s;
			lambdaparam->setParent(lastOpNode);

			lambdaparam = cpps_parse_last_func(buffer, lastOpNode, lambdaparam, domain, root);

			lastOpNode->l.push_back(lambdaparam);

			return 1;
		}


		Node* p = NULL;

		Node* leftoproot = NULL;
		Node* leftopparent = NULL;
		Node* leftop = NULL;
		while ((leftop = cpps_parse_symbol(domain, param, buffer, true)) && leftop->symbol && leftop->symbol->getparamnum() == 1)
		{
			if (leftoproot == NULL)
			{
				leftoproot = leftop;
				leftopparent = leftop;
			}
			else
			{
				leftopparent->addtoleft(leftop);
				leftopparent = leftop;
			}

			cpps_parse_rmspaceandenter(buffer);
		}

		p = cpps_parse_param(domain, param, root, buffer);
		if (!p)
		{
			//û�в���
			throw(cpps_error(param->filename, buffer.line(), cpps_error_paramerror, "Expression parsing error."));
		}


		cpps_parse_rmspaceandenter(buffer);

		Node* op = cpps_parse_symbol(domain, param, buffer);

		if (op == NULL)
		{
			if (leftoproot != NULL)
			{
				leftopparent->addtoleft(p);
				p = leftoproot;

			}
			if (!lastOpNode->symbol)
			{
				p->setParent(lastOpNode);
				lastOpNode->l.push_back(p);
			}
			else if (lastOpNode->type == CPPS_FUNCNAME)
			{
				if (lastOpNode->symbol && lastOpNode->symbol->getparamnum() > 2)
					lastOpNode->add(p);
				else
					lastOpNode->addtoright(p);
			}
			else
			{
				lastOpNode->addtoleft(p);
			}
			return 1;
		}


		//��Ԫ����ʽ
		if (op->s == "?")
		{
			if (lastOpNode && lastOpNode->type == CPPS_FUNCNAME && lastOpNode->symbol && op->symbol->getprio() > lastOpNode->symbol->getprio())
			{
				if (leftoproot != NULL)
				{
					leftopparent->addtoleft(p);
					p = leftoproot;

				}
				if (!lastOpNode->symbol)
				{
					p->setParent(lastOpNode);
					lastOpNode->l.push_back(p);
				}
				else if (lastOpNode->type == CPPS_FUNCNAME)
				{
					if (lastOpNode->symbol && lastOpNode->symbol->getparamnum() > 2)
						lastOpNode->add(p);
					else
						lastOpNode->addtoright(p);
				}
				else
				{
					lastOpNode->addtoleft(p);
				}
				p = lastOpNode;

				op->setParent(lastOpNode->parent);


				bool isLeft = (lastOpNode == lastOpNode->parent->getleft());
				if (isLeft)
					lastOpNode->parent->addtoleft(op);
				else
					lastOpNode->parent->addtoright(op);

				lastOpNode->parent = op;

				lastOpNode = op;
			}
			op->add(p);

			cpps_parse_rmspaceandenter(buffer);

			cpps_parse_expression(domain, op, root, buffer);

			cpps_parse_rmspaceandenter(buffer);

			if (buffer.cur() != ':')
			{
				throw(cpps_error(param->filename, buffer.line(), cpps_error_paramerror, "Ternary expression parsing error."));
			}
			buffer.pop();

			cpps_parse_rmspaceandenter(buffer);


			cpps_parse_expression(domain, op, root, buffer);

			p = op;

			op = cpps_parse_symbol(domain, param, buffer);


			if (op == NULL)
			{
				if (lastOpNode == p)
				{
					return 1;
				}
				if (leftoproot != NULL)
				{
					leftopparent->addtoleft(p);
					p = leftoproot;

				}
				if (!lastOpNode->symbol)
				{
					p->setParent(lastOpNode);
					lastOpNode->l.push_back(p);
				}
				else if (lastOpNode->type == CPPS_FUNCNAME)
				{
					if (lastOpNode->symbol && lastOpNode->symbol->getparamnum() > 2)
						lastOpNode->add(p);
					else
						lastOpNode->addtoright(p);
				}
				else
				{
					lastOpNode->addtoleft(p);
				}

				return	1;
			}

		}



		if (op->symbol && op->symbol->getparamnum() == 1)
		{
			if (leftoproot != NULL)
			{
				leftopparent->addtoleft(p);
				p = leftoproot;

			}

			op->addtoleft(p);

			p = op;

			op = cpps_parse_symbol(domain, param, buffer);

			if (op == NULL)
			{
				if (!lastOpNode->symbol)
				{
					p->setParent(lastOpNode);
					lastOpNode->l.push_back(p);
				}
				else if (lastOpNode->type == CPPS_FUNCNAME)
				{
					if (lastOpNode->symbol->getparamnum() > 2)
						lastOpNode->add(p);
					else
						lastOpNode->addtoright(p);
				}
				else
				{
					lastOpNode->addtoleft(p);
				}
				return 1;
			}
		}


		cpps_parse_rmspaceandenter(buffer);

		op->addtoleft(p);

		Node* take = op;

		if (leftoproot != NULL)
		{
			leftopparent->addtoleft(op);
			op = leftoproot;
		}

		if (lastOpNode->type == CPPS_FUNCNAME && lastOpNode->symbol)
		{
			if (lastOpNode->symbol->getparamnum() > 2)
				lastOpNode->add(op);
			else
				lastOpNode->addtoright(op);
		}
		else
		{
			p->setParent(lastOpNode);
			lastOpNode->l.push_back(op);

			if (lastOpNode->symbol && lastOpNode->symbol->getparamleftlimit())
			{
				if (op->type != CPPS_VARNAME && op->type != CPPS_OGETOBJECT && op->type != CPPS_OGETCHIILD)
				{
					throw(cpps_error(op->filename, op->line, cpps_error_paramerror, "The left side of %s must be a variable", lastOpNode->s.c_str()));
				}
				if (op->type == CPPS_VARNAME)
					op->type = CPPS_QUOTEVARNAME;
				else if (op->type == CPPS_OGETOBJECT)
					op->type = CPPS_QUOTEGETOBJECT;
				else if (op->type == CPPS_OGETCHIILD)
					op->type = CPPS_QUOTEGETCHIILD;

			}
			if (lastOpNode->symbol && lastOpNode->symbol->getparamrightlimit()
				&& lastOpNode->symbol->getparamnum() == 1)
			{
				if (op->type != CPPS_VARNAME && op->type != CPPS_OGETOBJECT && op->type != CPPS_OGETCHIILD)
				{
					throw(cpps_error(op->filename, op->line, cpps_error_paramerror, " The right side of %s must be a variable", lastOpNode->s.c_str()));
				}
				if (op->type == CPPS_VARNAME)
					op->type = CPPS_QUOTEVARNAME;
				else if (op->type == CPPS_OGETOBJECT)
					op->type = CPPS_QUOTEGETOBJECT;
				else if (op->type == CPPS_OGETCHIILD)
					op->type = CPPS_QUOTEGETCHIILD;
			}

			op->setParent(lastOpNode);
		}


		//�ҵ���Ҫ�����ĵط���Ȼ�󽻻���
		Node* compareOp = op->parent;
		bool needchange = false;
		bool isLeft = true;
		while (true)
		{
			if (compareOp->type != CPPS_FUNCNAME)
			{
				break;
			}
			if (compareOp->symbol == NULL)
			{
				break;
			}
			if (op->symbol == NULL)
			{
				break;
			}
			if (op->symbol->getprio() < compareOp->symbol->getprio())
			{
				break;
			}
			if (op->symbol->getprio() == compareOp->symbol->getprio() && !cpps_parse_isleftasso(op->symbol->getprio()))
			{
				break;
			}

			isLeft = (compareOp == compareOp->parent->getleft());
			compareOp = compareOp->parent;
			needchange = true;
		}
		if (needchange)
		{
			op->parent->addtoright(op->getleft());
			if (isLeft)
			{
				op->addtoleft(compareOp->getleft());
				compareOp->addtoleft(op);
			}
			else
			{
				op->addtoleft(compareOp->getright());
				compareOp->addtoright(op);
			}
		}


		lastOpNode = take;

		return	CPPS_NOERROR;
	}

	void cpps_parse_dofunction(cpps_domain* domain, Node* param, Node* root, cppsbuffer& buffer)
	{

		char ch = 0;


		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);


		while (!buffer.isend())
		{
			ch = buffer.cur();
			if (ch == ')')
			{
				buffer.pop();
				return;
			}
			if (ch == ',')
				buffer.pop();


			//�޳��ո�
			cpps_parse_rmspaceandenter(buffer);

			cpps_parse_expression(domain, param, root, buffer);
		}



		//δ�ҵ��� �ͷ����ˡ� ���Ǵ��İ�����
		throw(cpps_error(param->filename, buffer.line(), cpps_error_paramerror, "The calling function did not find ')'."));

	}

	void cpps_parse_assemble(cpps_domain* domain, Node* o, Node* root, cppsbuffer& buffer)
	{
		cpps_domain* assdomain = new cpps_domain(domain, cpps_domain_type_ass, "assemble");//Ϊ���������һ���Լ�����ռ�
		o->setdomain(assdomain);

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);

		int holdline = buffer.line();

		while (!buffer.isend())
		{
			//�޳��س�.
			cpps_parse_rmspaceandenter(buffer);

			//�Ƿ�����ˡ�
			if (buffer.cur() == '}')
			{
				buffer.pop();
				return;
			}
			cpps_parse_line(assdomain, o, root, buffer);
		}

		throw(cpps_error(o->filename, holdline, cpps_error_assembleerror, "The '}' of the execution collection was not found."));

	}
	void cpps_parse_def(cpps_domain* domain, Node* child, Node* root, cppsbuffer& buffer, int32 limit)
	{
		char ch = buffer.cur();

		if (!cpps_parse_isnotvarname(ch)) //���Ǹ�����. ��Ӧ���Ƕ������
		{
			if (child->s == "var")
			{
				child->type = CPPS_ODEFVAR;
			}

			cpps_parse_var(domain, child, root, buffer, limit, false);

			//�������͵ı���

		}
		else if (!child->s.empty())
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_varerror, "Missing '=' in front of '%c'.", ch));
		}


	}
	void cpps_parse_if(cpps_domain* domain, Node* child, Node* root, cppsbuffer& buffer)
	{
		cpps_domain* ifdomain = new cpps_domain(domain, cpps_domain_type_if, "if");//Ϊ���������һ���Լ�����ռ�
		child->setdomain(ifdomain);

		child->type = CPPS_OIF;

		Node* param = new Node(child, child->filename, buffer.line());// 0 if�Ĳ���
		Node* t = new Node(child, child->filename, buffer.line()); // true 
		Node* f = new Node(child, child->filename, buffer.line()); // false

		if (buffer.cur() != '(')
		{
			// if ��������() 
			throw(cpps_error(child->filename, buffer.line(), cpps_error_iferror, "Missing '('after if."));
		}
		buffer.pop();


		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);

		cpps_parse_expression(ifdomain, param, root, buffer);

		cpps_parse_rmspaceandenter(buffer);

		if (buffer.cur() != ')')
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_iferror, "Missing ')' after if."));
		}
		buffer.pop();

		cpps_parse_rmspaceandenter(buffer);

		cpps_parse_line(ifdomain, t, root, buffer);

		cpps_parse_rmspaceandenter(buffer);

		int32 offset = buffer.offset(); //��ס��ǰ���ַ��� ��Ϊ��һ����ܲ���else

		std::string el = cpps_parse_varname(buffer);
		if (el == "else")
		{
			cpps_parse_rmspaceandenter(buffer);

			cpps_parse_line(domain, f, root, buffer);
		}
		else
		{
			buffer.seek(offset);//rollback �ع�������
		}
	}
	bool cpps_parse_canreturn(cpps_domain* domain)
	{
		if (domain->domainType == cpps_domain_type_func)
		{
			return true;
		}
		if (domain->parent[0] && cpps_parse_canreturn(domain->parent[0]))
		{
			return true;
		}
		return false;
	}
	bool cpps_parse_canthrow(cpps_domain* domain)
	{
		if (domain->domainType == cpps_domain_type_trycatch)
		{
			return true;
		}
		if (domain->parent[0] && cpps_parse_canthrow(domain->parent[0]))
		{
			return true;
		}
		return false;
	}
	bool cpps_parse_canbreak(cpps_domain* domain)
	{
		if (domain->domainType == cpps_domain_type_for || domain->domainType == cpps_domain_type_foreach || domain->domainType == cpps_domain_type_while)
		{
			return true;
		}
		if (domain->parent[0] && cpps_parse_canbreak(domain->parent[0]))
		{
			return true;
		}
		return false;
	}
	bool cpps_parse_cancontinue(cpps_domain* domain)
	{
		if (domain->domainType == cpps_domain_type_for || domain->domainType == cpps_domain_type_while)
		{
			return true;
		}
		if (domain->parent[0] && cpps_parse_cancontinue(domain->parent[0]))
		{
			return true;
		}
		return false;
	}
	void cpps_parse_return(cpps_domain* domain, Node* child, Node* root, cppsbuffer& buffer)
	{
		//�ȼ�� ���return �Ƿ�Ϸ���
		if (!cpps_parse_canreturn(domain))
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_deffuncrror, "Unknown return. Return must be defined in function."));
		}

		child->type = CPPS_ORETURN;

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);

		//�������û�в����Ǿ���������һ��nil��
		if (buffer.cur() == ';')
		{
			buffer.pop();
			Node* nil = new Node(child, child->filename, child->line);
			return;
		}
		//���Һ�������
		cpps_parse_expression(domain, child, root, buffer);
	}

	void cpps_parse_throw(cpps_domain* domain, Node* child, Node* root, cppsbuffer& buffer)
	{
		//�ȼ�� ���return �Ƿ�Ϸ���
		if (!cpps_parse_canthrow(domain))
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_trycatherror, "Unknown throw. Throw must be defined after try!"));
		}

		child->type = CPPS_OTHROW;

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);

		//���Һ�������
		cpps_parse_expression(domain, child, root, buffer);
	}
	void cpps_parse_for(cpps_domain* domain, Node* child, Node* root, cppsbuffer& buffer)
	{
		cpps_domain* fordomain = new cpps_domain(domain, cpps_domain_type_for, "for");
		child->setdomain(fordomain);

		child->type = CPPS_OFOR;

		Node* for1 = new Node(child, child->filename, buffer.line());
		Node* for2 = new Node(child, child->filename, buffer.line());
		Node* for3 = new Node(child, child->filename, buffer.line());
		Node* for4 = new Node(child, child->filename, buffer.line());

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() != '(')
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_forerror, "Missing '(' after for"));
		}
		buffer.pop(); //pop (


		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() != ';')
			cpps_parse_line(fordomain, for1, root, buffer, CPPS_NOT_DEFASSEMBLE | CPPS_NOT_DEFFUNCTION | CPPS_NOT_USEBUILTIN | CPPS_NOT_DONTDELETEEND );

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() != ';')
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_forerror, "Missing first ';' after for"));
		}
		buffer.pop(); // pop ;


		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() != ';')
			cpps_parse_expression(fordomain, for2, root, buffer);


		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() != ';')
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_forerror, "Missing second ';' after for"));
		}
		buffer.pop(); // pop 


		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() != ')')
			cpps_parse_line(fordomain, for3, root, buffer, CPPS_NOT_DEFASSEMBLE | CPPS_NOT_DEFFUNCTION | CPPS_NOT_USEBUILTIN | CPPS_NOT_DEFVAR | CPPS_NOT_DONTDELETEEND);



		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() != ')')
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_forerror, "Missing ')'after for"));
		}
		buffer.pop(); // pop )


		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);

		cpps_parse_line(fordomain, for4, root, buffer);
	}
	void cpps_parse_foreach(cpps_domain* domain, Node* child, Node* root, cppsbuffer& buffer)
	{
		cpps_domain* fordomain = new cpps_domain(domain, cpps_domain_type_for, "for");
		child->setdomain(fordomain);

		child->type = CPPS_OFOREACH;

		Node* for1 = new Node(child, child->filename, buffer.line());
		Node* for2 = new Node(child, child->filename, buffer.line());
		Node* for4 = new Node(child, child->filename, buffer.line());

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() != '(')
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_forerror, "Missing '('after foreach"));
		}
		buffer.pop(); //pop (


		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);

		// var
		std::string s = cpps_parse_varname(buffer);
		if (s != "var")
			throw(cpps_error(child->filename, buffer.line(), cpps_error_forerror, "Foreach should start with 'var'"));

		//varname
		cpps_parse_rmspaceandenter(buffer);
		for1->s = cpps_parse_varname(buffer);

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() != ':')
			throw(cpps_error(child->filename, buffer.line(), cpps_error_forerror, "Missing ':'after foreach"));
		buffer.pop();

		//for2 �޸ĳɻ�ȡ����ʽ
		cpps_parse_rmspaceandenter(buffer);
		cpps_parse_expression(fordomain, for2, root, buffer);

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() != ')')
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_forerror, "Missing ')'after foreach"));
		}
		buffer.pop(); // pop )


		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);

		cpps_parse_line(fordomain, for4, root, buffer);
	}
	void cpps_parse_while(cpps_domain* domain, Node* child, Node* root, cppsbuffer& buffer)
	{
		cpps_domain* whiledomain = new cpps_domain(domain, cpps_domain_type_while, "while");
		child->setdomain(whiledomain);

		child->type = CPPS_OWHILE;


		Node* while1 = new Node(child, child->filename, buffer.line());
		Node* while2 = new Node(child, child->filename, buffer.line());

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() != '(')
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_whileerror, "Missing '('after while"));
		}
		buffer.pop(); //pop (

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		cpps_parse_expression(whiledomain, while1, root, buffer);

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() != ')')
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_whileerror, "Missing ')'after while"));
		}
		buffer.pop(); // pop )


		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);

		cpps_parse_line(whiledomain, while2, root, buffer);
	}
	void cpps_parse_class(cpps_domain* domain, Node* child, Node* root, cppsbuffer& buffer)
	{
		child->type = CPPS_OCLASS;

		Node* parents = new Node(child, child->filename, buffer.line());
		Node* vars = new Node(child, child->filename, buffer.line());

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		child->s = cpps_parse_varname(buffer);


		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() == ':')
		{
			//˵���м̳й�ϵ��
			buffer.pop();

			while (!buffer.isend())
			{
				//�޳��ո�
				cpps_parse_rmspaceandenter(buffer);
				Node* parent = new Node(parents, parents->filename, buffer.line());
				parent->s = cpps_parse_varname(buffer);
				// 2020-11-10 ����֧�ּ̳�namespace 
				Node* lastNode = parent;
				//�Ƿ�ʹ�����ռ�
				while (!buffer.isend())
				{
					//�޳��س�.
					cpps_parse_rmspaceandenter(buffer);
					if (buffer.cur() == ':' && buffer.at(buffer.offset() + 1) == ':')
					{
						buffer.pop();
						buffer.pop();

						cpps_parse_rmspaceandenter(buffer);

						Node* child = new Node(lastNode, parents->filename, buffer.line());
						child->s = cpps_parse_varname(buffer);
						child->type = CPPS_ONAMESPANCE_CHILD;
						lastNode = child;
					}
					else
					{
						break;
					}
				}

				if (buffer.cur() != ',')
				{
					break;
				}

				buffer.pop();
			}
		}

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() != '{')
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_classerror, "Missing '{' after class"));
		}
		buffer.pop(); //pop }


		while (!buffer.isend())
		{
			//�޳��س�.
			cpps_parse_rmspaceandenter(buffer);

			//�Ƿ�����ˡ�
			if (buffer.cur() == '}')
			{
				break;
			}
			if (buffer.isend())
			{
				throw(cpps_error(child->filename, buffer.line(), cpps_error_classerror, "Missing '}'after class"));
			}
			cpps_parse_line(domain, vars, root, buffer, CPPS_NOT_DEFASSEMBLE | CPPS_NOT_USEBUILTIN);
		}

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() != '}')
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_classerror, "Missing '}'after class"));
		}
		buffer.pop(); //pop }

	}
	void cpps_parse_import(cpps_domain* domain, Node* child, Node* root, cppsbuffer& buffer)
	{
		child->type = CPPS_OIMPORT;
		cpps_parse_rmspaceandenter(buffer);
		cpps_parse_expression(domain, child, root, buffer);
	}

	void cpps_parse_include(cpps_domain* domain, Node* child, Node* root, cppsbuffer& buffer)
	{
		child->type = CPPS_OINCLUDE;

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() != '(')
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_iferror, "Missing '(' after include"));
		}
		buffer.pop(); //pop (

		while (!buffer.isend())
		{
			//�޳��س�.
			cpps_parse_rmspaceandenter(buffer);

			//�Ƿ�����ˡ�
			if (buffer.cur() == ')')
			{
				break;
			}

			if (buffer.isend())
			{
				throw(cpps_error(child->filename, buffer.line(), cpps_error_classerror, "Missing ')'after include"));
			}
			cpps_parse_expression(domain, child, root, buffer);

			//�Ƿ�����ˡ�
			if (buffer.cur() == ',')
			{
				buffer.pop(); // pop ,
			}
		}

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() != ')')
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_iferror, "Missing '('after include"));
		}
		buffer.pop(); // pop )

	}

	void cpps_parse_dofile(cpps_domain* domain, Node* child, Node* root, cppsbuffer& buffer)
	{
		child->type = CPPS_ODOFILE;

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() != '(')
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_iferror, "Missing '(' after dofile"));
		}
		buffer.pop(); //pop (

		while (!buffer.isend())
		{
			//�޳��س�.
			cpps_parse_rmspaceandenter(buffer);

			//�Ƿ�����ˡ�
			if (buffer.cur() == ')')
			{
				break;
			}

			if (buffer.isend())
			{
				throw(cpps_error(child->filename, buffer.line(), cpps_error_classerror, "Missing ')' after dofile"));
			}
			cpps_parse_expression(domain, child, root, buffer);

			//�Ƿ�����ˡ�
			if (buffer.cur() == ',')
			{
				buffer.pop(); // pop ,
			}
		}

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() != ')')
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_iferror, "Missing ')' after dofile"));
		}
		buffer.pop(); // pop )

	}
	void cpps_parse_dostring(cpps_domain* domain, Node* child, Node* root, cppsbuffer& buffer)
	{
		child->type = CPPS_ODOSTRING;

		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() != '(')
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_iferror, "Missing '(' after dostring"));
		}
		buffer.pop(); //pop (


		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		cpps_parse_expression(domain, child, root, buffer);


		//�޳��ո�
		cpps_parse_rmspaceandenter(buffer);
		if (buffer.cur() != ')')
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_iferror, "Missing ')' after dostring"));
		}
		buffer.pop(); // pop )

	}

	void cpps_parse_break(cpps_domain* domain, Node* child, cppsbuffer& buffer)
	{
		if (!cpps_parse_canbreak(domain))
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_deffuncrror, "Unknown break must be defined in while or for."));
		}

		child->type = CPPS_OBREAK;
	}

	void cpps_parse_continue(cpps_domain* domain, Node* child, cppsbuffer& buffer)
	{
		if (!cpps_parse_cancontinue(domain))
		{
			throw(cpps_error(child->filename, buffer.line(), cpps_error_deffuncrror, "Unknown continue must be defined in while or for"));
		}

		child->type = CPPS_OCONTINUE;
	}
	void cpps_parse_builtin(cpps_domain* domain, Node* child, Node* root, cppsbuffer& buffer, int32 limit)
	{
		cpps_parse_rmspaceandenter(buffer);

		if (child->s == "if")
		{
			cpps_parse_if(domain, child, root, buffer);
		}
		else if (child->s == "return")
		{
			cpps_parse_return(domain, child, root, buffer);
		}
		else if (child->s == "throw")
		{
			cpps_parse_throw(domain, child, root, buffer);
		}
		else if (child->s == "for")
		{
			cpps_parse_for(domain, child, root, buffer);
		}
		else if (child->s == "foreach")
		{
			cpps_parse_foreach(domain, child, root, buffer);
		}
		else if (child->s == "while")
		{
			cpps_parse_while(domain, child, root, buffer);
		}
		else if (child->s == "class")
		{
			cpps_parse_class(domain, child, root, buffer);
		}
		else if (child->s == "dofile")
		{
			cpps_parse_dofile(domain, child, root, buffer);
		}
		else if (child->s == "import")
		{
			cpps_parse_import(domain, child, root, buffer);
		}
		else if (child->s == "include")
		{
			cpps_parse_include(domain, child, root, buffer);
		}
		else if (child->s == "dostring")
		{
			cpps_parse_dostring(domain, child, root, buffer);
		}
		else if (child->s == "break")
		{
			cpps_parse_break(domain, child, buffer);
		}
		else if (child->s == "continue")
		{
			cpps_parse_continue(domain, child, buffer);
		}
		else if (child->s == "const")
		{
			child->s = cpps_parse_varname(buffer);
			cpps_parse_rmspaceandenter(buffer);
			if (child->s == "var")
				cpps_parse_var(domain, child, root, buffer, limit, true);
			else
				throw(cpps_error(child->filename, buffer.line(), cpps_error_normalerror, "const needs to use var later.", buffer.cur()));

		}
		else if (child->s == "var")
		{
			cpps_parse_var(domain, child, root, buffer, limit, false);
		}
		else if (child->s == "namespace")
		{
			cpps_parse_namespace(domain, child, root, buffer, limit);
		}
		else if (child->s == "try")
		{
			cpps_parse_trycatch(domain, child, root, buffer, limit);
		}
	}
	Node* cpps_parse_line(cpps_domain* domain, Node* o, Node* root, cppsbuffer& buffer, int32 limit)
	{


		Node* child = new Node(o, o->filename, buffer.line());

		if (buffer.cur() == '{')
		{
			if (limit & CPPS_NOT_DEFASSEMBLE)
			{
				throw(cpps_error(o->filename, buffer.line(), cpps_error_normalerror, "Definition assemble not allowed"));
			}

			buffer.pop();
			//���Ǹ���~
			child->type = CPPS_OASSEMBLE;
			cpps_parse_assemble(domain, child, root, buffer);
		}
		else
		{
			if (cpps_parse_isnumber(buffer.cur()))
			{
				//����ĸΪ ���ֵĻ��϶������⡣�붼���롣��
				throw(cpps_error(o->filename, buffer.line(), cpps_error_normalerror, "The first letter of an expression cannot be a number '%c'", buffer.cur()));
			}
			int32 offset = buffer.offset(); //��¼һ�� �ȼ���Ƿ�Ϊ����ʽ


			child->s = cpps_parse_varname(buffer);

			//�޳��ո�
			cpps_parse_rmspaceandenter(buffer);

			//������ʽ

			if (cpps_parse_isbuiltinname(child->s))
			{
				if (limit & CPPS_NOT_USEBUILTIN && child->s != "var" && child->s != "const")
				{
					throw(cpps_error(o->filename, buffer.line(), cpps_error_normalerror, "builtin are not allowed"));
				}

				cpps_parse_builtin(domain, child, root, buffer, limit);
			}
			else if (o->parent && child->s == o->parent->s)//���캯��
			{
				//�޳��ո�
				child->type = CPPS_ODEFVAR;

				cpps_parse_rmspaceandenter(buffer);

				Node* str = new Node(child, child->filename, buffer.line());
				str->type = CPPS_VARNAME;

				//��������
				str->s = "constructor";

				cpps_parse_var_right(domain, str, root, buffer, CPPS_NOT_DEFVAR);

			}
			else
			{
				//����Ǳ���ʽ�Ļ�
				buffer.seek(offset);//���û�ȥ 
				child->type = CPPS_OEXPRESSION;
				cpps_parse_expression(domain, child, root, buffer);

			}


			//�޳��ո�
			cpps_parse_rmspaceandenter(buffer);

			if ((limit & CPPS_NOT_DONTDELETEEND) == 0)
			{
				if (buffer.cur() != ';')
				{
					//δ�ҵ�;�š� �ò��ñ����أ�

				}
				else if (buffer.cur() == ';')
				{
					buffer.pop();
				}
			}
		}
		return child;
	}

	Node* loadbuffer(C* c, cpps_domain* domain, std::string str, std::string filename)
	{
		//�����Ҫת��
		if (c->func)
			str = c->func(str);

		Node* o = new Node(filename);
		o->type = CPPS_ROOT;
		cppsbuffer buffer(filename.c_str(), str.c_str(), (int32)str.size());
		while (true)
		{
			//�޳��س�.
			cpps_parse_rmspaceandenter(buffer);
			//�Ƿ�����ˡ�
			if (buffer.isend())
			{
				break;
			}
			cpps_parse_line(domain, o, o, buffer);
		}
		return o;//���ظ��ڵ�


		return NULL;
	}
	void cpps_create_root_G(C* c)
	{
		c->_G = new cpps_domain(NULL, cpps_domain_type_root, "root");//�������ڵ���
		c->_G->setGCLevel(2); //���ô��� ���ǳ���û�ˡ�
		cpps_regvar* v = new cpps_regvar();//_G Ϊ���ڵ�
		v->setVarName("_G");
		v->setValue(cpps_value(c->_G)); //���б���copy��ȥ
		c->_G->regVar(NULL, v); //���Լ�ע���_G..
	}

	cpps::C* create()
	{
		C* c = new cpps::C();

		cpps_create_root_G(c);

		cpps_regsymbols(c);
		cpps_regbase(c);
		cpps_regmath(c);
		cpps_regmap(c);
		cpps_regio(c);
		cpps_regtime(c);
		cpps_regarray(c);
		cpps_regstring(c);
		cpps_reggc(c);
		cpps_regdebug(c);
		cpps_reglock(c);
		return c;
	}
	int32 dostring(C* c, std::string str)
	{


		_CPPS_TRY
			Node* o = loadbuffer(c, c->_G, str, "");
		if (o) cpps_step_all(c, CPPS_SINGLERET, 0, o); //dostring ��pcall
		_CPPS_CATCH
			cpps_gc_check_step(c);
		return CPPS_NOERROR;
	}
	int32 loadfile(cpps::C* c, const char* path)
	{
#ifdef _WIN32
		FILE* file;
		fopen_s(&file, path, "rb+");
#else
		FILE* file = fopen(path, "rb+");
#endif
		std::string fileSrc;
		if (file)
		{
			char s[4097];
			memset(s, 0, 4097);
			fseek(file, 0, SEEK_END);
			int32 size = ftell(file);
			fseek(file, 0, SEEK_SET);
			while (size != 0)
			{
				int32 rs = size < 4096 ? size : 4096;
				fread(s, rs, 1, file);
				s[rs] = '\0';
				size -= rs;
				fileSrc += s;
			}
			fclose(file);
		}
		if (fileSrc.substr(0, 11) == "cpps_encode")
		{
			std::string decode;
			for (size_t i = 11; i < fileSrc.size(); i++)
			{
				decode.append(1, fileSrc[i] - CPPS_ENCODE_CPPS_KEY);
			}
			fileSrc = decode;
		}
		Node* o = loadbuffer(c, c->_G, fileSrc, path);
		if (o)
		{
			c->push(o);
			return CPPS_NOERROR;
		}
		return 1;
	}
	int32 dofile(cpps::C* c, const char* path)
	{
		return loadfile(c, path) || pcall(c, CPPS_SINGLERET, 0, 0);
	}
	void  cpps_destory_node(cpps::C* c, Node* d)
	{
		for (std::vector<Node*>::iterator it = d->l.begin(); it != d->l.end(); ++it)
		{
			Node* n = *it;
			if (n)
			{
				cpps_destory_node(c, n);
				delete n;
			}
		}
		d->l.clear();
	}
	int32 close(cpps::C* c)
	{
		cpps_destory_node(c, c->o); delete c->o; c->o = NULL;
		//�����ڴ�
		c->_G->destory(c);
		c->barrierList.clear();
		c->_callstack.clear();
		cpps_gc_check_gen1(c);
		delete c;
		return 0;
	}
	int32		pcall(C* c, int32 retType, cpps_domain* domain, Node* o)
	{
		_CPPS_TRY
			cpps_step_all(c, retType, domain, o);
		_CPPS_CATCH
			cpps_gc_check_step(c);
		return 0;
	}

	void  cpps_step_all(C* c, int32 retType, cpps_domain* domain, Node* o)
	{
		if (domain == NULL)
			domain = c->_G;
		if (o == NULL)
			o = c->o;

		domain->isbreak = false; //�п�������������˳�

		for (std::vector<Node*>::iterator it = o->l.begin(); it != o->l.end() && !domain->isbreak; ++it)
		{
			Node* d = *it;
			cpps_step(c, domain, d);
		}
	}
	void cpps_step_def_var(C* c, cpps_domain* domain, Node* d, int8 isconst = false)
	{
		//d->s //��������
		for (size_t i = 0; i < d->l.size(); ++i)
		{
			Node* varName = d->l[i];

			if (varName->type == CPPS_VARNAME)
			{
				Node* var = varName->l[0];
				if (var && (var->type == CPPS_ODEFVAR_VAR || var->type == CPPS_ODEFVAR_NIL))
				{
					cpps_regvar* v = new cpps_regvar();
					v->setVarName(varName->s);
					v->setIsConst(isconst);
					//c->gclock.lock();

					if (var->type == CPPS_ODEFVAR_VAR)
					{
						cpps_domain* leftdomain = NULL;
						cpps_value value = cpps_calculate_expression(c, domain, var->l[0], leftdomain);
						if (value.tt == CPPS_TSTRING)
						{
							cpps_cppsclassvar* cppsclassvar = (cpps_cppsclassvar*)value.value.domain;
							std::string* tmpStr = (std::string*)cppsclassvar->getclsptr();

							v->setValue(cpps_value(c, *tmpStr));
						}
						else
						{
							v->setValue(value);
						}
					}
					//�ͷ��ϵ�
					cpps_domain* leftdomain = NULL;
					cpps_regvar* old = domain->getVar(v->varName, leftdomain, false);
					if (old)
					{
						domain->unregVar(c, old);
						delete old;
					}
					//ע���µ�
					domain->regVar(c, v);

				}
				else if (var && var->type == CPPS_ODEFVAR_FUNC)
				{
					cpps_regfunction regfunc = cpps_regfunction(varName->s, new cpps_cppsfunction(var->domain, var->l[0], var->l[1]));
					cpps_domain* leftdomain = NULL;
					cpps_regvar* old = domain->getVar(varName->s, leftdomain, false);
					if (old)
					{
						domain->unregVar(c, old);
						delete old;
					}

					domain->regFunc(&regfunc);
				}
			}
		}
	}
	void cpps_step_throw(C* c, cpps_domain* domain, Node* d)
	{
		if (d->l.size() == 1)
		{
			cpps_domain* leftdomain = NULL;
			cpps_value ret_value = cpps_calculate_expression(c, domain, d->l[0], leftdomain);

			cpps_domain* cpps_func_domain = domain;

			while (cpps_func_domain && cpps_func_domain->parent[0]->domainType != cpps_domain_type_trycatch)
			{
				cpps_func_domain->isbreak = true;
				cpps_func_domain = cpps_func_domain->parent[1];
			}

			if (cpps_func_domain && cpps_func_domain->parent[0]->domainType == cpps_domain_type_trycatch)
			{
				//���û�ȥ
				cpps_func_domain->isbreak = true;
				cpps_func_domain->funcRet = ret_value;
			}
			else
			{
				throw(cpps_error(d->filename, d->line, cpps_error_trycatherror, "Illegal throw. please define in try."));
			}
		}
		else
		{
			//��ʱ��֧�ֶ������
		}
	}
	void cpps_step_return(C* c, cpps_domain* domain, Node* d)
	{
		if (d->l.size() == 1)
		{
			cpps_domain* leftdomain = NULL;
			cpps_value ret_value = cpps_calculate_expression(c, domain, d->l[0], leftdomain);

			cpps_domain* cpps_func_domain = domain;

			while (cpps_func_domain && cpps_func_domain != c->_G && cpps_func_domain->parent[0]->domainType != cpps_domain_type_func)
			{
				cpps_func_domain->isbreak = true;
				cpps_func_domain = cpps_func_domain->parent[1];
			}

			if (cpps_func_domain && cpps_func_domain != c->_G && cpps_func_domain->parent[0]->domainType == cpps_domain_type_func)
			{
				//���û�ȥ
				cpps_func_domain->isbreak = true;
				cpps_func_domain->funcRet = ret_value;

				cpps_regvar* v = new cpps_regvar();
				v->setVarName("return");
				v->setValue(ret_value);
				cpps_func_domain->regVar(c, v);
			}
			else
			{
				//ܳ��û�ҵ��������� ����ôreturn
				throw(cpps_error(d->filename, d->line, cpps_error_deffuncrror, "Illegal retrun. please define in function."));
			}
		}
		else
		{
			//��ʱ��֧�ֶ������
		}
	}
	bool cpps_step_can_break(C* c, cpps_domain* domain)
	{
		if (domain == c->_G)
			return false;

		if (domain->parent[0]->domainType == cpps_domain_type_while)
			return true;
		if (domain->parent[0]->domainType == cpps_domain_type_for)
			return true;
		if (domain->parent[0]->domainType == cpps_domain_type_foreach)
			return true;



		return false;
	}
	void cpps_step_break(C* c, cpps_domain* domain, Node* d)
	{
		cpps_domain* cpps_func_domain = domain;
		while (cpps_func_domain && !cpps_step_can_break(c, cpps_func_domain))
		{
			cpps_func_domain->isbreak = true;
			cpps_func_domain = cpps_func_domain->parent[1];
		}

		if (cpps_func_domain && cpps_step_can_break(c, cpps_func_domain))
		{
			//���û�ȥ
			cpps_func_domain->isbreak = true;
		}
		else
		{
			throw(cpps_error(d->filename, d->line, cpps_error_deffuncrror, "Illegal break. please define in while or for."));
		}
	}

	void cpps_step_continue(C* c, cpps_domain* domain, Node* d)
	{
		cpps_domain* cpps_func_domain = domain;

		while (cpps_func_domain && (cpps_func_domain->parent[0]->domainType != cpps_domain_type_while && cpps_func_domain->parent[0]->domainType != cpps_domain_type_for))
		{
			cpps_func_domain->isbreak = true;
			cpps_func_domain = cpps_func_domain->parent[1];
		}
	}
	void cpps_step_if(C* c, cpps_domain* domain, Node* d)
	{
		cpps_domain* leftdomain = NULL;

		cpps_value if_value = cpps_calculate_expression(c, domain, d->l[0]->l[0], leftdomain);
		int32 cpp_bool_value = 1;
		cpp_bool_value = if_value.value.b;
		if (if_value.tt == CPPS_TSTRING) cpp_bool_value = 1; //Ҫ��Ҫ�����أ�����
		else if (if_value.tt == CPPS_TNIL) cpp_bool_value = 0;
		else if (if_value.tt == CPPS_TCLASSVAR) cpp_bool_value = 1;

		cpps_domain* execdomain = new cpps_domain(d->domain, cpps_domain_type_exec, "");
		execdomain->setexecdomain(domain);
		if (cpp_bool_value) cpps_step_all(c, CPPS_SINGLERET, execdomain, d->l[1]);
		else cpps_step_all(c, CPPS_SINGLERET, execdomain, d->l[2]);

		execdomain->destory(c);
		delete execdomain;

		cpps_gc_check_step(c);
	}
	void cpps_step_for(C* c, cpps_domain* domain, Node* d)
	{
		Node* for1 = d->l[0];
		Node* for2 = d->l[1];
		Node* for3 = d->l[2];
		Node* for4 = d->l[3];

		domain->isbreak = false;

		cpps_step_all(c, CPPS_SINGLERET, domain, for1); //ִ���׾�
		while (true)
		{
			cpps_domain* leftdomain = NULL;

			cpps_value canwhile = for2->l.size() == 0 ? true : cpps_calculate_expression(c, domain, for2->l[0], leftdomain);
			bool b = cpps_converter<bool>::apply(canwhile);
			if (b == false)
				break;

			cpps_domain* execdomain = new cpps_domain(domain, cpps_domain_type_exec, "");
			execdomain->setexecdomain(domain);
			cpps_step_all(c, CPPS_SINGLERET, execdomain, for4);

			execdomain->destory(c);
			delete execdomain;

			cpps_gc_check_step(c);

			if (domain->isbreak) break; //��Ҫ����ѭ��

			cpps_step_all(c, CPPS_SINGLERET, domain, for3);
		}
	}
	void cpps_step_foreach(C* c, cpps_domain* domain, Node* d)
	{
		Node* for1 = d->l[0];
		Node* for2 = d->l[1];
		Node* for4 = d->l[2];

		domain->isbreak = false;


		cpps_domain* leftdomain = NULL;

		cpps_value v = cpps_calculate_expression(c, domain, for2->l[0], leftdomain);

		//cpps_regvar* v = domain->getVar(for2->s, leftdomain, true);

		cpps_regvar* for1_v = new cpps_regvar();
		for1_v->setVarName(for1->s);
		for1_v->setIsConst(true);
		domain->regVar(c, for1_v);


		if (v.tt == CPPS_ONIL) return;

		if (v.isDomain() && v.value.domain->domainName == "vector")
		{
			cpps_vector* vec = cpps_converter<cpps_vector*>::apply(v);
			if (vec)
			{
				std::vector<cpps_value>& realvector = vec->realvector();
				for (size_t i = 0; i < realvector.size(); i++)
				{
					for1_v->setValue(realvector[i]);

					cpps_domain* execdomain = new cpps_domain(domain, cpps_domain_type_exec, "");
					execdomain->setexecdomain(domain);
					cpps_step_all(c, CPPS_SINGLERET, execdomain, for4);

					execdomain->destory(c);
					delete execdomain;

					cpps_gc_check_step(c);

					if (domain->isbreak) break; //��Ҫ����ѭ��
				}
			}
		}
		else if (v.isDomain() && v.value.domain->domainName == "map")
		{
			cpps_map_node* mapnode;
			cpps_value ret = newClassPtr<cpps_map_node>(c, &mapnode);

			for1_v->setValue(ret);
			cpps_map* vmap = cpps_converter<cpps_map*>::apply(v);
			if (vmap)
			{
				std::map<cpps_value, cpps_value>& realmap = vmap->realmap();
				std::map<cpps_value, cpps_value>::iterator it = realmap.begin();
				std::map<cpps_value, cpps_value>::iterator itend = realmap.end();
				for (; it != itend; ++it)
				{
					mapnode->_first = it->first;
					mapnode->_second = it->second;

					cpps_domain* execdomain = new cpps_domain(domain, cpps_domain_type_exec, "");
					execdomain->setexecdomain(domain);
					cpps_step_all(c, CPPS_SINGLERET, execdomain, for4);

					execdomain->destory(c);
					delete execdomain;

					cpps_gc_check_step(c);

					if (domain->isbreak) break; //��Ҫ����ѭ��
				}
			}
		}
		else if (v.isDomain() && v.value.domain->domainName == "unordered_map")
		{
			cpps_map_node* mapnode;
			cpps_value ret = newClassPtr<cpps_map_node>(c, &mapnode);

			for1_v->setValue(ret);
			cpps_unordered_map* vmap = cpps_converter<cpps_unordered_map*>::apply(v);
			if (vmap)
			{
				std::unordered_map<cpps_value, cpps_value, cpps_value::hash>& realmap = vmap->realmap();
				std::unordered_map<cpps_value, cpps_value, cpps_value::hash>::iterator it = realmap.begin();
				std::unordered_map<cpps_value, cpps_value, cpps_value::hash>::iterator itend = realmap.end();
				for (; it != itend; ++it)
				{
					mapnode->_first = it->first;
					mapnode->_second = it->second;

					cpps_domain* execdomain = new cpps_domain(domain, cpps_domain_type_exec, "");
					execdomain->setexecdomain(domain);
					cpps_step_all(c, CPPS_SINGLERET, execdomain, for4);

					execdomain->destory(c);
					delete execdomain;

					cpps_gc_check_step(c);

					if (domain->isbreak) break; //��Ҫ����ѭ��
				}
			}
		}
		else
		{

			throw(cpps_error(d->filename, d->line, cpps_error_forerror, "Foreach only supports map, unordered_ Map, vector. Other formats are not supported at the moment."));
		}

	}
	void cpps_step_while(C* c, cpps_domain* domain, Node* d)
	{
		Node* while1 = d->l[0];
		Node* while2 = d->l[1];

		while (true)
		{
			cpps_domain* leftdomain = NULL;

			cpps_value canwhile = cpps_calculate_expression(c, domain, while1->l[0], leftdomain);
			bool b = cpps_converter<bool>::apply(canwhile);
			if (b == false)
				break;


			cpps_domain* execdomain = new cpps_domain(domain, cpps_domain_type_exec, "");
			execdomain->setexecdomain(domain);

			cpps_step_all(c, CPPS_SINGLERET, execdomain, while2);

			execdomain->destory(c);
			delete execdomain;

			cpps_gc_check_step(c);

			if (domain->isbreak) break; //��Ҫ����ѭ��
		}
	}
	void cpps_step_namespace(C* c, cpps_domain* domain, Node* d)
	{
		cpps_domain* leftdomain;
		cpps_regvar* v = domain->getVar(d->s, leftdomain, false);
		cpps_domain* ns = NULL;
		if (!v || (v->getValue().isDomain() && v->getValue().value.domain->domainType != cpps_domain_type_namespace))
		{
			ns = new cpps_domain(domain, cpps_domain_type_namespace, d->s);

			v = new cpps_regvar();
			v->setVarName(d->s);
			v->setValue(cpps_value(ns));
			domain->regVar(c, v);

		}
		else
		{
			ns = v->getValue().value.domain;
		}


		cpps_step_all(c, CPPS_SINGLERET, ns, d);

	}
	void cpps_pop_stack_to_here(C* c, cpps_stack* here) {
		while (!c->getcallstack()->empty()) {
			cpps_stack* takestack2 = c->getcallstack()->at(c->getcallstack()->size() - 1);
			if (here != takestack2) {
				delete  takestack2;
				c->getcallstack()->pop_back();
			}
			else break;
		}
	}
	void cpps_do_catch(C* c, cpps_domain* parent_domain, cpps_domain* root_domain, Node* catchfun, cpps_trycatch_error& e)
	{
		cpps_domain* execdomain2 = new cpps_domain(parent_domain, cpps_domain_type_exec, "");
		execdomain2->setexecdomain(root_domain);

		Node* varName = catchfun->l[0];
		cpps_regvar* v = new cpps_regvar();
		v->setVarName(varName->s);
		v->setValue(cpps_cpp_to_cpps_converter<cpps_trycatch_error*>::apply(c, &e));
		execdomain2->regVar(c, v);

		cpps_step_all(c, CPPS_SINGLERET, execdomain2, catchfun->l[1]);

		execdomain2->destory(c);
		delete execdomain2;

		cpps_gc_check_step(c);


	}
	void cpps_step_trycath(C* c, cpps_domain* domain, Node* d)
	{
		Node* func = d->l[0];
		Node* catchfun = d->l[1];

		cpps_domain* execdomain = new cpps_domain(d->domain, cpps_domain_type_exec, "");
		execdomain->setexecdomain(domain);

		cpps_stack* takestack = c->getcallstack()->empty() ? NULL : c->getcallstack()->at(c->getcallstack()->size() - 1);
		bool hasCatch = false;
		cpps_trycatch_error throwerr;
		try {
			cpps_step_all(c, CPPS_SINGLERET, execdomain, func);
			if (execdomain->funcRet.tt != CPPS_TNIL)
			{
				throwerr = cpps_trycatch_error(d->filename, d->line, cpps_error_trycatherror, "The exception thrown by throw.");
				throwerr.value = execdomain->funcRet;
				hasCatch = true;
			}
		}
		catch (cpps_error e)
		{
			cpps_pop_stack_to_here(c, takestack); //��ջ
			throwerr = cpps_trycatch_error(e);
			hasCatch = true;
		}
		catch (const char* s)
		{
			cpps_pop_stack_to_here(c, takestack);//��ջ
			throwerr = cpps_trycatch_error(d->filename, d->line, cpps_error_trycatherror, "Unknown exception thrown by throw.");
			throwerr.value = s;
			hasCatch = true;
		}

		if (hasCatch) cpps_do_catch(c, d->domain, domain, catchfun, throwerr);

		execdomain->destory(c);
		delete execdomain;

	}

	void cpps_step_class(C* c, cpps_domain* domain, Node* d)
	{
		cpps_cppsclass* cppsclass = new cpps_cppsclass(d->s, d, domain, cpps_domain_type_class);

		cpps_regvar* v = new cpps_regvar();
		v->setVarName(d->s);
		v->setIsConst(true);
		v->setValue(cpps_value(cppsclass));
		domain->regVar(NULL, v);

		Node* parent = d->getleft();
		Node* vars = d->getright();

		for (std::vector<Node*>::iterator it = parent->l.begin(); it != parent->l.end(); ++it)
		{
			Node* o = *it;
			cpps_domain* leftdomain = NULL;
			cpps_regvar* regvar = domain->getVar(o->s, leftdomain);

			Node* lastNamespace = o;
			while (lastNamespace && lastNamespace->getleft() && lastNamespace->getleft()->type == CPPS_ONAMESPANCE_CHILD)
			{
				regvar = regvar->getValue().value.domain->getVar(lastNamespace->getleft()->s, leftdomain, false);
				lastNamespace = lastNamespace->getleft();
			}


			if (regvar->getValue().tt != CPPS_TCLASS)
			{
				throw(cpps_trycatch_error(d->filename, d->line, cpps_error_trycatherror, "The parent class must be a class. Cannot be another type."));
			}
			cpps_cppsclass* parentclass = (cpps_cppsclass*)regvar->getValue().value.domain;
			cppsclass->parentClassList().push_back(parentclass); //���Ӹ���
			//���Ƹ������еĺ���Ӵ��
			for (std::unordered_map<std::string, cpps_regvar*>::iterator it2 = parentclass->varList.begin(); it2 != parentclass->varList.end(); ++it2)
			{
				if (it2->first != "constructor") //�����ƹ��캯������
					cppsclass->varList.insert(std::unordered_map<std::string, cpps_regvar*>::value_type(it2->first, it2->second));

			}

			if (parentclass->o != NULL)
			{
				//����������ı���
				for (std::vector<Node*>::iterator it3 = parentclass->o->getright()->l.begin(); it3 != parentclass->o->getright()->l.end(); ++it3)
				{
					(*it3)->setParent(cppsclass->o->getright());
					cppsclass->o->getright()->l.push_back(*it3);
				}
			}
		}

		//ֻע�ắ����������  ����ע�ᵽ ������
		for (std::vector<Node*>::iterator it = vars->l.begin(); it != vars->l.end();)
		{
			Node* o = *it;
			if (o->type == CPPS_ODEFVAR || o->type == CPPS_ODEFCONSTVAR)
			{
				Node* varName = o->l[0];
				if (varName->type == CPPS_VARNAME)
				{
					Node* var = varName->l[0];
					if (var && var->type == CPPS_ODEFVAR_FUNC)
					{
						cpps_domain* funcdomain = new cpps_domain(cppsclass, cpps_domain_type_func, d->s + "::" + var->s);
						cpps_regfunction regfunc = cpps_regfunction(varName->s, new cpps_cppsfunction(funcdomain, var->l[0], var->l[1]));
						cppsclass->regFunc(&regfunc);
						it = vars->l.erase(it);
					}
					else
					{
						++it;
					}
				}
			}
			else
			{
				it = vars->l.erase(it);
			}
		}
	}
	void cpps_step_import(C* c, cpps_domain* domain, Node* o)
	{
		cpps_domain* leftdomain = NULL;
		if (o->l.empty()) return;

		cpps_value list = cpps_calculate_expression(c, domain, o->l[0], leftdomain);
		cpps_cppsclass* cppsclass = (cpps_cppsclass*)list.value.domain->parent[0];
		cpps_cppsclassvar* cppsclassvar = (cpps_cppsclassvar*)list.value.domain;
		if (list.tt == CPPS_TSTRING)
		{
			cpps_loadlibrary(c, cpps_to_string(list));
		}
		else if(cppsclass->getClassName() == "vector")
		{
			cpps_vector* vec = (cpps_vector*)cppsclassvar->getclsptr();
			for(auto &v : vec->realvector())
			{
				if (v.tt == CPPS_TSTRING)
				{
					cpps_loadlibrary(c, cpps_to_string(v));
				}
			}
		}
	}
	void cpps_step_dofile(C* c, cpps_domain* domain, Node* o)
	{
		_CPPS_TRY
			for (std::vector<Node*>::iterator it = o->l.begin(); it != o->l.end(); ++it)
			{
				cpps_domain* leftdomain = NULL;

				cpps_value path = cpps_calculate_expression(c, domain, *it, leftdomain);
				std::string fpath = cpps_to_string(path);

#ifdef _WIN32
				FILE* file;
				fopen_s(&file, fpath.c_str(), "rb+");
#else
				FILE* file = fopen(fpath.c_str(), "rb+");
#endif
				std::string fileSrc;
				if (file)
				{
					char s[4097];
					memset(s, 0, 4097);
					fseek(file, 0, SEEK_END);
					int32 size = ftell(file);
					fseek(file, 0, SEEK_SET);
					while (size != 0)
					{
						int32 rs = size < 4096 ? size : 4096;
						fread(s, rs, 1, file);
						s[rs] = '\0';
						size -= rs;
						fileSrc += s;
					}
					fclose(file);
				}
				if (fileSrc.substr(0, 11) == "cpps_encode")
				{
					std::string decode;
					for (size_t i = 11; i < fileSrc.size(); i++)
					{
						decode.append(1, fileSrc[i] - CPPS_ENCODE_CPPS_KEY);
					}
					fileSrc = decode;
				}
				Node* o = loadbuffer(c, domain, fileSrc, fpath);
				cpps_stack* stack = new cpps_stack((*it)->filename, (*it)->line, "dofile");

				c->push_stack(stack);
				cpps_step_all(c, CPPS_SINGLERET, domain, o);
				c->pop_stack();


				cpps_gc_check_step(c);

				delete stack;
			}
		_CPPS_CATCH
	}
	void cpps_step_dostring(C* c, cpps_domain* domain, Node* d)
	{

		_CPPS_TRY
			cpps_domain* leftdomain = NULL;

		cpps_value path = cpps_calculate_expression(c, domain, d->getleft(), leftdomain);
		std::string str = cpps_to_string(path);

		Node* o = loadbuffer(c, domain, str, "");
		cpps_stack* stack = new cpps_stack(d->filename, d->line, "dostring");
		c->push_stack(stack);
		cpps_step_all(c, CPPS_SINGLERET, domain, o);
		c->pop_stack();
		delete stack;

		cpps_gc_check_step(c);

		_CPPS_CATCH
	}
	void cpps_step(C* c, cpps_domain* domain, Node* d)
	{
		if (d->type == CPPS_ODEFVAR)
		{
			cpps_step_def_var(c, domain, d);
		}
		else if (d->type == CPPS_ODEFCONSTVAR)
		{
			cpps_step_def_var(c, domain, d, true);
		}
		else if (d->type == CPPS_OASSEMBLE)
		{
			cpps_domain* execdomain = new cpps_domain(d->domain, cpps_domain_type_exec, "");
			execdomain->setexecdomain(domain);

			cpps_step_all(c, CPPS_SINGLERET, execdomain, d);

			execdomain->destory(c);
			delete execdomain;

			cpps_gc_check_step(c);
		}
		else if (d->type == CPPS_OEXPRESSION)
		{
			cpps_domain* leftdomain = NULL;

			cpps_calculate_expression(c, domain, d->l[0], leftdomain);
		}
		else if (d->type == CPPS_ORETURN)
		{
			cpps_step_return(c, domain, d);
		}
		else if (d->type == CPPS_OTHROW)
		{
			cpps_step_throw(c, domain, d);
		}
		else if (d->type == CPPS_OBREAK)
		{
			cpps_step_break(c, domain, d);
		}
		else if (d->type == CPPS_OCONTINUE)
		{
			cpps_step_continue(c, domain, d);
		}
		else if (d->type == CPPS_OIF)
		{
			cpps_step_if(c, domain, d);
		}
		else if (d->type == CPPS_OFOR)
		{
			cpps_domain* execdomain = new cpps_domain(d->domain, cpps_domain_type_exec, "");
			execdomain->setexecdomain(domain);

			cpps_step_for(c, execdomain, d);

			execdomain->destory(c);
			delete execdomain;

			cpps_gc_check_step(c);
		}
		else if (d->type == CPPS_OFOREACH)
		{
			cpps_domain* execdomain = new cpps_domain(d->domain, cpps_domain_type_exec, "");
			execdomain->setexecdomain(domain);

			cpps_step_foreach(c, execdomain, d);

			execdomain->destory(c);
			delete execdomain;

			cpps_gc_check_step(c);
		}
		else if (d->type == CPPS_OWHILE)
		{
			cpps_domain* execdomain = new cpps_domain(d->domain, cpps_domain_type_exec, "");
			execdomain->setexecdomain(domain);

			cpps_step_while(c, execdomain, d);

			delete execdomain;

			cpps_gc_check_step(c);
		}
		else if (d->type == CPPS_OCLASS)
		{
			cpps_step_class(c, domain, d);
		}
		else if (d->type == CPPS_ONAMESPACE)
		{
			cpps_step_namespace(c, domain, d);
		}
		else if (d->type == CPPS_OTRYCATCH)
		{
			cpps_step_trycath(c, domain, d);
		}
		else if (d->type == CPPS_ODOFILE)
		{
			cpps_step_dofile(c, NULL, d);
		}
		else if (d->type == CPPS_OINCLUDE)
		{
			cpps_step_dofile(c, domain, d);
		}
		else if (d->type == CPPS_OIMPORT)
		{
			cpps_step_import(c, domain, d);
		}
		else if (d->type == CPPS_ODOSTRING)
		{
			cpps_step_dostring(c, domain, d);
		}
	}
	int32 cpps_str2d(const char* s, cpps_number* result) {
		char* endptr;
		*result = strtod(s, &endptr);
		if (endptr == s) return 0;  /* conversion failed */
		if (*endptr == 'x' || *endptr == 'X')  /* maybe an hexadecimal constant? */
			*result = cpps_number(strtoul(s, &endptr, 16));
		if (*endptr == '\0') return 1;  /* most common case */
		while (cpps_parse_isspace(usint8(*endptr))) endptr++;
		if (*endptr != '\0') return 0;  /* invalid trailing characters? */
		return 1;
	}

	int32 cpps_str2i64(const char* s, cpps_integer* result) {
		cpps_number n = 0;
		cpps_str2d(s, &n);
		*result = cpps_number2integer(n);
		return 1;
	}
	cpps_regvar* getregvar(cpps_domain* domain, Node* o)
	{
		cpps_domain* leftdomain = NULL;
		cpps_regvar* v = domain->getVar(o->s, leftdomain);
		if (v)
		{
			if ((v->getValue().tt == CPPS_TDOMAIN || v->getValue().tt == CPPS_TCLASSVAR) && !o->l.empty())
			{
				return getregvar(v->getValue().value.domain, o->l[0]);
			}
		}
		return v;
	}

	void make_values(C* c, cpps_domain* domain, Node* d, std::vector<cpps_value>& params, std::vector<cpps_regvar*>& varlist, cpps_domain* execdomain)
	{


		int index = 0;
		for (std::vector<Node*>::iterator it = d->l.begin(); it != d->l.end(); ++it)
		{
			index++;
			cpps_domain* leftdomain = NULL;
			cpps_value value = cpps_calculate_expression(c, domain, *it, leftdomain);
			params.push_back(value);

			cpps_regvar* v = new cpps_regvar;
			v->setValue(value);
			std::string strStream;
			strStream = "p";
			strStream.push_back(65 + index);
			v->setVarName(strStream);
			execdomain->regVar(c, v);
		}

	}
	cpps_regvar* cpps_node_to_regver(cpps_domain* domain, Node* d, bool isgetRight = true)
	{
		if (d == NULL) return NULL; /*d is null when it is a constructor*/

		cpps_regvar* v = NULL;


		if (d->type == CPPS_QUOTEGETCHIILD)
		{
			v = cpps_node_to_regver(domain, d->getleft());
			if (v && ((v->getValue().tt == CPPS_TDOMAIN || v->getValue().tt == CPPS_TCLASSVAR)) && isgetRight)
			{
				if (v->getValue().tt == CPPS_TCLASSVAR && v->getValue().value.domain->getDomainName() != "map" && v->getValue().value.domain->getDomainName() != "unordered_map" && v->getValue().value.domain->getDomainName() != "vector")
				{
					v = getregvar(v->getValue().value.domain, d->getright());
				}
			}
		}
		else if (d->type == CPPS_QUOTEGETOBJECT)
		{
			v = cpps_node_to_regver(domain, d->getleft());
			if (v && ((v->getValue().tt == CPPS_TDOMAIN || v->getValue().tt == CPPS_TCLASSVAR)) && isgetRight)
			{
				if (v->getValue().tt == CPPS_TCLASSVAR && v->getValue().value.domain->getDomainName() != "map" && v->getValue().value.domain->getDomainName() != "unordered_map" && v->getValue().value.domain->getDomainName() != "vector")
				{
					v = getregvar(v->getValue().value.domain, d->getright());
				}
			}
		}
		else if (d->type == CPPS_OGETOBJECT)
		{
			v = cpps_node_to_regver(domain, d->getleft());
			if (v && ((v->getValue().tt == CPPS_TDOMAIN || v->getValue().tt == CPPS_TCLASSVAR)) && isgetRight)
			{
				if ((v->getValue().tt == CPPS_TDOMAIN || v->getValue().tt == CPPS_TCLASSVAR) && v->getValue().value.domain->getDomainName() != "map" && v->getValue().value.domain->getDomainName() != "unordered_map" && v->getValue().value.domain->getDomainName() != "vector")
				{
					v = getregvar(v->getValue().value.domain, d->getright());
				}
			}
		}
		else if (d->type == CPPS_OGETCHIILD)
		{
			v = cpps_node_to_regver(domain, d->getleft());
			if (v && ((v->getValue().tt == CPPS_TDOMAIN || v->getValue().tt == CPPS_TCLASSVAR)) && isgetRight)
			{
				if (v->getValue().tt == CPPS_TCLASSVAR && v->getValue().value.domain->getDomainName() != "map" && v->getValue().value.domain->getDomainName() != "unordered_map" && v->getValue().value.domain->getDomainName() != "vector")
				{
					v = getregvar(v->getValue().value.domain, d->getright());
				}
			}
		}
		else
		{
			cpps_domain* leftdomain = NULL;
			v = domain->getVar(d->s, leftdomain);
		}

		return v;
	}
	bool cpps_check_parent_class(cpps_cppsclass* a, cpps_cppsclass* b)
	{
		if (a == NULL) return false;
		if (b == NULL) return false;
		for (size_t i = 0; i < a->parentClassList().size(); i++)
		{
			cpps_cppsclass* c = a->parentClassList()[i];
			if (c == b)
				return true;
		}
		for (size_t i = 0; i < a->parentClassList().size(); i++)
		{
			cpps_cppsclass* c = a->parentClassList()[i];
			bool ret = cpps_check_parent_class(c, b);
			if (ret) return true;
		}
		return false;
	}
	void cpps_call_parent_class_default_constructor(C* c, Node* n, cpps_cppsclass* parent_cppsclass, cpps_domain* domain, cpps_domain* leftdomain)
	{
		cpps_domain* tmpdomain = NULL;
		cpps_regvar* var = parent_cppsclass->getVar("constructor", tmpdomain, false);

		if (var && var->getValue().tt == CPPS_TFUNCTION) {


			//��ִ�и���ĸ���Ĺ��캯��
			for (size_t i = 0; i < parent_cppsclass->parentClassList().size(); i++)
			{
				cpps_cppsclass* tmp_parent_cppsclass = parent_cppsclass->parentClassList()[i];
				cpps_call_parent_class_default_constructor(c, n, tmp_parent_cppsclass, domain, leftdomain);
			}

			cpps_function* f = var->getValue().value.func;
			if (f->getparamcount() == 0)
			{
				cpps_domain* execdomain = new cpps_domain(domain, cpps_domain_type_func, "constructor");
				execdomain->setexecdomain(domain);

				Node tmp(n->filename, n->line);
				Node params(n->filename, n->line);
				tmp.addtoright(&params); //
				cpps_step_callfunction(c, execdomain, var->getValue(), &tmp, leftdomain);

				execdomain->destory(c);
				delete execdomain;
			}
		}
	}
	cpps_value cpps_calculate_expression(C* c, cpps_domain* domain, Node* d, cpps_domain*& leftdomain)
	{
		cpps_value ret;
		if (d->type == CPPS_ODOFUNCTION)
		{
			cpps_value var = cpps_calculate_expression(c, domain, d->getleft(), leftdomain);


			cpps_domain* execdomain = new cpps_domain(domain, cpps_domain_type_func, d->getleft()->s.c_str());
			execdomain->setexecdomain(domain);

			//cpps_step_for(c, execdomain, d);
			ret = cpps_step_callfunction(c, execdomain, var, d, leftdomain);

			execdomain->destory(c);
			delete execdomain;
		}
		else if (d->type == CPPS_OSTR)
		{
			ret.tt = CPPS_TSTRING;

			std::string* str;
			cpps_value ret2 = newClassPtr<std::string>(c, &str);
			for (size_t i = 0; i < d->l.size(); i++)
			{
				Node* s = d->l[i];
				if (s->type == CPPS_OSTR)
				{
					str->append(s->s);
				}
				else
				{
					cpps_value v = cpps_calculate_expression(c, domain, s->l[0], leftdomain);
					str->append(cpps_to_string(v));
				}
			}

			ret.value.domain = ret2.value.domain;
		}
		else if (d->type == CPPS_ONUMBER)
		{
			ret.tt = CPPS_TNUMBER;
			//str2d
			cpps_str2d(d->s.c_str(), &ret.value.number);
		}
		else if (d->type == CPPS_OINTEGER)
		{
			ret.tt = CPPS_TINTEGER;
			//str2i64
			cpps_str2i64(d->s.c_str(), &ret.value.integer);
		}
		else if (d->type == CPPS_OINTEGER16)
		{
			ret.tt = CPPS_TINTEGER;
			//str2i64
			cpps_str2i64(d->s.c_str(), &ret.value.integer);
		}
		else if (d->type == CPPS_ONEWVAR)
		{
			ret.tt = CPPS_TCLASSVAR;
			cpps_regvar* v = domain->getVar(d->s, leftdomain);

			//������û��ʹ�����ռ�
			Node* lastNamespace = d;
			while (lastNamespace && lastNamespace->getleft() && lastNamespace->getleft()->type == CPPS_ONAMESPANCE_CHILD)
			{
				v = v->getValue().value.domain->getVar(lastNamespace->getleft()->s, leftdomain);
				lastNamespace = lastNamespace->getleft();
			}

			if (v && v->getValue().tt == CPPS_TCLASS)
			{
				cpps_cppsclass* cppsclass = (cpps_cppsclass*)v->getValue().value.domain;
				cpps_cppsclassvar* cppsclassvar = cppsclass->create(c);


				//�����������ı�����������
				if (cppsclass->o)
					cpps_step_all(c, CPPS_SINGLERET, cppsclassvar, cppsclass->o->getright());

				//���´������������ӵ��������Ժ���Ҫ��Ҫ�ɵ�
				cpps_gc_add_gen0(c, cppsclassvar);

				ret = cpps_value(cppsclassvar);

				cpps_regvar* v = new cpps_regvar();//_G Ϊ���ڵ�
				v->setVarName("this");
				v->setValue(ret); //���б���copy��ȥ
				v->setIsConst(true);
				cppsclassvar->regVar(NULL, v);


				cpps_domain* takedomain2 = leftdomain;
				leftdomain = cppsclassvar;

				//�������⴦����
				if (d->s == "vector" && d->getleft())
				{
					cpps_vector* array = static_cast<cpps_vector*>(cppsclassvar->getclsptr());
					cpps_integer result;
					cpps_str2i64(d->getleft()->s.c_str(), &result);
					array->resize(result);
				}
				else if (d->getright()) //���캯��
				{
					if (d->l.size() == 3)
					{
						Node* n = d->l[2];

						if (n && n->type == CPPS_ONEW_SETV)
						{
							Node* rr = n;
							for (size_t i = 0; i < rr->l.size(); i++)
							{
								Node* k = rr->l[i]->getleft();
								cpps_domain* takedomain = leftdomain;
								leftdomain = NULL;
								cpps_value v = cpps_calculate_expression(c, domain, rr->l[i]->getright()->getleft(), leftdomain);
								//m->insert(cpps_value(c, k->s), v);
								cpps_regvar* var = cppsclassvar->getVar(k->s, leftdomain);
								if (var)
									var->setValue(v);
								leftdomain = takedomain;
							}
						}
					}

					Node* n = d->getright();
					if (n && n->type == CPPS_OCLASS_CONSTRUCTOR)
					{
						//ִ�и����Ĭ�Ϲ��캯�����в����Ͳ�ִ�У�
						for (size_t i = 0; i < cppsclass->parentClassList().size(); ++i)
						{
							cpps_cppsclass* parent_cppsclass = cppsclass->parentClassList()[i];
							cpps_call_parent_class_default_constructor(c, n, parent_cppsclass, domain, leftdomain);
						}

						cpps_regvar* var = cppsclassvar->getVar("constructor", leftdomain);
						if (var && var->getValue().tt == CPPS_TFUNCTION) {

							cpps_domain* execdomain = new cpps_domain(domain, cpps_domain_type_func, "constructor");
							execdomain->setexecdomain(domain);

							cpps_step_callfunction(c, execdomain, var->getValue(), d, leftdomain);

							execdomain->destory(c);
							delete execdomain;
						}
					}

				}
				leftdomain = takedomain2;
			}
			else
			{
				throw(cpps_error(d->filename, d->line, cpps_error_normalerror, "The object out of new [%s] must be a class object", d->s.c_str()));
			}
		}
		else if (d->type == CPPS_OBRACKET)
		{
			cpps_domain* leftdomain = NULL;

			if (d->getleft()) //��ȡ��һ���ڵ�
				ret = cpps_calculate_expression(c, domain, d->getleft(), leftdomain);
		}
		else if (d->type == CPPS_OARRAY)
		{


			cpps_vector* vec = NULL;
			ret = newClassPtr<cpps_vector>(c, &vec);


			cpps_regvar v;
			v.setVarName("ret");
			v.setValue(ret);
			cpps_gc_add_barrier(c, &v);

			for (size_t i = 0; i < d->l.size(); i++)
			{
				cpps_domain* takedomain = leftdomain;
				leftdomain = NULL;
				cpps_value v = cpps_calculate_expression(c, domain, d->l[i]->getleft(), leftdomain);
				vec->push_back(v);
				leftdomain = takedomain;
			}

			cpps_gc_remove_barrier(c, &v);
		}
		else if (d->type == CPPS_OOBJECT)
		{
			cpps_map* m = NULL;
			ret = newClassPtr<cpps_map>(c, &m);

			cpps_regvar v;
			v.setVarName("ret");
			v.setValue(ret);
			cpps_gc_add_barrier(c, &v);


			for (size_t i = 0; i < d->l.size(); i++)
			{
				Node* k = d->l[i]->getleft();
				cpps_domain* takedomain = leftdomain;
				leftdomain = NULL;
				cpps_value v = cpps_calculate_expression(c, domain, d->l[i]->getright()->getleft(), leftdomain);
				m->insert(cpps_value(c, k->s), v);
				leftdomain = takedomain;
			}

			cpps_gc_remove_barrier(c, &v);

		}
		else if (d->type == CPPS_OBOOL)
		{
			ret.tt = CPPS_TBOOLEAN;

			ret.value.b = (d->s.at(0) == 't'); //����ĸΪt ��ֱ����Ϊ���� true
		}
		else if (d->type == CPPS_VARNAME)
		{
			cpps_regvar* v = (leftdomain ? leftdomain : domain)->getVar(d->s, leftdomain);
			if (v)
			{
				ret = v->getValue();
			}
			else
			{
				printf("Warning:  got a not existent variable of [%s].  line: %d file: %s\n", d->s.c_str(), d->line, d->filename.c_str());
			}
		}
		else if (d->type == CPPS_VARNAME_LAMBDA)
		{
			cpps_regvar* v = domain->getVar(d->s, leftdomain);
			if (v)
			{
				//printf(domain->domainName.c_str());
				ret = v->getValue();
				ret.parentLambdaVar = domain;
			}
		}
		else if (d->type == CPPS_FUNCNAME)
		{
			cpps_regvar* v = domain->getVar(d->s, leftdomain);
			if (v && v->getValue().tt == CPPS_TFUNCTION)
			{
				if (d->s == "&&")
				{
					if (d->l.size() == 2)//������2�������Ŷ�
					{
						typedef cpps_converter<bool> converter;
						ret.tt = CPPS_TBOOLEAN;
						cpps_domain* leftdomain = NULL;
						ret.value.b = converter::apply(cpps_calculate_expression(c, domain, d->l[0], leftdomain)) && converter::apply(cpps_calculate_expression(c, domain, d->l[1], leftdomain));
					}
				}
				else if (d->s == "||")
				{
					if (d->l.size() == 2)//������2�������Ŷ�
					{
						typedef cpps_converter<bool> converter;
						ret.tt = CPPS_TBOOLEAN;
						cpps_domain* leftdomain = NULL;
						ret.value.b = converter::apply(cpps_calculate_expression(c, domain, d->l[0], leftdomain)) || converter::apply(cpps_calculate_expression(c, domain, d->l[1], leftdomain));
					}
				}
				else
				{
					cpps_function* f = v->getValue().value.func;
					cpps_stack* stack = new cpps_stack(d->filename, d->line, d->s);
					c->push_stack(stack);
					std::vector<cpps_value> params;
					std::vector<cpps_regvar*> params_var;
					cpps_domain* execdomain = new cpps_domain(domain, cpps_domain_type_func, "");
					execdomain->setexecdomain(domain);
					cpps_value isNeedC;
					if (f->getIsNeedC())
					{
						isNeedC = cpps_cpp_to_cpps_converter<C*>::apply(c, c);
						params.push_back(isNeedC);
						cpps_regvar* v = new cpps_regvar;
						v->setValue(isNeedC);
						std::string pc = "pc";
						v->setVarName(pc);
						execdomain->regVar(c, v);
					}
					make_values(c, domain, d, params, params_var, execdomain);
					f->callfunction(c, &ret, domain, &params);
					execdomain->destory(c);
					delete execdomain;
					c->pop_stack();
					delete stack;
				}
			}
			else
			{
				//��Ҫһ�����������������ǣ�������
				printf("cpps warring: [%s] is not function or parent is nil... line:%d , file:%s \n", d->s.c_str(), d->line, d->filename.c_str());
			}
		}
		else if (d->type == CPPS_OGETCHIILD)
		{
			cpps_value left = cpps_calculate_expression(c, domain, d->getleft(), leftdomain);
			if (left.tt != CPPS_TNIL)
			{
				if (left.tt == CPPS_TCLASSVAR)
				{
					cpps_cppsclass* cppsclass = (cpps_cppsclass*)left.value.domain->parent[0];
					if (d->getright()->type != CPPS_FUNCNAME && cppsclass->getClassName() == "vector")
					{
						cpps_cppsclassvar* cppsclassvar = (cpps_cppsclassvar*)left.value.domain;
						cpps_vector* pVec = (cpps_vector*)cppsclassvar->getclsptr();
						cpps_domain* takedomain = leftdomain;
						leftdomain = NULL;
						cpps_value right = cpps_calculate_expression(c, domain, d->getright()->getleft(), leftdomain);
						if (right.tt != CPPS_TINTEGER)
						{
							throw(cpps_error(d->getright()->filename, d->getright()->line, cpps_error_classerror, "Array must contain a number as an index.��"));
						}
						leftdomain = takedomain;
						if (pVec->size() <= right.value.integer)
							throw(cpps_error(d->getright()->filename, d->getright()->line, cpps_error_classerror, "Array has crossed the current length: [%d]. You need to get the length: [%d].��", pVec->size(), right.value.integer));

						ret = pVec->at(right.value.integer);
					}
					else if (d->getright()->type != CPPS_FUNCNAME && cppsclass->getClassName() == "map")
					{
						cpps_cppsclassvar* cppsclassvar = (cpps_cppsclassvar*)left.value.domain;
						cpps_map* pMap = (cpps_map*)cppsclassvar->getclsptr();
						cpps_domain* takedomain = leftdomain;
						leftdomain = NULL;
						cpps_value right = cpps_calculate_expression(c, domain, d->getright()->getleft(), leftdomain);
						leftdomain = takedomain;
						ret = pMap->find(right);
					}
					else if (d->getright()->type != CPPS_FUNCNAME && cppsclass->getClassName() == "unordered_map")
					{
						cpps_cppsclassvar* cppsclassvar = (cpps_cppsclassvar*)left.value.domain;
						cpps_unordered_map* pMap = (cpps_unordered_map*)cppsclassvar->getclsptr();
						cpps_domain* takedomain = leftdomain;
						leftdomain = NULL;
						cpps_value right = cpps_calculate_expression(c, domain, d->getright()->getleft(), leftdomain);
						leftdomain = takedomain;
						ret = pMap->find(right);
					}
					else
					{
						cpps_domain* execdomain = new cpps_domain(left.value.domain, cpps_domain_type_exec, "");
						leftdomain = left.value.domain;
						ret = cpps_calculate_expression(c, execdomain, d->getright()->getleft(), leftdomain);
						execdomain->destory(c);
						delete execdomain;
					}

				}
				else if (left.tt == CPPS_TDOMAIN)
				{
					leftdomain = left.value.domain;

					ret = cpps_calculate_expression(c, left.value.domain, d->getright(), leftdomain);
				}
			}
			else
			{
				throw(cpps_error(d->filename, d->getleft()->line, cpps_error_classerror, "[%s] must be a class object or a domain before the '.'", d->getleft()->s.c_str()));
			}
		}
		else if (d->type == CPPS_OGETOBJECT)
		{
			cpps_value left = cpps_calculate_expression(c, domain, d->getleft(), leftdomain);
			if (left.tt != CPPS_TNIL)
			{
				if (left.tt == CPPS_TCLASSVAR)
				{
					cpps_cppsclass* cppsclass = (cpps_cppsclass*)left.value.domain->parent[0];
					cpps_domain* execdomain = new cpps_domain(c->_G, cpps_domain_type_exec, "");
					//cpps_domain *takedomain = leftdomain;
					leftdomain = left.value.domain;
					execdomain->setexecdomain(left.value.domain);
					ret = cpps_calculate_expression(c, execdomain, d->getright(), leftdomain);
					//leftdomain = takedomain;
					execdomain->destory(c);
					delete execdomain;

				}
				else if (left.tt == CPPS_TCLASS && leftdomain && leftdomain->domainType == cpps_domain_type_classvar)
				{
					cpps_cppsclass* cppsclass = (cpps_cppsclass*)left.value.domain;
					cpps_domain* takedomain = NULL;

					//check this class is parent class..
					bool is_parent_class = cpps_check_parent_class((cpps_cppsclass*)leftdomain->parent[0], cppsclass);
					if (!is_parent_class) throw(cpps_error(d->filename, d->getleft()->line, cpps_error_classerror, "Non parent function executed: [%s::%s].", d->getleft()->s.c_str(), d->getright()->s.c_str()));

					cpps_regvar* var = cppsclass->getVar(d->getright()->s, takedomain, false);
					if (var && var->getValue().tt == CPPS_TFUNCTION)
					{
						ret = var->getValue();
					}
				}
				else if (left.tt == CPPS_TDOMAIN)
				{
					cpps_domain* takedomain = leftdomain;
					leftdomain = left.value.domain;
					ret = cpps_calculate_expression(c, left.value.domain, d->getright(), leftdomain);
					//if (left.value.domain->domainType == cpps_domain_type_namespace) //���ռ䲻��������
					//	leftdomain = takedomain;
				}
			}
			else
			{
				throw(cpps_error(d->filename, d->getleft()->line, cpps_error_classerror, "[%s] must be a class object or a domain before the '.'", d->getleft()->s.c_str()));
			}

		}
		else if (d->type == CPPS_QUOTEVARNAME)
		{
			cpps_regvar* v = getregvar(domain, d);
			if (v)
			{
				cpps_value tmp = v->getValue();

				ret = cpps_value(v->isConst() ? (&tmp) : (&v->getValue()));
			}
			else
			{
				printf("cpps warring: [%s] is not define.. line : %d, file : %s\n", d->s.c_str(), d->line, d->filename.c_str());
			}
		}
		else if (d->type == CPPS_QUOTEGETCHIILD)
		{
			cpps_value left = cpps_calculate_expression(c, domain, d->getleft(), leftdomain);
			if (left.tt != CPPS_TNIL)
			{
				if (left.tt == CPPS_TCLASSVAR)
				{
					cpps_cppsclass* cppsclass = (cpps_cppsclass*)left.value.domain->parent[0];
					if (d->getright()->type != CPPS_FUNCNAME && cppsclass->getClassName() == "vector")
					{
						cpps_cppsclassvar* cppsclassvar = (cpps_cppsclassvar*)left.value.domain;
						cpps_vector* pVec = (cpps_vector*)cppsclassvar->getclsptr();
						cpps_domain* takedomain = leftdomain;
						leftdomain = NULL;
						cpps_value right = cpps_calculate_expression(c, domain, d->getright()->getleft(), leftdomain);
						if (right.tt != CPPS_TINTEGER)
						{
							throw(cpps_error(d->getright()->filename, d->getright()->line, cpps_error_classerror, "Array must contain a number as an index."));
						}
						leftdomain = takedomain;
						if (pVec->size() <= right.value.integer)
							throw(cpps_error(d->getright()->filename, d->getright()->line, cpps_error_classerror, "Array has crossed the current length: [%d]. You need to get the length: [%d].", pVec->size(), right.value.integer));

						ret = cpps_value(&pVec->cpps_at(right.value.integer));
					}
					else if (d->getright()->type != CPPS_FUNCNAME && cppsclass->getClassName() == "map")
					{
						cpps_cppsclassvar* cppsclassvar = (cpps_cppsclassvar*)left.value.domain;
						cpps_map* pMap = (cpps_map*)cppsclassvar->getclsptr();
						cpps_domain* takedomain = leftdomain;
						leftdomain = NULL;
						cpps_value right = cpps_calculate_expression(c, domain, d->getright()->getleft(), leftdomain);
						leftdomain = takedomain;
						ret = cpps_value(&pMap->cpps_find(right));
					}
					else if (d->getright()->type != CPPS_FUNCNAME && cppsclass->getClassName() == "unordered_map")
					{
						cpps_cppsclassvar* cppsclassvar = (cpps_cppsclassvar*)left.value.domain;
						cpps_unordered_map* pMap = (cpps_unordered_map*)cppsclassvar->getclsptr();
						cpps_domain* takedomain = leftdomain;
						leftdomain = NULL;
						cpps_value right = cpps_calculate_expression(c, domain, d->getright()->getleft(), leftdomain);
						leftdomain = takedomain;
						cpps_value& t = pMap->cpps_find(right);
						ret = cpps_value(&t);
					}
					else
					{
						cpps_regvar* v = getregvar(left.value.domain, d->getright()->getleft());
						if (v)
						{
							cpps_value tmp = v->getValue();

							ret = cpps_value(v->isConst() ? (&tmp) : (&v->getValue()));
						}
					}
				}
			}
			else
			{
				throw(cpps_error(d->filename, d->getleft()->line, cpps_error_classerror, "[%s] must be a class object or a domain before the '.'", d->getleft()->s.c_str()));
			}
		}
		else if (d->type == CPPS_QUOTEGETOBJECT)
		{
			cpps_value left = cpps_calculate_expression(c, domain, d->getleft(), leftdomain);
			if (left.tt != CPPS_TNIL)
			{
				if (left.tt == CPPS_TCLASSVAR)
				{
					cpps_cppsclass* cppsclass = (cpps_cppsclass*)left.value.domain->parent[0];
					cpps_regvar* v = getregvar(left.value.domain, d->getright());
					if (v)
					{
						cpps_value tmp = v->getValue();

						ret = cpps_value(v->isConst() ? (&tmp) : (&v->getValue()));
					}
					else
					{
						printf("cpps warring: [%s] is not define.. line : %d, file : %s\n", d->getright()->s.c_str(), d->getright()->line, d->getright()->filename.c_str());
					}
				}
			}
			else
			{
				throw(cpps_error(d->filename, d->getleft()->line, cpps_error_classerror, "[%s] must be a class object or a domain before the '.'", d->getleft()->s.c_str()));
			}
		}
		return ret;
	}
	///////////////////////
	cpps_value cpps_execute_callfunction(C* c, cpps_function* f, cpps_domain* domain, std::string filename, int32 line, std::string funcname, std::vector<cpps_value>& params)
	{
		cpps_value ret;
		cpps_stack* stack = new cpps_stack(filename, line, funcname);
		c->push_stack(stack);

		f->callfunction(c, &ret, domain, &params, stack);
		c->pop_stack();
		delete stack;

		return ret;
	}
	void cpps_debug_breakpoint(C* c, cpps_domain* domain, Node* d)
	{
		std::vector<cpps_stack*>* stacklist = c->getcallstack();
		int i = 1;
		for (std::vector<cpps_stack*>::reverse_iterator it = stacklist->rbegin(); it != stacklist->rend(); ++it)
		{
			cpps::cpps_stack* stack = *it;
			std::cout << "#f" << i << " " << stack->f.c_str() << "	The" << stack->l << "line	function��" << stack->func.c_str() << std::endl;
			i++;
		}

		std::cout << "quit debug command : quit" << std::endl;
		while (true)
		{
			std::cout << "debug>";
			std::string str = "";
			std::cin >> str;

			if (str == "quit")
				break;

			Node* o = loadbuffer(c, domain, str, "");
			cpps_stack* stack = new cpps_stack(d->filename, d->line, "dostring");
			c->push_stack(stack);
			cpps_step_all(c, CPPS_SINGLERET, domain, o);
			c->pop_stack();
			delete stack;
		}
	}
	cpps_value cpps_step_callfunction(C* c, cpps_domain* domain, cpps_value func, Node* d, cpps_domain* leftdomain)
	{
		cpps_value ret;

		if (func.tt == CPPS_TFUNCTION)
		{
			cpps_function* f = func.value.func;
			cpps_domain* execdomain = new cpps_domain(domain, cpps_domain_type_func, "");
			execdomain->setexecdomain(domain);

			std::vector<cpps_value> params;
			cpps_value isNeedC;
			if (f->getIsNeedC())
			{
				isNeedC = cpps_cpp_to_cpps_converter<C*>::apply(c, c);
				params.push_back(isNeedC);

				cpps_regvar* v = new cpps_regvar;
				v->setValue(isNeedC);
				std::string pc = "pc";
				v->setVarName(pc);
				execdomain->regVar(c, v);
			}
			std::vector<cpps_regvar*> params_var;
			make_values(c, domain, d->getright(), params, params_var, execdomain);
			std::string filename = d->getright()->filename;
			std::string funcname = f->getfuncname();
			int32 line = d->getright()->line;
			if (f)
			{
				cpps_domain* execdomain = leftdomain;

				if (!execdomain) execdomain = c->_G;

				if (func.parentLambdaVar)
					execdomain = func.parentLambdaVar;

				ret = cpps_execute_callfunction(c, f, execdomain, filename, line, funcname, params);
			}

			cpps_regvar* v = cpps_node_to_regver(domain, d->getleft(), false);

			if (v && v->varName == "debug" && d->getleft()->getright()->s == "breakpoint")
			{
				cpps_debug_breakpoint(c, domain, d);
			}
			execdomain->destory(c);
			delete execdomain;
		}
		else
		{
			//��Ҫһ�����������������ǣ�������
			if (d->getleft()->l.size() == 2)
				printf("cpps warring: [%s] is not function  line:%d , file:%s \n", d->getleft()->getright()->s.c_str(), d->line, d->filename.c_str());
			else
				printf("cpps warring: [%s] is not function  line:%d , file:%s \n", d->getleft()->s.c_str(), d->line, d->filename.c_str());
		}
		return ret;
	}
	cpps::object _G(C* c)
	{
		return cpps_value(c->_G);
	}
	void collect(C* c)
	{
		cpps_gc_check_gen0(c);
	}
	void collectall(C* c)
	{
		cpps_gc_check_gen1(c);
	}
	void setchartrans(C* c, std::string(*func)(std::string&))
	{
		c->func = func;
	}
}