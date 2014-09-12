#include"Generator.h"
#include<cmath>
#include<iostream>
using namespace std;


// ----------------------------------------------------------------------------
// 清空及初始化
// ----------------------------------------------------------------------------
void Generator::clear_TResult()
{
	for( vector<int*>::iterator i = TResult.begin() ; i != TResult.end() ; i++ )
		delete[] (*i) ;
	TResult.clear();
}

// ----------------------------------------------------------------------------
// 使用CT方法生成
// 输入：候选测试用例集大小N
// 输出：测试用例集存入TResult
// ----------------------------------------------------------------------------
void Generator::CT_Method( int N )
{
	// 生成S
	sut->GenerateS();

	// firstweight初始化
	firstweight = NULL ;
	firstweight = new int*[sut->parameter];
	for( int i=0 ; i<sut->parameter ; i++ )
	{
		firstweight[i] = new int[sut->value[i]+1];
		firstweight[i][0] = 0 ;
		for( int j=1 ; j<sut->value[i]+1 ; j++ )	
		{
			int allc = 0 ;
			for( int k=0 ; k<sut->parameter ; k++ )
			{
				if( k != i )
					allc += sut->value[k] ;
			}
			firstweight[i][j] = allc ;
			firstweight[i][0] += allc ;
		}
	}

	// TResult初始化
	clear_TResult();

	int *temp ;
	while( sut->SCount != 0 )
	{
		//  记录每次的最好结果
		int *best ;   
		best = new int[sut->parameter] ;
		int ptrcov = 0 ;

		for( int x=0 ; x<N ; x++ )   // 再生成N条测试用例
		{
			temp = CT_GenerateTestCase();

			// 比较
			int covered = sut->FitnessValue(temp, 0);
			if( covered > ptrcov )
			{
				ptrcov = covered ;
				for( int x=0 ; x<sut->parameter ; x++ )
					best[x] = temp[x] ;
			}
			else
				delete[] temp ;     // 删除temp空间
		}

		// 覆盖能力最强的加入TResult, 修改firstweight, 并从S中删去已覆盖对
		TResult.push_back( best );
		SetFirstWeight( best );
		sut->FitnessValue( best , 1 );
	}
}

// ----------------------------------------------------------------------------
// 修改firstweight，对temp的每个tway组合分别考虑
// ----------------------------------------------------------------------------
void Generator::SetFirstWeight( const int *temp )
{
	// 依次按字典序生成temp的各种组合情况
	int *position = new int[sut->tway] ;      // 储存参数情况
	int *position_max = new int[sut->tway];   // 各自最大值
	for( int k=0 ; k<sut->tway ; k++ )  // 初始化
	{
		position[k] = k ;
		position_max[k] = sut->parameter - sut->tway + k ;
	}
	int end = sut->tway - 1 ;
	int ptr = end ;

	int *schema = new int[sut->tway] ;  // 存储取值的情况

	for( int x = 0 ; x < cal_combine(sut->parameter,sut->tway) ; x++ )
	{
		// 得到组合情况
		for( int k=0 ; k<sut->tway ; k++ )
			schema[k] = temp[position[k]] ;
		// 若尚未覆盖，则temp将覆盖它
		if( !sut->Covered( position , schema , 0 ) )
		{
			for( int y=0 ; y<sut->tway ; y++ )
			{
				firstweight[position[y]][schema[y]+1]--;
				firstweight[position[y]][0]--;
			}
		}
			
		// 生成一下个
		position[end] = position[end] + 1 ;  // 末位加1
		ptr = end ;
		while( ptr > 0 )
		{
			if( position[ptr] > position_max[ptr] )  // 超过该位允许最大值
			{
				position[ptr-1] = position[ptr-1] + 1 ;   // 前一位加1
				ptr-- ;
			}
			else
				break ;
		}
		if( position[ptr] <= position_max[ptr])  // 若该位值不是最大，后面每位在前一位基础上加1
		{
			for( int i=ptr+1 ; i<sut->tway ; i++ ) 
				position[i] = position[i-1] + 1 ;
		}
	}
	delete[] position;
	delete[] position_max;
	delete[] schema;
}


