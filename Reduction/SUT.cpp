#include"SUT.h"
#include<iostream>
using namespace std ;

// ----------------------------------------------------------------------------
// SUT��ʼ��
// ----------------------------------------------------------------------------
// �Ӹ���
void SUT::SetSub( int way , int par , const int *pos )
{
	SUBINFO temp ;
	temp.subparameter = par ;
	temp.subway = way ;
	temp.subposition = new int[temp.subparameter] ;
	for( int k=0 ; k<temp.subparameter ; k++ )
		temp.subposition[k] = pos[k] ;
	temp.coverIndex = coverMax ;
	temp.coverSub = cal_combine( temp.subparameter , temp.subway );
	
	coverMax += temp.coverSub ;
	testcaseCoverMax = coverMax ;

	subInfo.push_back(temp);
}

// ����
void SUT::SetSeed( const vector<int*> seed )
{
	for( vector<int*>::const_iterator i = seed.begin() ; i != seed.end() ; i++ )
	{
		int *temp = new int[parameter] ;
		for( int k=0 ; k<parameter ; k++ )
			temp[k] = (*i)[k] ;
		seedInfo.push_back(temp);
	}
}

// ����Ԥ����
void SUT::preforseed()
{
	for( vector<int*>::const_iterator i = seedInfo.begin() ; i != seedInfo.end() ; i++ )
		FitnessValue((*i),1);
}


// ----------------------------------------------------------------------------
// ����AllS�ռ䣬������
// ----------------------------------------------------------------------------
void SUT::GenerateS()
{
	// ��ʼ��
	if( AllS != NULL )
	{
		for( int i=0 ; i<coverMax ; i++ )
			delete[] AllS[i];
		delete[] AllS;
		AllS = NULL;
	}
	SCount = 0 ;

	// ����coverMax��
	AllS = new BYTE*[coverMax] ;

	// ���ֵ����������в������������
	int *temp = new int[tway] ;      // ������¼
	int *temp_max = new int[tway];   // �������ֵ
	for( int k=0 ; k<tway ; k++ )  // ��ʼ��
	{
		temp[k] = k ;
		temp_max[k] = parameter - tway + k ;
	}
	int end = tway - 1 ;
	int ptr = end ;

	int already = 0 ;
	while( already < coverMain )
	{
		//
		// ����temp����ʱtemp[]����˲������
		//

		// �����漰��������ϸ���
		int allcomb = 1 ;
		for( int p=0 ; p<tway ; p++ )
			allcomb = allcomb * value[temp[p]] ;
		// �����µ�1��
		int column = (int)ceil((double)allcomb/(double)8) ; 
		AllS[already] = new BYTE[column];
		// ��ʼ����ȫΪ0
		for( int k=0 ; k<column ; k++ )
			AllS[already][k] = 0x00 ;

		// �����ܵ���ϸ���
		SCount += allcomb ;

		//
		// ����һ�����
		//
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
			for( int i=ptr+1 ; i<tway ; i++ ) 
				temp[i] = temp[i-1] + 1 ;
		}
		already++ ;
	}

	delete[] temp ;
	delete[] temp_max ;

	// �����Ӹ���
	if( subInfo.size() != 0 )
		GenerateS_Sub();

	// ��������
	if( seedInfo.size() !=0 )
		preforseed();

	// ��¼�ܵĴ����������
	SCountAll = SCount ;
}

// Sub����
void SUT::GenerateS_Sub()
{
	// ��ÿ��sub
	for( vector<SUBINFO>::iterator i = subInfo.begin() ; i != subInfo.end() ; i++ )
	{
		int subway = (*i).subway ;
		int subparameter = (*i).subparameter ;
		int *subposition = (*i).subposition ;

		// ���ֵ����������в������������
		int *subtemp = new int[subway] ;      // ������¼
		int *subtemp_max = new int[subway];   // �������ֵ
		for( int k=0 ; k<subway ; k++ )  // ��ʼ��
		{
			subtemp[k] = k ;
			subtemp_max[k] = subparameter - subway + k ;
		}

		int subend = subway - 1 ;
		int subptr = subend ;

		int subalready = 0 ;
		int subrow = (*i).coverIndex ;
		while( subalready < (*i).coverSub )
		{
			//
			// ����subtemp
			// subtemp[]�ǰ�[0,1,...]��ŵģ�subtemp[]�ĸ�ֵ������subposition[]�е�λ��
			//

			// �����漰��������ϸ���
			int suballcomb = 1 ;
			for( int p=0 ; p<subway ; p++ )
				suballcomb = suballcomb * value[subposition[subtemp[p]]] ;
			// �����µ�1��
			int subcolumn = (int)ceil((double)suballcomb/(double)8) ; 
			AllS[subrow] = new BYTE[subcolumn];
			// ��ʼ����ȫΪ0
			for( int k=0 ; k<subcolumn ; k++ )
				AllS[subrow][k] = 0x00 ;

			// �����ܵ���ϸ���
			SCount += suballcomb ;

			//
			// ����һ�����
			//
			subtemp[subend] = subtemp[subend] + 1 ;  // ĩλ��1
			subptr = subend ;
			while( subptr > 0 )
			{
				if( subtemp[subptr] > subtemp_max[subptr] )  // ������λ�������ֵ
				{
					subtemp[subptr-1] = subtemp[subptr-1] + 1 ;   // ǰһλ��1
					subptr-- ;
				}
				else
					break ;
			}
			if( subtemp[subptr] <= subtemp_max[subptr])  // ����λֵ������󣬺���ÿλ��ǰһλ�����ϼ�1
			{
				for( int i=subptr+1 ; i<subway ; i++ ) 
					subtemp[i] = subtemp[i-1] + 1 ;
			}
			// row++
			subrow++;
			subalready++ ;
		}
		delete[] subtemp ;
		delete[] subtemp_max ;

	} // end for each sub
}

