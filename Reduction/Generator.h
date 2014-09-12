/*------------------------------------------------
Covering Array Generation
last updated: 2013-06-17

Methods:
CT_Method()   贪心算法
RT_Method()   随机算法
ART_Method()  自适应随机算法
------------------------------------------------*/
#pragma once
#include"algorithm.h"
#include"SUT.h"
#include<vector>
using namespace std;

class Generator
{
public:
	Generator( SUT *s )
	{
		sut = s ;
	};
	~Generator()
	{
		clear_TResult();
	}

public:
	SUT *sut ;               // 待测系统
	vector<int*> TResult ;   // 最终生成的测试用例集 

public:
	void CT_Method( int N ); 
	void RT_Method();    
	void ART_Method( int version , int N );   

private:
	// 使用CT方法生
	int **firstweight ;   // 记录各参数各取值未出现次数
	int *CT_GenerateTestCase();  
	int CT_FitSchema( const int *temp_test , const int n , const int v , const int count );
	void SetFirstWeight( const int *temp );

	// 使用ART方法生成
	int ART_TestFit( const int *temp , int version );

public:
	void clear_TResult();     // 清空
	void testprint_TR();
	void testprint_firstweight();
};