// ----------------------------------------------------------------------------
// 生成一个新的测试用例
// ----------------------------------------------------------------------------
int *Generator::CT_GenerateTestCase()
{
	int willfit = 0 ;  // 该用例能覆盖的组合数

	int *test = new int[sut->parameter];
	for( int p=0 ; p<sut->parameter ; p++ )
		test[p] = -1 ;

	int assigned_count = 0 ;   // 已确定参数个数
	int un_count = sut->parameter ;  // 未确定参数个数

	int *parameter_order ;  // 存储未确定参数序号
	int **weight ;  // 存储各参数各取值的权重

	// 变量
	int *p_number ;
	int *p_order ;
	int *v_number ;
	int *v_order ;

	// 最终选择结果
	int maxp = 0 ;
	int maxv = 0 ;

	//
	// 选择第一个参数，直接从firstweight中计算
	//
	
	// 找到一个最大的参数
	p_number = new int[sut->parameter];
	p_order = new int[sut->parameter];
	for( int x=0 ; x<sut->parameter ; x++ )
	{
		p_order[x] = x ;
		p_number[x] = firstweight[x][0] ;
	}
	// 排序
	cal_sort( p_number , p_order , 0 , un_count-1 , 1 );
	// 同序处理
	int p_equal_judge = 1 ;
	int p_equal_number = p_number[0] ;
	for( int l=1 ; l<sut->parameter ; l++ )
	{
		if( p_number[l] == p_equal_number )
			p_equal_judge++;
		else
			break;
	}
	// 有相同值的参数，在其中随机取一个值作为返回值
	if( p_equal_judge > 1 )   
	{
		int tt = rand()%p_equal_judge ;
		maxp = p_order[tt] ;
	}
	else
		maxp = p_order[0] ;
	
	delete[] p_number;
	delete[] p_order ;

	// 找到一个最大取值
	int col = sut->value[maxp] ;
	v_number = new int[col];
	v_order = new int[col];

	for( int x=0 ; x<col ; x++ )
	{
		v_order[x] = x ;
		v_number[x] = firstweight[maxp][x+1] ;
	}
	// 排序
	cal_sort( v_number , v_order , 0 , col-1 , 1 );
	// 同序处理
	int v_equal_judge = 1 ;
	int v_equal_number = v_number[0] ;
	for( int l=1 ; l<col ; l++ )
	{
		if( v_number[l] == v_equal_number )
			v_equal_judge++;
		else
			break;
	}
	// 有相同值的参数，在其中随机取一个值作为返回值
	if( v_equal_judge > 1 )   
	{
		int tt = rand()%v_equal_judge ;
		maxv = v_order[tt] ;
	}
	else
		maxv = v_order[0] ;
	
	delete[] v_number ;
	delete[] v_order  ;


	// 加入test
	test[maxp] = maxv ;

	// 更新
	assigned_count++;
	un_count--;
	
	// 第一个参数选择完毕 //

	while( assigned_count < sut->parameter )
	{
		//
		// 生成新的weight
		//

		// 找到未确定的参数序号
		parameter_order = new int[un_count] ;
		for( int x=0 , y=0 ; x<sut->parameter , y<un_count ; x++ )   
		{
			if( test[x] == -1 )
			{
				parameter_order[y] = x ;
				y++ ;
			}
		}

		// 生成新的二维数组，并同时计算weight
		weight = new int*[un_count];
		for( int i=0 ; i<un_count ; i++ )
		{
			int jmax = sut->value[parameter_order[i]] + 2 ;
			weight[i] = new int[jmax] ;
			weight[i][0] = parameter_order[i] ;
			weight[i][1] = 0 ;
			for( int j=2 ; j<jmax ; j++ )
			{
				int pp = weight[i][0] ;
				int vv = j - 2 ;
				weight[i][j] = CT_FitSchema( test , pp , vv , assigned_count+1 );
				weight[i][1] += weight[i][j] ;
			}
		}
		
		delete[] parameter_order ;

		// 找到一个最大的参数
		int *p_number = new int[un_count];
		int *p_order = new int[un_count];

		for( int x=0 ; x<un_count ; x++ )
		{
			p_order[x] = weight[x][0] ;
			p_number[x] = weight[x][1] ;
		}
		// 排序
		cal_sort( p_number , p_order , 0 , un_count-1 , 1 );
		// 同序处理
		int p_equal_judge = 1 ;
		int p_equal_number = p_number[0] ;
		for( int l=1 ; l<un_count ; l++ )
		{
			if( p_number[l] == p_equal_number )
				p_equal_judge++;
			else
				break;
		}
		// 有相同值的参数，在其中随机取一个值作为返回值
		if( p_equal_judge > 1 )   
		{
			int tt = rand()%p_equal_judge ;
			maxp = p_order[tt] ;
		}
		else
			maxp = p_order[0] ;

		delete[] p_number;
		delete[] p_order ;

		// 找到一个最大取值
		int col = sut->value[maxp] ;
		int *v_number = new int[col];
		int *v_order = new int[col];

		int prow = 0 ;

		for( int x=0 ; x<un_count ; x++ )
		{
			if( weight[x][0] == maxp )
			{
				prow = x ;
				for( int m=0 ; m<col ; m++ )
				{
					v_order[m] = m ;
					v_number[m] = weight[x][m+2] ;
				}
				break;
			}
		}

		// 排序
		cal_sort( v_number , v_order , 0 , col-1 , 1 );
		// 同序处理
		int v_equal_judge = 1 ;
		int v_equal_number = v_number[0] ;
		for( int l=1 ; l<col ; l++ )
		{
			if( v_number[l] == v_equal_number )
				v_equal_judge++;
			else
				break;
		}
		// 有相同值的参数，在其中随机取一个值作为返回值
		if( v_equal_judge > 1 )   
		{
			int tt = rand()%v_equal_judge ;
			maxv = v_order[tt] ;
		}
		else
			maxv = v_order[0] ;

		delete[] v_number ;
		delete[] v_order  ;

		// 加入test
		test[maxp] = maxv ;

		// 更新 test fit
		if( assigned_count + 1 >= sut->tway )
			willfit += weight[prow][maxv+2] ; 

		assigned_count++;
		un_count--;

		for( int i=0 ; i<un_count ; i++ )
			delete[] weight[i];
		delete[] weight;
		weight = NULL;

	}  // end while

	return test ;
}

