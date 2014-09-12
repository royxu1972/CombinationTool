#include"Generator.h"
#include<cmath>
#include<iostream>
using namespace std;


// ----------------------------------------------------------------------------
// ��ռ���ʼ��
// ----------------------------------------------------------------------------
void Generator::clear_TResult()
{
	for( vector<int*>::iterator i = TResult.begin() ; i != TResult.end() ; i++ )
		delete[] (*i) ;
	TResult.clear();
}

// ----------------------------------------------------------------------------
// ʹ��CT��������
// ���룺��ѡ������������СN
// �������������������TResult
// ----------------------------------------------------------------------------
void Generator::CT_Method( int N )
{
	// ����S
	sut->GenerateS();

	// firstweight��ʼ��
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

	// TResult��ʼ��
	clear_TResult();

	int *temp ;
	while( sut->SCount != 0 )
	{
		//  ��¼ÿ�ε���ý��
		int *best ;   
		best = new int[sut->parameter] ;
		int ptrcov = 0 ;

		for( int x=0 ; x<N ; x++ )   // ������N����������
		{
			temp = CT_GenerateTestCase();

			// �Ƚ�
			int covered = sut->FitnessValue(temp, 0);
			if( covered > ptrcov )
			{
				ptrcov = covered ;
				for( int x=0 ; x<sut->parameter ; x++ )
					best[x] = temp[x] ;
			}
			else
				delete[] temp ;     // ɾ��temp�ռ�
		}

		// ����������ǿ�ļ���TResult, �޸�firstweight, ����S��ɾȥ�Ѹ��Ƕ�
		TResult.push_back( best );
		SetFirstWeight( best );
		sut->FitnessValue( best , 1 );
	}
}

