#include "cpps.h"

namespace cpps
{
	void cpps_base_printf(object b);

	void cpps_gc_add_barrier(C*c, cpps_regvar *v)
	{
		if (!v->getIsJoinBarrier())
		{
			v->setIsJoinBarrier(true);
			c->getBarrierList()->insert(v);
		}
	}

	void cpps_gc_remove_barrier(C*c, cpps_regvar *v)
	{
		if (v->getIsJoinBarrier())
		{
			v->setIsJoinBarrier(false);
			c->getBarrierList()->erase(v);
		}
	}

	void cpps_gc_add_gen0(C*c, cpps_cppsclassvar *p)
	{
		c->setGen0size(c->getGen0size() + p->size());
		//新增到新生代
		c->getGen0()->insert(p);
	}
	void cpps_gc_add_gen1(C*c, cpps_cppsclassvar *p)
	{
		c->setGen1size(c->getGen1size() + p->size());
		//新增到老生代
		c->getGen1()->insert(p);
	}
	void cpps_gc_check_gen_value(C*c, cpps_value &v, bool checkchild, std::unordered_set<cpps_cppsclassvar *> *oldgen, std::unordered_set<cpps_cppsclassvar *> *newgen, size_t &size, std::unordered_set<cpps_cppsclassvar *> &isCheck);