// ----------------------------------------------------------------------------
// �����������test��δ������϶Լ�S���ܸ��ǵ���϶���
// ���FLAG=0ֻ���㲻�޸ģ�FLAG=1�򽫸��ǵ�����������Ϊ�Ѹ���
// ���룺һ����������test�����FLAG
// ��������������
// ----------------------------------------------------------------------------
int SUT::FitnessValue( const int *test , int FLAG )
{
	int num = 0 ;   // ����ֵ

	// ���ΰ��ֵ�������test�ĸ�����������Ȼ���AllS�����ж��Ƿ񸲸ǣ�O(C(par,tway))

	int *pos = new int[tway] ;      // ����������
	int *pos_max = new int[tway];   // �������ֵ
	for( int k=0 ; k<tway ; k++ )  // ��ʼ��
	{
		pos[k] = k ;
		pos_max[k] = parameter - tway + k ;
	}
	int end = tway - 1 ;
	int ptr = end ;

	int *sch = new int[tway] ;  // �洢ȡֵ�����

	for( int row = 0 ; row < coverMain ; row++ )
	{
		// �õ�������
		for( int k=0 ; k<tway ; k++ )
			sch[k] = test[pos[k]] ;
		// �ж��Ƿ񸲸�
		if( !Covered( pos , sch , FLAG ) )
			num++ ;

		// ����һ�¸�
		pos[end] = pos[end] + 1 ;  // ĩλ��1
		ptr = end ;
		while( ptr > 0 )
		{
			if( pos[ptr] > pos_max[ptr] )  // ������λ�������ֵ
			{
				pos[ptr-1] = pos[ptr-1] + 1 ;   // ǰһλ��1
				ptr-- ;
			}
			else
				break ;
		}
		if( pos[ptr] <= pos_max[ptr])  // ����λֵ������󣬺���ÿλ��ǰһλ�����ϼ�1
		{
			for( int i=ptr+1 ; i<tway ; i++ ) 
				pos[i] = pos[i-1] + 1 ;
		}
	}
	delete[] pos;
	delete[] pos_max;
	delete[] sch;

	if( subInfo.size() !=  0 )
		num += FitnessValue_Sub( test , FLAG );

	return num ;
}

// Sub����
int SUT::FitnessValue_Sub( const int *test , int FLAG )
{
	int subnum = 0 ;   // ����ֵ

	// ��ÿ��Sub
	for( vector<SUBINFO>::iterator i = subInfo.begin() ; i != subInfo.end() ; i++ )
	{
		int subway = (*i).subway ;
		int subparameter = (*i).subparameter ;
		int *subposition = (*i).subposition ;
		// ���ΰ��ֵ�������test�ĸ�����������Ȼ���AllS�����ж��Ƿ񸲸ǣ�O(C(subpar,subway))

		int *subpos = new int[subway] ;      // ����������
		int *subpos_max = new int[subway];   // �������ֵ
		for( int k=0 ; k<subway ; k++ )  // ��ʼ��
		{
			subpos[k] = k ;
			subpos_max[k] = subparameter - subway + k ;
		}
		int subend = subway - 1 ;
		int subptr = subend ;

		int *subsch = new int[subway] ;  // �洢ȡֵ�����

		for( int row = 0 ; row < (*i).coverSub ; row++ )
		{
			// �õ�������
			for( int k=0 ; k<subway ; k++ )
				subsch[k] = test[subposition[subpos[k]]] ;

			// �ж��Ƿ񸲸�
			if( !Covered( subpos , subsch , i , FLAG ) )
				subnum++ ;

			// ����һ�¸�
			subpos[subend] = subpos[subend] + 1 ;  // ĩλ��1
			subptr = subend ;
			while( subptr > 0 )
			{
				if( subpos[subptr] > subpos_max[subptr] )  // ������λ�������ֵ
				{
					subpos[subptr-1] = subpos[subptr-1] + 1 ;   // ǰһλ��1
					subptr-- ;
				}
				else
					break ;
			}
			if( subpos[subptr] <= subpos_max[subptr])  // ����λֵ������󣬺���ÿλ��ǰһλ�����ϼ�1
			{
				for( int i=subptr+1 ; i<subway ; i++ ) 
					subpos[i] = subpos[i-1] + 1 ;
			}
		}
		delete[] subpos;
		delete[] subpos_max;
		delete[] subsch;
	}

	return subnum ;
}

