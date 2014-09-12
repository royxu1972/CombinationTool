/*------------------------------------------------
Covering Array Generation
last updated: 2013-06-17

Methods:
CT_Method()   ̰���㷨
RT_Method()   ����㷨
ART_Method()  ����Ӧ����㷨
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
	SUT *sut ;               // ����ϵͳ
	vector<int*> TResult ;   // �������ɵĲ��������� 

public:
	void CT_Method( int N ); 
	void RT_Method();    
	void ART_Method( int version , int N );   

private:
	// ʹ��CT������
	int **firstweight ;   // ��¼��������ȡֵδ���ִ���
	int *CT_GenerateTestCase();  
	int CT_FitSchema( const int *temp_test , const int n , const int v , const int count );
	void SetFirstWeight( const int *temp );

	// ʹ��ART��������
	int ART_TestFit( const int *temp , int version );

public:
	void clear_TResult();     // ���
	void testprint_TR();
	void testprint_firstweight();
};