// ----------------------------------------------------------------------------
// �޸�firstweight����temp��ÿ��tway��Ϸֱ���
// ----------------------------------------------------------------------------
void Generator::SetFirstWeight( const int *temp )
{
	// ���ΰ��ֵ�������temp�ĸ���������
	int *position = new int[sut->tway] ;      // ����������
	int *position_max = new int[sut->tway];   // �������ֵ
	for( int k=0 ; k<sut->tway ; k++ )  // ��ʼ��
	{
		position[k] = k ;
		position_max[k] = sut->parameter - sut->tway + k ;
	}
	int end = sut->tway - 1 ;
	int ptr = end ;

	int *schema = new int[sut->tway] ;  // �洢ȡֵ�����

	for( int x = 0 ; x < cal_combine(sut->parameter,sut->tway) ; x++ )
	{
		// �õ�������
		for( int k=0 ; k<sut->tway ; k++ )
			schema[k] = temp[position[k]] ;
		// ����δ���ǣ���temp��������
		if( !sut->Covered( position , schema , 0 ) )
		{
			for( int y=0 ; y<sut->tway ; y++ )
			{
				firstweight[position[y]][schema[y]+1]--;
				firstweight[position[y]][0]--;
			}
		}
			
		// ����һ�¸�
		position[end] = position[end] + 1 ;  // ĩλ��1
		ptr = end ;
		while( ptr > 0 )
		{
			if( position[ptr] > position_max[ptr] )  // ������λ�������ֵ
			{
				position[ptr-1] = position[ptr-1] + 1 ;   // ǰһλ��1
				ptr-- ;
			}
			else
				break ;
		}
		if( position[ptr] <= position_max[ptr])  // ����λֵ������󣬺���ÿλ��ǰһλ�����ϼ�1
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
// ����һ���µĲ�������
// ----------------------------------------------------------------------------
int *Generator::CT_GenerateTestCase()
{
	int willfit = 0 ;  // �������ܸ��ǵ������

	int *test = new int[sut->parameter];
	for( int p=0 ; p<sut->parameter ; p++ )
		test[p] = -1 ;

	int assigned_count = 0 ;   // ��ȷ����������
	int un_count = sut->parameter ;  // δȷ����������

	int *parameter_order ;  // �洢δȷ���������
	int **weight ;  // �洢��������ȡֵ��Ȩ��

	// ����
	int *p_number ;
	int *p_order ;
	int *v_number ;
	int *v_order ;

	// ����ѡ����
	int maxp = 0 ;
	int maxv = 0 ;

	//
	// ѡ���һ��������ֱ�Ӵ�firstweight�м���
	//
	
	// �ҵ�һ�����Ĳ���
	p_number = new int[sut->parameter];
	p_order = new int[sut->parameter];
	for( int x=0 ; x<sut->parameter ; x++ )
	{
		p_order[x] = x ;
		p_number[x] = firstweight[x][0] ;
	}
	// ����
	cal_sort( p_number , p_order , 0 , un_count-1 , 1 );
	// ͬ����
	int p_equal_judge = 1 ;
	int p_equal_number = p_number[0] ;
	for( int l=1 ; l<sut->parameter ; l++ )
	{
		if( p_number[l] == p_equal_number )
			p_equal_judge++;
		else
			break;
	}
	// ����ֵͬ�Ĳ��������������ȡһ��ֵ��Ϊ����ֵ
	if( p_equal_judge > 1 )   
	{
		int tt = rand()%p_equal_judge ;
		maxp = p_order[tt] ;
	}
	else
		maxp = p_order[0] ;
	
	delete[] p_number;
	delete[] p_order ;

	// �ҵ�һ�����ȡֵ
	int col = sut->value[maxp] ;
	v_number = new int[col];
	v_order = new int[col];

	for( int x=0 ; x<col ; x++ )
	{
		v_order[x] = x ;
		v_number[x] = firstweight[maxp][x+1] ;
	}
	// ����
	cal_sort( v_number , v_order , 0 , col-1 , 1 );
	// ͬ����
	int v_equal_judge = 1 ;
	int v_equal_number = v_number[0] ;
	for( int l=1 ; l<col ; l++ )
	{
		if( v_number[l] == v_equal_number )
			v_equal_judge++;
		else
			break;
	}
	// ����ֵͬ�Ĳ��������������ȡһ��ֵ��Ϊ����ֵ
	if( v_equal_judge > 1 )   
	{
		int tt = rand()%v_equal_judge ;
		maxv = v_order[tt] ;
	}
	else
		maxv = v_order[0] ;
	
	delete[] v_number ;
	delete[] v_order  ;


	// ����test
	test[maxp] = maxv ;

	// ����
	assigned_count++;
	un_count--;
	
	// ��һ������ѡ����� //

	while( assigned_count < sut->parameter )
	{
		//
		// �����µ�weight
		//

		// �ҵ�δȷ���Ĳ������
		parameter_order = new int[un_count] ;
		for( int x=0 , y=0 ; x<sut->parameter , y<un_count ; x++ )   
		{
			if( test[x] == -1 )
			{
				parameter_order[y] = x ;
				y++ ;
			}
		}

		// �����µĶ�ά���飬��ͬʱ����weight
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

		// �ҵ�һ�����Ĳ���
		int *p_number = new int[un_count];
		int *p_order = new int[un_count];

		for( int x=0 ; x<un_count ; x++ )
		{
			p_order[x] = weight[x][0] ;
			p_number[x] = weight[x][1] ;
		}
		// ����
		cal_sort( p_number , p_order , 0 , un_count-1 , 1 );
		// ͬ����
		int p_equal_judge = 1 ;
		int p_equal_number = p_number[0] ;
		for( int l=1 ; l<un_count ; l++ )
		{
			if( p_number[l] == p_equal_number )
				p_equal_judge++;
			else
				break;
		}
		// ����ֵͬ�Ĳ��������������ȡһ��ֵ��Ϊ����ֵ
		if( p_equal_judge > 1 )   
		{
			int tt = rand()%p_equal_judge ;
			maxp = p_order[tt] ;
		}
		else
			maxp = p_order[0] ;

		delete[] p_number;
		delete[] p_order ;

		// �ҵ�һ�����ȡֵ
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

		// ����
		cal_sort( v_number , v_order , 0 , col-1 , 1 );
		// ͬ����
		int v_equal_judge = 1 ;
		int v_equal_number = v_number[0] ;
		for( int l=1 ; l<col ; l++ )
		{
			if( v_number[l] == v_equal_number )
				v_equal_judge++;
			else
				break;
		}
		// ����ֵͬ�Ĳ��������������ȡһ��ֵ��Ϊ����ֵ
		if( v_equal_judge > 1 )   
		{
			int tt = rand()%v_equal_judge ;
			maxv = v_order[tt] ;
		}
		else
			maxv = v_order[0] ;

		delete[] v_number ;
		delete[] v_order  ;

		// ����test
		test[maxp] = maxv ;

		// ���� test fit
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
// ���룺temp_test[parameter]��count-1λǰ��ȷ��Ϊfixed value��Ŀǰ���ǵ�count���������������n������ȡֵv
// �������temp_test��S�еĸ�����
// ----------------------------------------------------------------------------
int Generator::CT_FitSchema( const int *temp_test , const int n , const int v , const int count )
{
	int fit = 0 ;  // ����ֵ

	int *test = new int[sut->parameter];
	for( int k=0 ; k<sut->parameter ; k++ )
		test[k] = temp_test[k] ;
	
	//
	// ��ʱ����n��ֵv�����룬��ȷ���Ĳ�������countС��tway����δȷ���Ĳ�����ѡ�����е�tway-count����������������
	// ����ÿһ����������������¸��漰����������ȡֵ��Ȼ������һ��position_1��schema_1��ʹ��Covered�������������Ƿ񸲸�
	//  
	if( count < sut->tway )
	{
		// ��v����test[]
	    test[n] = v ;

		int *position = new int[sut->tway];
		int *schema = new int[sut->tway];

		int *position_1 = new int[sut->tway] ;
		int *schema_1 = new int[sut->tway] ;

		// ��ȷ��count������
		// δȷ��parameter - count������
		int un_count = sut->parameter - count ;
		int *un_pos = new int[un_count] ;

		// ��ֵ
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
		
		// ��un��ѡ��tway-count������position���һ��position��schema��Ȼ������Ƿ�ɸ���
		int need_choose = sut->tway - count ;
		int allrow = cal_combine( un_count , need_choose );

		// ���ֵ�����������������
		int *temp = new int[need_choose] ;      // ������¼���洢λ�ã���Ӧun_pos
		int *un_value = new int[need_choose];   // ������¼����ȡֵ
		int *temp_max = new int[need_choose];   // �������ֵ
		for( int k=0 ; k<need_choose ; k++ )   // ��ʼ��
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
			// �Ը�λ�õĸ�ȡֵ 
			int allcomb = 1 ;   // �����
			for( int p=0 ; p<need_choose ; p++ )
				allcomb = allcomb * sut->value[un_pos[temp[p]]] ;

			// ������¼Ϊun_value
			for( int k=0 ; k<need_choose ; k++ )
				un_value[k] = 0 ;
			int cend = need_choose - 1 ;         // ���ÿ�μ�1��λ�ã�����ĩβ
			int cptr = cend ;             // ������ν�λ          

			// ���������
			for( int k = 0 ; k < allcomb ; k++ )
			{
				// ����position
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

				// position_1��schema_1����
				cal_sort( position_1 , schema_1 , 0 , sut->tway-1 , 0 );

				// �ж��Ƿ񸲸�
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

			// ����һ�����
			temp[end] = temp[end] + 1 ;  // ĩλ��1
			ptr = end ;
			while( ptr > 0 )
			{
				if( temp[ptr] > temp_max[ptr] )  // ������λ�������ֵ
				{
					temp[ptr-1] = temp[ptr-1] + 1 ;   // ǰһλ��1
					ptr-- ;
				}
				else
					break ;
			}
			if( temp[ptr] <= temp_max[ptr])  // ����λֵ������󣬺���ÿλ��ǰһλ�����ϼ�1
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
	// ��ʱ��ȷ����������count>=tway����ˣ�����ȷ���Ĳ�����ѡ�����е�tway-1��������
	// ��ÿһ������������n��ֵv����position��schema��ʹ��Covered�������㸲����
	//
	else
	{
		int *position = new int[sut->tway];
		int *schema = new int[sut->tway];

		// ��ȷ����������Ϊcount-1
		int al_count = count - 1 ;
		int *al_pos = new int[al_count] ;
		int *al_value = new int[al_count] ;

		// ��ֵ
		for( int k=0 , p=0 ; k<sut->parameter ; k++ )
		{
			if( test[k] != -1 )
			{
				al_pos[p] = k ;
				al_value[p] = test[k] ;
				p++ ;
			}
		}

		// ����ȷ����al_count��������ѡneed_choose������
		int need_choose = sut->tway - 1 ;
		int allrow = cal_combine( al_count , need_choose );

		// ���ֵ�����������������
		int *temp = new int[need_choose] ;      // ������¼��λ�ã���Ӧun_pos
		int *temp_max = new int[need_choose];  // �������ֵ
		for( int k=0 ; k<need_choose ; k++ )  // ��ʼ��
		{
			temp[k] = k ;
			temp_max[k] = al_count - need_choose + k ;
		}
		int end = need_choose - 1 ;
		int ptr = end ;

		int already = 0 ;
		while( already < allrow )
		{
			// ��ʱtemp[]����������δal_pos[]���±꣬����ȡ������position��schema��
			for( int w=0 ; w<sut->tway-1 ; w++ )
			{
				position[w] = al_pos[temp[w]] ;
				schema[w] = al_value[temp[w]] ;
			}
			// �������n
			position[sut->tway-1] = n ;
			schema[sut->tway-1] = v ;

			// ����
			cal_sort( position , schema , 0 , sut->tway-1 , 0 );
			// �ж��Ƿ񸲸�
			if( !sut->Covered( position , schema , 0 ) )
				fit++ ;

			// ����һ�����
			temp[end] = temp[end] + 1 ;  // ĩλ��1
			ptr = end ;
			while( ptr > 0 )
			{
				if( temp[ptr] > temp_max[ptr] )  // ������λ�������ֵ
				{
					temp[ptr-1] = temp[ptr-1] + 1 ;   // ǰһλ��1
					ptr-- ;
				}
				else
					break ;
			}
			if( temp[ptr] <= temp_max[ptr])  // ����λֵ������󣬺���ÿλ��ǰһλ�����ϼ�1
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
// ʹ��RT��������
// �������������������TResult
// ----------------------------------------------------------------------------
void Generator::RT_Method()
{
	//srand( (unsigned int)time(NULL) );

	// ����S
	sut->GenerateS();
	// ��ʼ��TResult
	clear_TResult();

	while( sut->SCount != 0 )
	{
		bool judge = true ;

		int *temp = new int[sut->parameter];

		do {
			// �������һ����������t
			for( int i=0 ; i<sut->parameter ; i++ )
				temp[i] = (int)(((double)(rand()%1000)/1000.0)*sut->value[i]) ;

			// �жϸò��������Ƿ���TResult�г���
			judge = true ;
			for( vector<int*>::const_iterator i = TResult.begin() ; i != TResult.end() ; i++ )
			{
				int pc = 0 ;
				for( int j=0 ; j<sut->parameter ; j++ )
				{
					if( temp[j] == (*i)[j] )
						pc++ ;
				}
				if( pc == sut->parameter )   // �ҵ���ͬ��
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
// ʹ��ART��������
// ���룺������㷽��version����ѡ������������СN��
// �������������������TResult
// ----------------------------------------------------------------------------
void Generator::ART_Method( int version , int N )
{
	//srand( (unsigned int)time(NULL) );

	// ����S
	sut->GenerateS();
	// ��ʼ��TResult
	clear_TResult();

	int *temp = new int[sut->parameter];

	while( sut->SCount != 0 )
	{
		int best_f = 0  ;  // ��þ���
		int *best_t = new int[sut->parameter] ;  // ��Ӧ��ò�������

		int distance = 0 ;
		
		for( int i=0 ; i<N ; i++ )   // ѭ��N��
		{
			// �������һ����������t
			for( int i=0 ; i<sut->parameter ; i++ )
				temp[i] = (int)(((double)(rand()%1000)/1000.0)*sut->value[i]) ;

			distance = ART_TestFit( temp , version );

			if( distance > best_f )   // ���
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
// �����������test�ľ���
// version=0 : ����test�ܸ��ǵ�����ϸ���
// version=1 ������test��TResult�ĺ�������
// ----------------------------------------------------------------------------
int Generator::ART_TestFit( const int *temp , int version )
{
	// ����test�ܸ��ǵ�����ϸ���
	if( version == 0 )
	{
		int num = sut->FitnessValue( temp , 0 );
		return num ;
	}

	// ����test��TResult�ĺ�������
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
// ���
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
	cout<<"��firstweigth��"<<endl;
	for( int i=0 ; i<sut->parameter ; i++ )
	{
		for( int j=0 ; j<sut->value[i]+1 ; j++ )
			cout<<firstweight[i][j]<<" ";
		cout<<endl;
	}
}