// ----------------------------------------------------------------------------
// ����һ��ȡֵ����Ƿ񸲸ǣ�����pos�洢�����ֵ���ţ�sch�洢��Ӧȡֵ
// ���룺pos[tway]��sch[tway]��FLAG=1���ʾδ����ʱ��Ҫ����λ��1
// �����trueΪ�Ѹ��ǣ�falseΪδ����
// ----------------------------------------------------------------------------
bool SUT::Covered( const int *pos , const int *sch , int FLAG )
{
	// ͨ��position����õ��кţ�schema����õ��кţ���AllS�ó�������ӽ�O(1)
	bool ret = true ;

	// �洢������Ϣ
	int row = cal_combine2num( pos , parameter , tway );
	int column = 0 ;       // ��һ��BYTE
	int column_bit = 0 ;   // ��һ��bit

	// ����column���±��0��ʼ����
	int Index = 0 ;
	int it = 0 ;
	for( int i=0 ; i<tway ; i++ )
	{
		it = sch[i] ;
		for( int j=i+1 ; j<tway ; j++ )
			it = value[pos[j]] * it ;
		Index += it ;
	}

	column = Index / 8 ;
	column_bit = Index % 8 ;
	
	// �ж�column_bitλ�Ƿ�Ϊ0
	// index : 0 1 2 3 4 5 6 7
	// BYTE  : 0 0 0 0 0 0 0 0
	//                 |
	//             column_bit
	BYTE byte = AllS[row][column] ;
	if( !( byte >> ( 7-column_bit ) & 0x01 ) )
	{
		ret = false ;
		if( FLAG == 1 )
		{
			AllS[row][column] = AllS[row][column] | (0x01<<(7-column_bit)) ;
			SCount--;
		}
	}
	return ret ;
}

// Sub����
// ����pos�洢�Ӹ����ж�Ӧ�ֵ���ţ���ʵ����λ����sub.subposition�У�sch�洢��Ӧȡֵ
bool SUT::Covered( const int *pos , const int *sch , vector<SUBINFO>::const_iterator sub , int FLAG )
{
	int subway = (*sub).subway ;
	int subparameter = (*sub).subparameter ;
	int *subposition = (*sub).subposition ;

	// ͨ��position����õ��кţ�schema����õ��кţ���AllS�ó����
	bool subret = true ;

	// �洢������Ϣ
	int subrow = (*sub).coverIndex ;
	int subcolumn = 0 ;       // ��һ��BYTE
	int subcolumn_bit = 0 ;   // ��һ��bit

	// ����row���±��0��ʼ����
	subrow += cal_combine2num( pos , subparameter , subway );

	// ����column���±��0��ʼ����
	int subIndex = 0 ;
	int it = 0 ;
	for( int i=0 ; i<subway ; i++ )
	{
		it = sch[i] ;
		for( int j=i+1 ; j<subway ; j++ )
			it = value[subposition[pos[j]]] * it ;
		subIndex += it ;
	}

	subcolumn = subIndex / 8 ;
	subcolumn_bit = subIndex % 8 ;

	// �ж�column_bitλ�Ƿ�Ϊ0
	BYTE byte = AllS[subrow][subcolumn] ;
	if( !( byte>>(7-subcolumn_bit) & 0x01 ) )
	{
		subret = false ;
		if( FLAG == 1 )
		{
			AllS[subrow][subcolumn] = AllS[subrow][subcolumn] | (0x01<<(7-subcolumn_bit)) ;
			SCount--;
		}
	}
	return subret ;
}

// ----------------------------------------------------------------------------
// ���AllS
// ----------------------------------------------------------------------------
void SUT::printAllS()
{
	int *p = new int[tway] ;
	for( int i=0 ; i<coverMax ; i++ )
	{
		cal_num2combine(  p , i , parameter , tway );
		cout<<"{ ";
		for( int m=0 ; m<tway ; m++ )
			cout<<p[m]<<" ";
		cout<<"} : ";

		int allcomb = 1 ;
		for( int q=0 ; q<tway ; q++ )
			allcomb = allcomb * value[p[q]] ;
		int column = (int)ceil((double)allcomb/(double)8) ; 

		int out = 0 ;
		for( int column_index = 0 ; column_index < column ; column_index++ )
		{
			BYTE ac = AllS[i][column_index] ;
			for( int c = 0 ; c < 8 && out < allcomb ; c++ )
			{
				// ѭ������һλ
				BYTE b = ac >> 7 ;
				ac = ac << 1 ;
				ac = ac | b ;
				if( ac & 0x01 )
					cout<<"1 ";
				else
					cout<<"0 ";
				out++ ;
			}
		}
		cout<<endl;	
	}
	delete[] p;
}


// ----------------------------------------------------------------------------
// check coverage
// input: CA , number of row , t-way
// output: t-way coverage
// ----------------------------------------------------------------------------
double SUT::CheckCoverage( int **ca , int r , int t )
{
	tway = t ;
	GenerateS();
	for( int x = 0 ; x < r ; x++ )
		FitnessValue( ca[x] , 1 );

	return (double)(SCountAll-SCount) / (double)(SCountAll) ;
}