#pragma once
#include<iostream>
using namespace std;
// ----------------------------------------------------------------------------
// 计算C(n,m)
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
// 计算c[]在所有组合的字典序中的序号，在n个参数中选m个情况下
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
// 计算字典序第t个的参数组合，结果存入c中，在n个参数中选m个情况下 (n,m)
// cal_combine2num( c , 2 , 4 , 2 )
// c = [0 3]
// ----------------------------------------------------------------------------
inline void cal_num2combine( int *c , int t , const int n , const int m )  
{
	t++;                        // 输入 + 1
	int j=1 , k ;
	for( int i=0 ; i<m ; c[i++]=j++ )
		for( ; t > ( k = cal_combine(n-j, m-i-1) ) ; t-=k , j++ )
		;
	for( int p=0 ; p<m ; p++ )   // 输出 - 1
		c[p]--;
}

// ----------------------------------------------------------------------------
// 按a的值排序, b中对应交换, a,b同长
// version = 0 升序, version = 1 降序
// ----------------------------------------------------------------------------
inline void cal_sort( int *a , int *b , int left , int right , int version ) 
{
	// 排序算法：快速排序 //
	int i , j ;
	int temp , temp_1 ;
	// 升序
	if( version == 0 && left < right )
	{
		i = left ;
		j = right ;
		temp = a[i] ;  // 主元为左边第一个
		temp_1 = b[i] ;

		while( i != j )
		{
			// 从右向左找第1个小于temp的位置j
			while( a[j] >= temp && i < j ) 
				j-- ;        
			if( i < j )
			{ 
				a[i] = a[j] ;
				b[i] = b[j] ;
				i++ ;
			}
			// 从左向右找第1个大于temp的位置i
			while( a[i] <= temp && i < j )
				i++ ;
			if( i < j )
			{
				a[j] = a[i] ;
				b[j] = b[i] ;
				j-- ;
			}
		}
		 // 将temp放入最终位置,本次划分结束
		a[i] = temp ;
		b[i] = temp_1 ;

		cal_sort( a , b , left , i-1 , version );
		cal_sort( a , b , i+1 , right , version );
	}
	// 降序
	if( version == 1 && left < right )
	{
		i = left ;
		j = right ;
		temp = a[i] ;  // 主元为左边第一个
		temp_1 = b[i] ;

		while( i != j )
		{
			// 从右向左找第1个大于temp的位置j
			while( a[j] <= temp && i < j ) 
				j-- ;        
			if( i < j )
			{ 
				a[i] = a[j] ;
				b[i] = b[j] ;
				i++ ;
			}
			// 从左向右找第1个小于temp的位置i
			while( a[i] >= temp && i < j )
				i++ ;
			if( i < j )
			{
				a[j] = a[i] ;
				b[j] = b[i] ;
				j-- ;
			}
		}
		 // 将temp放入最终位置,本次划分结束
		a[i] = temp ;
		b[i] = temp_1 ;

		cal_sort( a , b , left , i-1 , version );
		cal_sort( a , b , i+1 , right , version );
	}
}

// ----------------------------------------------------------------------------
// 对二维数组ary按a的值排序(升序), p为数组列长
// ----------------------------------------------------------------------------
inline void cal_sortArray( int *a , int **ary , int left , int right , int p )
{
	int i , j ;
	int temp ;
	if( left < right )
	{
		i = left ;
		j = right ;
		temp = a[i] ;  // 主元为左边第一个
		int *tpc = new int[p];
		for( int k=0 ; k<p ; k++ )
			tpc[k] = ary[i][k];

		while( i != j )
		{
			// 从右向左找第1个小于temp的位置j
			while( a[j] > temp && i < j )
				j-- ;        
			if( i < j )
			{ 
				a[i] = a[j] ;		
				for( int k=0 ; k<p ; k++ )
					ary[i][k] = ary[j][k] ;
				i++ ;
			}
			// 从左向右找第1个大于temp的位置i
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
		 // 将temp放入最终位置,本次划分结束
		a[i] = temp ;
		for( int k=0 ; k<p ; k++ )
			 ary[i][k] = tpc[k];
		delete[] tpc ;

		cal_sortArray( a , ary , left , i-1 , p );
		cal_sortArray( a , ary , i+1 , right , p );
	}
}