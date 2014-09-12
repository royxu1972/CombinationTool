#include"Reduce.h"
#include"Prioritization.h"
#include"Generator.h"
#include"SUT.h"
#include<iostream>
#include<ctime>
using namespace std;

int main(int argc, char* argv[])
{
	srand( (unsigned int)time(NULL) );

	// generate a CA
	int p = 10 ;
	int *v = new int[p] ;
	for( int k=0 ; k<p ; k++ )
		v[k] = 5 ;
	int t = 2 ;
	SUT *s = new SUT( p , v , t );

	Generator g(s);
	g.CT_Method(10);
	cout<<"original size: "<<g.TResult.size()<<endl;

	//
	// reduction example
	//

	// 1) initialize reduce
	Reduce a ;
	a.ReadVector( p , v , t , g.TResult, g.TResult.size(), 0 ) ;

	// 2) execute reduction
	a.Execution();

	// 3) final test suite
	cout<<"reduced size: "<<a.row<<endl;

	for( int i=0 ; i<a.row ; i++ )
	{
		for( int j=0 ; j<p ; j++ )
			cout<<a.CArray[i][j]<<" ";
		cout<<endl;
	}


	//
	// prioritization example
	//

	// 1) initialize
	Prioritization pri ;
	int w[10] = {1,1,1,1,1,1,1,1,1} ;
	pri.ReadVector( p, v, g.TResult, w );
	cout<<"original cost: "<<pri.sum<<endl;

	// 2) execution
	pri.reorder() ;
	cout<<"final cost: "<<pri.sum<<endl;
	cout<<"final order: ";
	for( int i=0 ; i<pri.size ; i++ )
		cout<<pri.order[i]<<" ";
	cout<<endl;
	
	system("Pause");
	return 0 ;
}