// ----------------------------------------------------------------------------
// 输入：temp_test[parameter]，count-1位前已确定为fixed value，目前考虑第count个参数，参数序号n，参数取值v
// 输出：该temp_test在S中的覆盖数
// ----------------------------------------------------------------------------
int Generator::CT_FitSchema( const int *temp_test , const int n , const int v , const int count )
{
	int fit = 0 ;  // 返回值

	int *test = new int[sut->parameter];
	for( int k=0 ; k<sut->parameter ; k++ )
		test[k] = temp_test[k] ;
	
	//
	// 此时参数n的值v被加入，已确定的参数个数count小于tway，从未确定的参数中选择所有的tway-count个参数的组合情况，
	// 对于每一种情况，计算该情况下各涉及参数的所有取值，然后生成一个position_1和schema_1，使用Covered函数计算该情况是否覆盖
	//  
	if( count < sut->tway )
	{
		// 将v加入test[]
	    test[n] = v ;

		int *position = new int[sut->tway];
		int *schema = new int[sut->tway];

		int *position_1 = new int[sut->tway] ;
		int *schema_1 = new int[sut->tway] ;

		// 已确定count个参数
		// 未确定parameter - count个参数
		int un_count = sut->parameter - count ;
		int *un_pos = new int[un_count] ;

		// 赋值
		for( int k=0 , p=0 , q=0 ; k<sut->parameter ; k++ )
		{
			if( test[k] != -1 )
			{
				position[q] = k ;
				schema[q] = test[k] ;
				q++ ;
			}
			else
			{
				un_pos[p] = k ;
				p++;
			}
		}
		
		// 从un中选择tway-count个，与position组成一个position和schema，然后计算是否可覆盖
		int need_choose = sut->tway - count ;
		int allrow = cal_combine( un_count , need_choose );

		// 按字典序生成所有组合情况
		int *temp = new int[need_choose] ;      // 迭代记录，存储位置，对应un_pos
		int *un_value = new int[need_choose];   // 迭代记录，各取值
		int *temp_max = new int[need_choose];   // 各自最大值
		for( int k=0 ; k<need_choose ; k++ )   // 初始化
		{
			temp[k] = k ;
			temp_max[k] = un_count - need_choose + k ;
		}
		int end = need_choose - 1 ;
		int ptr = end ;

		int already = 0 ;
		while( already < allrow )
		{
			// begin every value //
			// 对该位置的各取值 
			int allcomb = 1 ;   // 组合数
			for( int p=0 ; p<need_choose ; p++ )
				allcomb = allcomb * sut->value[un_pos[temp[p]]] ;

			// 递增记录为un_value
			for( int k=0 ; k<need_choose ; k++ )
				un_value[k] = 0 ;
			int cend = need_choose - 1 ;         // 标记每次加1的位置，即最末尾
			int cptr = cend ;             // 标记依次进位          

			// 对所有组合
			for( int k = 0 ; k < allcomb ; k++ )
			{
				// 加入position
				for( int x=count , y=0 ; x<sut->tway , y<need_choose ; x++ , y++ )
				{
					position[x] = un_pos[temp[y]] ;
					schema[x] = un_value[y] ;
				}

				for( int x=0 ; x<sut->tway ; x++ )
				{
					position_1[x] = position[x] ;
					schema_1[x] = schema[x] ;
				}

				/*
				cout<<"position_1: ";
				for( int i=0 ; i<tway ; i++ )
					cout<<position_1[i]<<" ";
				cout<<"schema_1: ";
				for( int i=0 ; i<tway ; i++ )
					cout<<schema_1[i]<<" ";
				cout<<endl;
				*/

				// position_1和schema_1排序
				cal_sort( position_1 , schema_1 , 0 , sut->tway-1 , 0 );

				// 判断是否覆盖
				if( !sut->Covered( position_1 , schema_1 , 0 ) )
					fit++ ;
			
				// add
				un_value[cend] = un_value[cend] + 1 ;
				cptr = cend ;
				while( cptr > 0 )
				{
					if( un_value[cptr] > sut->value[un_pos[temp[cptr]]] - 1 )
					{
						un_value[cptr-1] = un_value[cptr-1] + 1 ;
						un_value[cptr] = 0 ;
						cptr-- ;
					}
					else
						break;
				}
			} 
			// end every value //

			// 求下一个组合
			temp[end] = temp[end] + 1 ;  // 末位加1
			ptr = end ;
			while( ptr > 0 )
			{
				if( temp[ptr] > temp_max[ptr] )  // 超过该位允许最大值
				{
					temp[ptr-1] = temp[ptr-1] + 1 ;   // 前一位加1
					ptr-- ;
				}
				else
					break ;
			}
			if( temp[ptr] <= temp_max[ptr])  // 若该位值不是最大，后面每位在前一位基础上加1
			{
				for( int i=ptr+1 ; i<need_choose ; i++ ) 
					temp[i] = temp[i-1] + 1 ;
			}

			already++ ;
		}  // end while

		delete[] position ;
		delete[] position_1 ;
		delete[] schema ;
		delete[] schema_1 ;
		delete[] un_pos ;
		delete[] temp ;
		delete[] un_value ;
		delete[] temp_max ;

		delete[] test ;

		return fit;

	}  // end if

	//
	// 此时已确定参数个数count>=tway，因此，从已确定的参数中选择所有的tway-1组合情况，
	// 对每一种情况，与参数n的值v构成position与schema，使用Covered函数计算覆盖数
	//
	else
	{
		int *position = new int[sut->tway];
		int *schema = new int[sut->tway];

		// 已确定参数个数为count-1
		int al_count = count - 1 ;
		int *al_pos = new int[al_count] ;
		int *al_value = new int[al_count] ;

		// 赋值
		for( int k=0 , p=0 ; k<sut->parameter ; k++ )
		{
			if( test[k] != -1 )
			{
				al_pos[p] = k ;
				al_value[p] = test[k] ;
				p++ ;
			}
		}

		// 从已确定的al_count个参数中选need_choose个参数
		int need_choose = sut->tway - 1 ;
		int allrow = cal_combine( al_count , need_choose );

		// 按字典序生成所有组合情况
		int *temp = new int[need_choose] ;      // 迭代记录，位置，对应un_pos
		int *temp_max = new int[need_choose];  // 各自最大值
		for( int k=0 ; k<need_choose ; k++ )  // 初始化
		{
			temp[k] = k ;
			temp_max[k] = al_count - need_choose + k ;
		}
		int end = need_choose - 1 ;
		int ptr = end ;

		int already = 0 ;
		while( already < allrow )
		{
			// 此时temp[]中所存内容未al_pos[]的下标，将其取出加入position和schema中
			for( int w=0 ; w<sut->tway-1 ; w++ )
			{
				position[w] = al_pos[temp[w]] ;
				schema[w] = al_value[temp[w]] ;
			}
			// 加入参数n
			position[sut->tway-1] = n ;
			schema[sut->tway-1] = v ;

			// 排序
			cal_sort( position , schema , 0 , sut->tway-1 , 0 );
			// 判断是否覆盖
			if( !sut->Covered( position , schema , 0 ) )
				fit++ ;

			// 求下一个组合
			temp[end] = temp[end] + 1 ;  // 末位加1
			ptr = end ;
			while( ptr > 0 )
			{
				if( temp[ptr] > temp_max[ptr] )  // 超过该位允许最大值
				{
					temp[ptr-1] = temp[ptr-1] + 1 ;   // 前一位加1
					ptr-- ;
				}
				else
					break ;
			}
			if( temp[ptr] <= temp_max[ptr])  // 若该位值不是最大，后面每位在前一位基础上加1
			{
				for( int i=ptr+1 ; i<need_choose ; i++ ) 
					temp[i] = temp[i-1] + 1 ;
			}

			already++ ;
		}  // end while

		delete[] position ;
		delete[] schema ;
		delete[] al_pos ;
		delete[] al_value ;
		delete[] temp ;
		delete[] temp_max ;

		delete[] test ;

		return fit ;
	}
}

