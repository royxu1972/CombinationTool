#pragma once
#include<iostream>
using namespace std;
// ----------------------------------------------------------------------------
// ����C(n,m)
// ----------------------------------------------------------------------------
inline int cal_combine( int n , int m )   
{
	int ret = 1 ;
	int p = n ;
	for( int x=1 ; x<=m ; x++ , p-- )
	{
		ret = ret * p ;
		ret = ret / x ;
	}
	return ret ;
}

// ----------------------------------------------------------------------------
// ����c[]��������ϵ��ֵ����е���ţ���n��������ѡm�������
// cal_combine2num( [1 2] , 4 , 2 ) = 3
// all order : 0 1 , 0 2 , 0 3 , 1 2 , 1 3 , 2 3
// ----------------------------------------------------------------------------
inline int cal_combine2num( const int *c , const int n , const int m )    
{
	int ret = cal_combine( n , m ) ;
	for( int i=0 ; i<m ; i++ )
		ret -= cal_combine( n-c[i]-1 , m-i );
	ret--;
	return ret ;                
}

// ----------------------------------------------------------------------------
// �����ֵ����t���Ĳ�����ϣ��������c�У���n��������ѡm������� (n,m)
// cal_combine2num( c , 2 , 4 , 2 )
// c = [0 3]
// ----------------------------------------------------------------------------
inline void cal_num2combine( int *c , int t , const int n , const int m )  
{
	t++;                        // ���� + 1
	int j=1 , k ;
	for( int i=0 ; i<m ; c[i++]=j++ )
		for( ; t > ( k = cal_combine(n-j, m-i-1) ) ; t-=k , j++ )
		;
	for( int p=0 ; p<m ; p++ )   // ��� - 1
		c[p]--;
}

// ----------------------------------------------------------------------------
// ��a��ֵ����, b�ж�Ӧ����, a,bͬ��
// version = 0 ����, version = 1 ����
// ----------------------------------------------------------------------------
inline void cal_sort( int *a , int *b , int left , int right , int version ) 
{
	// �����㷨���������� //
	int i , j ;
	int temp , temp_1 ;
	// ����
	if( version == 0 && left < right )
	{
		i = left ;
		j = right ;
		temp = a[i] ;  // ��ԪΪ��ߵ�һ��
		temp_1 = b[i] ;

		while( i != j )
		{
			// ���������ҵ�1��С��temp��λ��j
			while( a[j] >= temp && i < j ) 
				j-- ;        
			if( i < j )
			{ 
				a[i] = a[j] ;
				b[i] = b[j] ;
				i++ ;
			}
			// ���������ҵ�1������temp��λ��i
			while( a[i] <= temp && i < j )
				i++ ;
			if( i < j )
			{
				a[j] = a[i] ;
				b[j] = b[i] ;
				j-- ;
			}
		}
		 // ��temp��������λ��,���λ��ֽ���
		a[i] = temp ;
		b[i] = temp_1 ;

		cal_sort( a , b , left , i-1 , version );
		cal_sort( a , b , i+1 , right , version );
	}
	// ����
	if( version == 1 && left < right )
	{
		i = left ;
		j = right ;
		temp = a[i] ;  // ��ԪΪ��ߵ�һ��
		temp_1 = b[i] ;

		while( i != j )
		{
			// ���������ҵ�1������temp��λ��j
			while( a[j] <= temp && i < j ) 
				j-- ;        
			if( i < j )
			{ 
				a[i] = a[j] ;
				b[i] = b[j] ;
				i++ ;
			}
			// ���������ҵ�1��С��temp��λ��i
			while( a[i] >= temp && i < j )
				i++ ;
			if( i < j )
			{
				a[j] = a[i] ;
				b[j] = b[i] ;
				j-- ;
			}
		}
		 // ��temp��������λ��,���λ��ֽ���
		a[i] = temp ;
		b[i] = temp_1 ;

		cal_sort( a , b , left , i-1 , version );
		cal_sort( a , b , i+1 , right , version );
	}
}

// ----------------------------------------------------------------------------
// �Զ�ά����ary��a��ֵ����(����), pΪ�����г�
// ----------------------------------------------------------------------------
inline void cal_sortArray( int *a , int **ary , int left , int right , int p )
{
	int i , j ;
	int temp ;
	if( left < right )
	{
		i = left ;
		j = right ;
		temp = a[i] ;  // ��ԪΪ��ߵ�һ��
		int *tpc = new int[p];
		for( int k=0 ; k<p ; k++ )
			tpc[k] = ary[i][k];

		while( i != j )
		{
			// ���������ҵ�1��С��temp��λ��j
			while( a[j] > temp && i < j )
				j-- ;        
			if( i < j )
			{ 
				a[i] = a[j] ;		
				for( int k=0 ; k<p ; k++ )
					ary[i][k] = ary[j][k] ;
				i++ ;
			}
			// ���������ҵ�1������temp��λ��i
			while( a[i] < temp && i < j )
				i++ ;
			if( i < j )
			{
				a[j] = a[i] ;
				for( int k=0 ; k<p ; k++ )
					ary[j][k] = ary[i][k] ;
				j-- ;
			}
		}
		 // ��temp��������λ��,���λ��ֽ���
		a[i] = temp ;
		for( int k=0 ; k<p ; k++ )
			 ary[i][k] = tpc[k];
		delete[] tpc ;

		cal_sortArray( a , ary , left , i-1 , p );
		cal_sortArray( a , ary , i+1 , right , p );
	}
}