	void cpps_gc_check_child(cpps_value &v, C* c, bool checkchild, std::unordered_set<cpps_cppsclassvar *> * oldgen, std::unordered_set<cpps_cppsclassvar *> * newgen, size_t &size, std::unordered_set<cpps_cppsclassvar *> &isCheck)
	{
		for (std::unordered_map<std::string, cpps_regvar*>::iterator it = v.value.domain->varList.begin(); it != v.value.domain->varList.end(); ++it)
		{
			cpps_regvar *var = it->second;
			if (var->getValue().tt == CPPS_TCLASSVAR|| var->getValue().tt == CPPS_TSTRING)
			{
				if(c->debug) printf("%s\n", var->varName.c_str());
				cpps_gc_check_gen_value(c, var->getValue(), checkchild, oldgen, newgen,size, isCheck);
			}
		}
	}
	//
	void cpps_gc_check_gen_value(C*c, cpps_value &v, bool checkchild, std::unordered_set<cpps_cppsclassvar *> *oldgen, std::unordered_set<cpps_cppsclassvar *> *newgen, size_t &size, std::unordered_set<cpps_cppsclassvar *> &isCheck)
	{
		if (v.tt == CPPS_TCLASSVAR)
		{
			if (isCheck.find((cpps_cppsclassvar*)v.value.domain) != isCheck.end()) return;//已经check过
			isCheck.insert((cpps_cppsclassvar*)v.value.domain);

			if (v.value.domain->getDomainName() == "vector")
			{
				cpps_cppsclassvar *pClsVar = (cpps_cppsclassvar *)v.value.domain;
				cpps_vector *pVector = (cpps_vector*)pClsVar->getclsptr();
				for (pVector->begin(); pVector->end(); pVector->next())
				{
					cpps_value value = pVector->it();
					cpps_gc_check_gen_value(c, value, checkchild, oldgen, newgen, size, isCheck);
				}
				std::unordered_set<cpps_cppsclassvar *>::iterator it = oldgen->find(pClsVar);
				if (it != oldgen->end())
				{
					oldgen->erase(it);
					newgen->insert(pClsVar);
					size += pClsVar->size();
					pClsVar->setGCLevel(1);//设置成老生代 哪怕也是老生代了 也设置一下
				}

			}
			else if (v.value.domain->getDomainName() == "map" )
			{
				cpps_cppsclassvar *pClsVar = (cpps_cppsclassvar *)v.value.domain;
				cpps_map *pMap = (cpps_map*)pClsVar->getclsptr();
				for (pMap->begin(); pMap->end(); pMap->next())
				{
					cpps_value value0 = pMap->key();
					cpps_value value1 = pMap->it();

					cpps_gc_check_gen_value(c, value0, checkchild, oldgen, newgen, size, isCheck);
					cpps_gc_check_gen_value(c, value1, checkchild, oldgen, newgen, size, isCheck);

				}
				std::unordered_set<cpps_cppsclassvar *>::iterator it = oldgen->find(pClsVar);
				if (it != oldgen->end())
				{
					oldgen->erase(it);
					newgen->insert(pClsVar);
					size += pClsVar->size();
					pClsVar->setGCLevel(1);//设置成老生代 哪怕也是老生代了 也设置一下
				}
			}
			else if (v.value.domain->getDomainName() == "unordered_map")
			{
				cpps_cppsclassvar *pClsVar = (cpps_cppsclassvar *)v.value.domain;
				cpps_unordered_map *pMap = (cpps_unordered_map*)pClsVar->getclsptr();
				for (pMap->begin(); pMap->end(); pMap->next())
				{
					cpps_value value0 = pMap->key();
					cpps_value value1 = pMap->it();

					cpps_gc_check_gen_value(c, value0, checkchild, oldgen, newgen, size, isCheck);
					cpps_gc_check_gen_value(c, value1, checkchild, oldgen, newgen, size, isCheck);

				}
				std::unordered_set<cpps_cppsclassvar *>::iterator it = oldgen->find(pClsVar);
				if (it != oldgen->end())
				{
					oldgen->erase(it);
					newgen->insert(pClsVar);
					size += pClsVar->size();
					pClsVar->setGCLevel(1);//设置成老生代 哪怕也是老生代了 也设置一下
				}
			}
			else if (v.tt == CPPS_TCLASSVAR)
			{
				cpps_cppsclassvar *pClsVar = (cpps_cppsclassvar *)v.value.domain;
				std::unordered_set<cpps_cppsclassvar *>::iterator it = oldgen->find(pClsVar);
				if (it != oldgen->end())
				{
					oldgen->erase(it);
					newgen->insert(pClsVar);
					size += pClsVar->size();
					pClsVar->setGCLevel(1);//设置成老生代 哪怕也是老生代了 也设置一下
					
				}
				if (checkchild )
				{
					cpps_gc_check_child(v, c, checkchild, oldgen, newgen, size, isCheck);
				}
			}
		}
		else if(v.tt == CPPS_TSTRING)
		{
			cpps_cppsclassvar *pClsVar = static_cast<cpps_cppsclassvar *>(v.value.domain);
			std::unordered_set<cpps_cppsclassvar *>::iterator it = oldgen->find(pClsVar);
			if (it != oldgen->end())
			{
				oldgen->erase(it);
				newgen->insert(pClsVar);
				size += pClsVar->size();
				pClsVar->setGCLevel(1);//设置成老生代 哪怕也是老生代了 也设置一下
			}
		}
	}
	//检测新生代
	void cpps_gc_check_gen0(C *c)
	{
		if (c->debug) printf("gc gen0\n");

		std::unordered_set<cpps_cppsclassvar *> isCheck;
	
		cpps_value value = c->_G;
		size_t tmp = c->getGen1size();
		cpps_gc_check_child(value, c, true, c->getGen0(), c->getGen1(), tmp, isCheck);
		c->setGen1size(tmp);


		std::unordered_set<cpps_cppsclassvar *> tempoldgen;
		size_t tempoldgensize = 0;
		//c->gclock.lock();

		for (std::unordered_set<cpps_regvar*>::iterator it = c->getBarrierList()->begin();
			it != c->getBarrierList()->end(); ++it)
		{
			cpps_regvar *v = *it;
			//size_t tmp = 0;
			cpps_gc_check_gen_value(c, v->getValue(), true, c->getGen0(), &tempoldgen, tempoldgensize, isCheck);
			//c->setGen1size(tmp);
		}



		//释放gen0里面的内存
		for (std::unordered_set<cpps_cppsclassvar *>::iterator it = c->getGen0()->begin();
			it != c->getGen0()->end(); ++it)
		{
			cpps_cppsclassvar * pClsVar = *it;
			pClsVar->destory(c);
			if (c->debug) printf("=======================================================gc to %s\n", pClsVar->getDomainName().c_str());
			//cpps_base_printf(cpps_value(pClsVar));
			delete pClsVar;
		}
		c->getGen0()->clear();
		*(c->getGen0()) = tempoldgen;
		c->setGen0size(tempoldgensize);

		//c->gclock.unlock();
	}