// ----------------------------------------------------------------------------
// 使用RT方法生成
// 输出：测试用例集存入TResult
// ----------------------------------------------------------------------------
void Generator::RT_Method()
{
	//srand( (unsigned int)time(NULL) );

	// 生成S
	sut->GenerateS();
	// 初始化TResult
	clear_TResult();

	while( sut->SCount != 0 )
	{
		bool judge = true ;

		int *temp = new int[sut->parameter];

		do {
			// 随机生成一个测试用例t
			for( int i=0 ; i<sut->parameter ; i++ )
				temp[i] = (int)(((double)(rand()%1000)/1000.0)*sut->value[i]) ;

			// 判断该测试用例是否在TResult中出现
			judge = true ;
			for( vector<int*>::const_iterator i = TResult.begin() ; i != TResult.end() ; i++ )
			{
				int pc = 0 ;
				for( int j=0 ; j<sut->parameter ; j++ )
				{
					if( temp[j] == (*i)[j] )
						pc++ ;
				}
				if( pc == sut->parameter )   // 找到相同的
				{
					judge = false ;
					break ;
				}
			}

		}while( !judge );

		TResult.push_back( temp );
		sut->FitnessValue( temp , 1 ) ;
	}
}

// ----------------------------------------------------------------------------
// 使用ART方法生成
// 输入：距离计算方法version，候选测试用例集大小N，
// 输出：测试用例集存入TResult
// ----------------------------------------------------------------------------
void Generator::ART_Method( int version , int N )
{
	//srand( (unsigned int)time(NULL) );

	// 生成S
	sut->GenerateS();
	// 初始化TResult
	clear_TResult();

	int *temp = new int[sut->parameter];

	while( sut->SCount != 0 )
	{
		int best_f = 0  ;  // 最好距离
		int *best_t = new int[sut->parameter] ;  // 相应最好测试用例

		int distance = 0 ;
		
		for( int i=0 ; i<N ; i++ )   // 循环N次
		{
			// 随机生成一个测试用例t
			for( int i=0 ; i<sut->parameter ; i++ )
				temp[i] = (int)(((double)(rand()%1000)/1000.0)*sut->value[i]) ;

			distance = ART_TestFit( temp , version );

			if( distance > best_f )   // 如果
			{
				best_f = distance ;
				// copy temp to best_t
				for( int x=0 ; x<sut->parameter ; x++ )
					best_t[x] = temp[x] ;
			}
		}

		if( best_f != 0 )
		{
			TResult.push_back( best_t );
			sut->FitnessValue( best_t , 1 ) ;

		}
	}

	delete[] temp;
}

