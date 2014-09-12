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

struct SUBINFO // �Ӹ�����Ϣ  
{
	// subway��ʾǿ�ȣ�subparameter��ʾ����������subposition[]��ʾ����λ��
	int subway ;
	int subparameter ;
	int *subposition ;

	// ���ڳ�ʼ��ʱ��ֵ�������AllS��λ��
	int coverIndex ;  //  ��AllS�����±�
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
	// CA����
	int parameter ;
	int *value ;
	int tway ;

	// �Ӹ���
	vector<SUBINFO> subInfo ;

	// ���� 
	vector<int*> seedInfo ;

	int SCount ;       // δ���������
	int SCountAll ;    // �ܵĴ����������
	
private:
	// �洢����Ӧ�������������Լ���¼��δ���������
	BYTE **AllS ;           // ��λΪ0��ʾδ����
	int coverMain ;         // ��¼Main������C(parameter,tway)
	int coverMax ;          // ��¼AllS������ = Main + AllSub
	int testcaseCoverMax ;  // һ��������������ܸ��������

public:
	// ��Ϣ��ʼ��
	void SetSub( int way , int par , const int *pos );
	void SetSeed( const vector<int*> seed ); 

	// ����AllS
    void GenerateS() ;
	// ����fitness
	int FitnessValue( const int *test , int FLAG );
	// �ж��Ƿ񸲸�
    bool Covered( const int *pos , const int *sch , int FLAG );
	
	// ����Ԥ����
	void preforseed();

	// ���AllS
	void printAllS();

	// ���t-way������
	double CheckCoverage( int **ca , int r , int t );

private:
	// ����sub����
	void GenerateS_Sub();
	int FitnessValue_Sub( const int *test , int FLAG );
	bool Covered( const int *pos , const int *sch , vector<SUBINFO>::const_iterator sub , int FLAG );

};