	//检测老生代  这样检测估计后面就卡死了。  还得想办法优化！！！
	void cpps_gc_check_gen1(C *c)
	{
		if (c->debug) printf("=======================================================gc all\n");

		std::unordered_set<cpps_cppsclassvar *> newgen;
		size_t newgensize = 0;
		std::unordered_set<cpps_cppsclassvar *> isCheck1;
		std::unordered_set<cpps_cppsclassvar *> isCheck;
		cpps_value value = c->_G;
		cpps_gc_check_child(value, c, true, c->getGen0(), &newgen, newgensize, isCheck);
		cpps_gc_check_child(value, c, true, c->getGen1(), &newgen, newgensize, isCheck1);



		std::unordered_set<cpps_cppsclassvar *> tempoldgen;
		size_t tempoldgensize = 0;

		//先把新生代的检测了
		for (std::unordered_set<cpps_regvar*>::iterator it = c->getBarrierList()->begin();
			it != c->getBarrierList()->end(); ++it)
		{
			cpps_regvar *v = *it;
			cpps_gc_check_gen_value(c, v->getValue(), true, c->getGen0(), &tempoldgen, tempoldgensize, isCheck);
			cpps_gc_check_gen_value(c, v->getValue(), true, c->getGen1(), &tempoldgen, tempoldgensize, isCheck1);
		}




		//释放gen0里面的内存
		for (std::unordered_set<cpps_cppsclassvar *>::iterator it = c->getGen0()->begin();
			it != c->getGen0()->end(); ++it)
		{
			cpps_cppsclassvar * pClsVar = *it;
			pClsVar->destory(c);
			if (c->debug) printf("=======================================================gc to %s\n", pClsVar->getDomainName().c_str());
			delete pClsVar;
		}
		c->getGen0()->clear();
		*(c->getGen0()) = tempoldgen;
		c->setGen0size(tempoldgensize);


		//释放gen1里面的内存
		for (std::unordered_set<cpps_cppsclassvar *>::iterator it = c->getGen1()->begin();
			it != c->getGen1()->end(); ++it)
		{
			cpps_cppsclassvar * pClsVar = *it;
			pClsVar->destory(c);
			if (c->debug) printf("=======================================================gc to %s\n", pClsVar->getDomainName().c_str());
			delete pClsVar;
		}
		c->getGen1()->clear();
		*(c->getGen1()) = newgen;
		c->setGen1size(newgensize);
		c->setLastgensize(c->getGen1size());



	}
	void		gc_cleanup(C *c,int tid )
	{
		//清理当前线程的
		for (std::unordered_set<cpps_cppsclassvar *>::iterator it = c->getGen0(tid)->begin();
			it != c->getGen0(tid)->end(); ++it)
		{
			cpps_cppsclassvar * pClsVar = *it;
			pClsVar->destory(c);
			if (c->debug) printf("=======================================================gc to %s\n", pClsVar->getDomainName().c_str());
			delete pClsVar;
		}
		c->getGen0(tid)->clear();
		c->setGen0size(0);

		//释放gen1里面的内存
		for (std::unordered_set<cpps_cppsclassvar *>::iterator it = c->getGen1(tid)->begin();
			it != c->getGen1(tid)->end(); ++it)
		{
			cpps_cppsclassvar * pClsVar = *it;
			pClsVar->destory(c);
			if (c->debug) printf("=======================================================gc to %s\n", pClsVar->getDomainName().c_str());
			delete pClsVar;
		}

		c->getGen1(tid)->clear();
		c->setGen1size(0, tid);
		c->setLastgensize(0, tid);
		c->getBarrierList(tid)->clear();

		auto gen0 = c->gen0[tid];
		if(gen0) delete gen0;
		c->gen0.erase(tid);

		auto gen1 = c->gen1[tid];
		if (gen1) delete gen1;
		c->gen1.erase(tid);

		auto barrierList = c->barrierList[tid];
		if (barrierList) delete barrierList;
		c->barrierList.erase(tid);

	}
	std::string gcinfo(C *c)
	{
		//c->gclock.lock();
		std::string ret = "";
		char buffer[1024];
		sprintf(buffer,"gen0内存 %I64d b\n", c->getGen0size());
		ret += buffer;
		sprintf(buffer, "gen1内存 %I64d b\n", c->getGen1size());	//测试 200字节进行清理年轻代
		ret += buffer;
		sprintf(buffer, "当前内存 %I64d b\n", c->getGen0size() + c->getGen1size());
		ret += buffer;
		sprintf(buffer, "c->barrierList.size(): %I64d 个\n", c->getBarrierList()->size());
		ret += buffer;
		sprintf(buffer, "c->gen1.size(): %I64d 个\n", c->getGen1()->size());
		ret += buffer;
		sprintf(buffer, "c->gen0.size(): %I64d 个\n", c->getGen0()->size());
		ret += buffer;

		for (std::unordered_set<cpps_cppsclassvar *>::iterator it = c->getGen1()->begin(); it != c->getGen1()->end(); it++)
		{
			auto v = *it;
			if (v->domainName == "String")
			{
				std::string *tmpStr = (std::string *)v->getclsptr();
				printf("%s\r\n", tmpStr->c_str());
			}
		}
		//c->gclock.unlock();
		return ret;
	}
	void cpps_reggc(C *c)
	{
		module(c,"GC")[
			def_inside("Collect", cpps_gc_check_gen0),
			def_inside("CollectAll", cpps_gc_check_gen1),
			def_inside("gcinfo", gcinfo)
		];
	}

}
