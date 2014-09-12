/*---------------------------------------------------------------------------------------------------------
Software Under Test (SUT)
last updated: 2013-06-17

Methods:
GenerateS()     generate all interactions that to be covered
FitnessValue()  compute the fitness value of one test case, i.e. the number of new covered interactions
Covered()       check whether the interation is covered or not
CheckCoverage() check whether the test suite is fully covered
---------------------------------------------------------------------------------------------------------*/
#ifdef SUT_EXPORTS
#define SUT_API __declspec(dllexport) 
#else
#define SUT_API __declspec(dllimport) 
#endif

#pragma once
#include"algorithm.h"
#include<vector>
using namespace std;

#define MAX 999999 ;
typedef unsigned char BYTE ;

struct SUBINFO // 子覆盖信息  
{
	// subway表示强度，subparameter表示参数个数，subposition[]表示参数位置
	int subway ;
	int subparameter ;
	int *subposition ;

	// 将在初始化时赋值，标记在AllS中位置
	int coverIndex ;  //  在AllS中行下标
	int coverSub ;    //  C( subparameter , subway )
};

class SUT
{
public:
	SUT( int p , const int *v , int t )
	{
		parameter = p ;
		value = new int[p] ;
		for( int i = 0 ; i < p ; i++ )
			value[i] = v[i] ;
		tway = t ;

		AllS = NULL ;
		coverMain = cal_combine( p , t ) ;  
		coverMax = coverMain ;
		testcaseCoverMax = coverMain ;
	};
	~SUT()
	{
		delete[] value ;
		if( AllS != NULL )
		{
			for( int i=0 ; i<coverMax ; i++ )
				delete[] AllS[i];
			delete[] AllS;
			AllS = NULL;
		}
		for( vector<int*>::iterator i = seedInfo.begin() ; i != seedInfo.end() ; i++ )
			delete[] (*i) ;
		seedInfo.clear();
	}

public:
	// CA变量
	int parameter ;
	int *value ;
	int tway ;

	// 子覆盖
	vector<SUBINFO> subInfo ;

	// 种子 
	vector<int*> seedInfo ;

	int SCount ;       // 未覆盖组合数
	int SCountAll ;    // 总的待覆盖组合数
	
private:
	// 存储所有应覆盖组合情况，以及记录还未覆盖组合数
	BYTE **AllS ;           // 该位为0表示未覆盖
	int coverMain ;         // 记录Main行数，C(parameter,tway)
	int coverMax ;          // 记录AllS总行数 = Main + AllSub
	int testcaseCoverMax ;  // 一个测试用例最多能覆盖组合数

public:
	// 信息初始化
	void SetSub( int way , int par , const int *pos );
	void SetSeed( const vector<int*> seed ); 

	// 生成AllS
    void GenerateS() ;
	// 计算fitness
	int FitnessValue( const int *test , int FLAG );
	// 判断是否覆盖
    bool Covered( const int *pos , const int *sch , int FLAG );
	
	// 种子预处理
	void preforseed();

	// 输出AllS
	void printAllS();

	// 检查t-way覆盖率
	double CheckCoverage( int **ca , int r , int t );

private:
	// 处理sub部分
	void GenerateS_Sub();
	int FitnessValue_Sub( const int *test , int FLAG );
	bool Covered( const int *pos , const int *sch , vector<SUBINFO>::const_iterator sub , int FLAG );

};







