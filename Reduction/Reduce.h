/*
 *	last update: 2014/06/20
 */
#pragma once
#include"algorithm.h"
#include"SUT.h"
#include<vector>
#include<map>
#include<string> 
#include<fstream>
#include<iostream>
using namespace std;

class Reduce
{
public:
	Reduce()
	{
		parameter = tway = row  = -1 ;
		value = NULL ;

		Original_Care = 0 ;
		Original_PossCare = 0 ;
		iteration = 0 ;
		print = false ;
	};
	~Reduce()
	{
		if( parameter != -1 )
		{
			delete[] value ;
			for( int k=0 ; k<row ; k++ )
				delete[] CArray[k] ;
			delete[] CArray ;
		}
	}

public:
	// CA
	int parameter ;
	int *value ;
	int tway ;

	int row ;       // the number of CA's row
	int **CArray ;  // CA, row * parameter

	// statistic info
	int PossCare , Original_PossCare ;  // possible care position
	int Care , Original_Care ;          // care position
	int Last_Care ;
	map<int,int> comb_count ;   // the number of cominations that appear 1, 2, ,..., times
	map<int,int> delite_count ;

	int iteration ;      // current iteration
	int all_iteration ;  // maximum iteration

public:
	/*  read from file
	 *	PARAMETER: filename - CA file name, specific format required
	 *			   all - the number of orginal rows
	 *			   p - whether to print the statistic result
	 */
	int ReadFile( string filename , int all , bool p );

	/*  read from generation result
	 *	PARAMETER: par - the number of parameters
	 *			   v[] - the number of each values
	 *			   t - covering strength
	 *			   T - the covering array
	 *			   all - the number of orginal rows
	 *			   p - whether to print the statistic result
	 */
	int ReadVector( const int par , const int *v , const int t , const vector<int*> T , int all , bool p );

	/*	main algorithms
     *	ReorderCArray() - find possible care positions
	 *  PossibleCareCArray() - find care positions and then eliminate and replace
	 */
	void Execution();
	bool ReorderCArray();
	bool PossibleCareCArray();

	// cal index of t-way combination
	int cal_index( const int *pos, const int *val );

	// output fundtions
	bool print ;
	void testprint( int **ca , int r , string str );
	void testprint_sta( bool final ) ;
};