#include"Prioritization.h"
using namespace std ;

/*  read CA from generation result
 *	PARAMETER: par - the number of parameters
 *			   v[] - the number of each values
 *			   T - the covering array
 *			   w[] - the specified weight
 */
void Prioritization::ReadVector( const int par , const int *v, const vector<int*> T , const int *w )
{
	// basic
	parameter = par ;
	value = new int[par] ;
	weight = new int[par] ;
	for( int k=0 ; k<par ; k++ ) 
	{
		value[k] = v[k] ;
		weight[k] = w[k] ;
	}

	// covering array
	size = T.size();
	CArray = new int*[size] ;
	vector<int*>::const_iterator index = T.begin() ;
	for( int i=0 ; i<size ; i++ , index++ )
	{
		CArray[i] = new int[parameter] ;
		for( int j=0 ; j<parameter; j++ )
			CArray[i][j] = (*index)[j] ;
	}

	// default order
	order = new int[size] ;
	sum = 0 ;
	for( int i=0 ; i<size-1 ; i++ )
	{
		order[i] = i ;
		sum += distance(CArray[i], CArray[i+1]) ;
	}
	order[size-1] = size-1 ;

}

// compute the distance between test a and b
int Prioritization::distance( const int *a , const int *b )
{
	int dis = 0 ;
	for( int i=0 ; i<parameter ; i++ )
	{
		if( a[i] != b[i] )
			dis += weight[i] ;
	}
	return dis ;
}

// greedy based prioritization by switching cost
void Prioritization::reorder()
{
	int *already = new int[size] ;
	for( int k=0 ; k<size ; k++ )
		already[k] = 0 ;

	// randomly select the first one
	int start = rand()%size ;
	order[0] = start ;
	already[start] = 1 ;

	// select the i-th test case
	int index = -1 ;
	for( int i=1 ; i<size ; i++ )
	{
		int min = 9999999 ;
		for( int k=0 ; k<size ; k++ )
		{
			if( already[k] == 0 )
			{
				int tp = distance(CArray[order[i-1]], CArray[k]) ;
				if( tp < min )
				{
					min = tp ;
					index = k ;
				}
			}
		} // end for each unselected one

		order[i] = index ;
		already[index] = 1 ;
	}

	// update new cost
	sum = 0 ;
	for( int i=0 ; i<size-1 ; i++ )
		sum += distance(CArray[order[i]], CArray[order[i+1]]) ;

}