// ----------------------------------------------------------------------------
// 计算测试用例test的距离
// version=0 : 返回test能覆盖的新组合个数
// version=1 ：返回test与TResult的海明距离
// ----------------------------------------------------------------------------
int Generator::ART_TestFit( const int *temp , int version )
{
	// 返回test能覆盖的新组合个数
	if( version == 0 )
	{
		int num = sut->FitnessValue( temp , 0 );
		return num ;
	}

	// 返回test与TResult的海明距离
	else if( version == 1 )
	{
		int Hamming = sut->parameter ;
		int count ;
		for( vector<int*>::const_iterator i = TResult.begin() ; i != TResult.end() ; i++ )
		{
			count = 0 ;
			for( int j=0 ; j<sut->parameter ; j++ )
			{
				if( temp[j] != (*i)[j] )
					count++ ;
			}
			if( count < Hamming )
				Hamming = count ;
		}
		return Hamming ;
	}
}


// ----------------------------------------------------------------------------
// 输出
// ----------------------------------------------------------------------------
void Generator::testprint_TR()
{
	for( vector<int*>::const_iterator i = TResult.begin() ; i != TResult.end() ; i++ )
	{
		for( int k=0 ; k<sut->parameter ; k++ )
			cout<<(*i)[k]<<" ";
		cout<<endl;
	}
	cout<<"size: "<<TResult.size()<<endl;
}

void Generator::testprint_firstweight()
{
	cout<<"【firstweigth】"<<endl;
	for( int i=0 ; i<sut->parameter ; i++ )
	{
		for( int j=0 ; j<sut->value[i]+1 ; j++ )
			cout<<firstweight[i][j]<<" ";
		cout<<endl;
	}
}