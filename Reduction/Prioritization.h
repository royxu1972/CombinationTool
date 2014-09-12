/*
 *	last update: 2014/09/12
 */
#pragma once
#include<vector>
using namespace std;

class Prioritization
{
public:
	Prioritization()
	{
		parameter = sum = -1 ;
		size = 0 ;
		value = weight = order = NULL ;
		CArray = NULL ;
	};
	~Prioritization()
	{
		/*
		if( parameter != -1 )
		{
			delete[] value ;
			delete[] weight ;
			delete[] order ;
			for( int k=0 ; k<size ; k++ )
				delete[] CArray[k] ;
			delete[] CArray ;
		}*/
	}

public:
	int parameter ;
	int *value ;
	int **CArray ;
	int size ;        // the number of CArray rows
	int *weight ;

	int sum ;
	int *order ;

public:
    // greedy based prioritization by switching cost
	void reorder() ;

	// compute the distance between test a and b
	int distance( const int *a , const int *b ) ;

	/*  read CA from generation result
	 *	PARAMETER: par - the number of parameters
	 *			   v[] - the number of each values
	 *			   T - the covering array
	 *			   w[] - the specified weight
	 */
	void ReadVector( const int par , const int *v , const vector<int*> T , const int *w );

};