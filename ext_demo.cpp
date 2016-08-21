#include <stdio.h>
#include <pthread.h>
#include "ext_closure.h"

using namespace sofa;
using namespace pbrpc;

typedef struct
{
    int a;
	int b;
} T_Num;

int MyAdd(T_Num &num)
{
    return num.a + num.b;
}

typedef int (*Calc)(T_Num &);

class CTest
{
public:

    void Register(Calc pfun)
    {
        fun_ = NewPermanentExtClosure(pfun);
    }
	
    int GetResult(T_Num &num) const
    {
        if (NULL == fun_)
        {
            return 0;
        }
        return fun_->Run(num);
    }

    static void* MyFun(void *arg)
	{
	    T_Num *num =(T_Num *)arg;
	    int result = test_->GetResult(*num);
	    printf("result: %d\n", result);
	    return NULL;
	}
	
	static CTest* GetInstance()
	{
	    if (NULL == test_)
	    {
	        test_ = new CTest;
	    }
	    return test_;
	}
	
private:
    CTest()
    {
        test_ = NULL;
    }
    ~CTest()
    {
        if (NULL != test_)
        {
            delete test_;
        }
    }

private:
	static CTest *test_;
	ExtClosure<int(T_Num &)>* fun_;
};

CTest *CTest::test_ = NULL;

#define NLOOP (2)
T_Num g_num[NLOOP] = {{1, 2}, {3, 4}};

int main()
{
    CTest::GetInstance()->Register(MyAdd);
    
	pthread_t tid[NLOOP];
	int i;
    for (i = 0; i < NLOOP; i++)
    {
	    ::pthread_create(&tid[i], NULL, CTest::MyFun, (void *)&g_num[i]);
	}

    for (i = 0; i < NLOOP; i++)
    {
	    ::pthread_join(tid[i], NULL);
	}	
    return 0;
}


