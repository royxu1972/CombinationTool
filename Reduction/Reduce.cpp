#include"Reduce.h"
#include<set>
#include<iomanip>
using namespace std ;

// cal index of t-way combination
int Reduce::cal_index( const int *pos , const int *val  )
{
	int com = 1 ;
	int ret = 0 ;
	for( int k=tway-1 ; k>=0 ; k-- )
	{
		ret += com * val[k] ;
		com = com * value[pos[k]] ;
	}
	return ret ;
}

/*  read from file
 *	PARAMETER: filename - CA file name, specific format required
 *			   all - the number of orginal rows
 *			   p - whether to print the statistic result
 */
int Reduce::ReadFile( string filename , int all , bool p)
{
	ifstream file ;
	file.open( filename );

	string str ;
	if( file.is_open() ) 
	{
		// read first line
		file>>str ;
		parameter = atoi( str.c_str() );
		value = new int[parameter] ;

		for( int k=0 ; k<parameter; k++ ) 
		{
			file>>str ;
			value[k] = atoi( str.c_str() );
		}

		file>>str ;
		tway = atoi( str.c_str() );
		
		file>>str ;
		row = atoi( str.c_str() );

		// read CA
		CArray = new int*[row] ;
		for( int i=0 ; i<row; i++ )
		{
			CArray[i] = new int[parameter] ;
			for( int j=0 ; j<parameter ; j++ )
			{
				file>>str ;
				if( str == "-" )
				//if( atoi( str.c_str() ) == -1 )
					CArray[i][j] = rand()%value[j] ;
				else
					CArray[i][j] = atoi( str.c_str() );
			}
		}
	} 
	else
	{
		cout<<"open file "<<filename<<" error"<<endl; 
		return -1 ;
	}
	
	file.close(); 

	all_iteration = all ;
	print = p ;
	return 1 ;
}

/*  read from generation result
 *	PARAMETER: par - the number of parameters
 *			   v[] - the number of each values
 *			   t - covering strength
 *			   T - the covering array
 *			   all - the number of orginal rows
 *			   p - whether to print the statistic result
 */
int Reduce::ReadVector( const int par , const int *v , const int t , const vector<int*> T , int all , bool p )
{
	parameter = par ;
	tway = t ;
	value = new int[par];
	for( int k=0 ; k<par ; k++ )
		value[k] = v[k] ;

	row = T.size();

	CArray = new int*[row] ;
	vector<int*>::const_iterator index = T.begin() ;
	for( int i=0 ; i<row ; i++ , index++ )
	{
		CArray[i] = new int[parameter] ;
		for( int j=0 ; j<parameter; j++ )
			CArray[i][j] = (*index)[j] ;
	}

	all_iteration = all ;
	print = p ;
	return 1 ;
}

// find possible care position representing as *
bool Reduce::ReorderCArray()
{
	//if( print )
	//	testprint( CArray , row , "[Begin] original covering array" );

	int **tempCA = new int*[row] ;
	for( int i=0 ; i<row ; i++ )
	{
		tempCA[i] = new int[parameter] ;
		for( int j=0 ; j<parameter ; j++ )
			tempCA[i][j] = -1 ;
	}

	// the number of * in each row
	int *count = new int[row] ;
	for( int k=0 ; k<row ; k++ )
		count[k] = parameter ;

	// sub column
	int *temp = new int[tway] ;      // index for each sub column representing positions
	int *temp_max = new int[tway];   // the maximum values for the position
	for( int k=0 ; k<tway ; k++ )    // initializarion
	{
		temp[k] = k ;
		temp_max[k] = parameter - tway + k ;
	}
	int end = tway - 1 ;
	int ptr = end ;

	// for each sub column ( C(par,tway) in total )
	int *val = new int[tway];
	int allsubcol = cal_combine(parameter, tway) ;
	for( int i = 0 ; i < allsubcol ; i++ )
	{
		int subcomb = 1 ;
		for( int p=0 ; p<tway ; p++ )
			subcomb = subcomb * value[temp[p]] ;

		int *cover = new int[subcomb] ;     // the number of apperances of each combination
		int *cover_pos = new int[subcomb] ;
		for( int k=0 ; k<subcomb ; k++ )
		{
			cover[k] = 0 ;
			cover_pos[k] = -1 ;
		}

		// for each row of tempCA
		for( int j=0 ; j<row ; j++ )
		{
			for( int k=0 ; k<tway ; k++ )
				val[k] = CArray[j][temp[k]];

			int index = cal_index(temp, val);
			cover[index]++ ;
			if( cover[index] == 1 )
				cover_pos[index] = j ;   // the row makes combination appear first
		}

		/*
		cout<<"for subcol "<<i<<endl;
		for( int k=0 ; k<subcomb ; k++ )
			cout<<cover[k]<<" ";
		cout<<endl;
		for( int k=0 ; k<subcomb ; k++ )
			cout<<cover_pos[k]<<" ";
		cout<<endl;
		*/

		for( int k=0 ; k<subcomb ; k++ )
		{
			if( cover[k] == 1 )   // this combination totally appears once
			{
				for( int n=0 ; n<tway ; n++ )   // set to original values
				{
					if( tempCA[cover_pos[k]][temp[n]] == -1 )
					{
						tempCA[cover_pos[k]][temp[n]] = CArray[cover_pos[k]][temp[n]] ;
						count[cover_pos[k]]-- ;
					}
				}
			}
		}

		map<int,int>::iterator iter ;
		for( int k=0 ; k<subcomb ; k++ )
		{
			iter = comb_count.find( cover[k] );
			if( iter != comb_count.end() )
				iter->second = iter->second + 1 ;
			else
				comb_count.insert( pair<int,int>(cover[k], 1) );
		}

		delete[] cover ;
		delete[] cover_pos ;

		// next sub column
		temp[end] = temp[end] + 1 ;  // adding 1 to the last position
		ptr = end ;
		while( ptr > 0 )
		{
			if( temp[ptr] > temp_max[ptr] )  // exceed the maximum value
			{
				temp[ptr-1] = temp[ptr-1] + 1 ;   // adding 1 to the previous position
				ptr-- ;
			}
			else
				break ;
		}
		if( temp[ptr] <= temp_max[ptr])  // if the value of this position is not the maximum, adding 1 to the follows based on the previous value
		{
			for( int i=ptr+1 ; i<tway ; i++ ) 
				temp[i] = temp[i-1] + 1 ;
		}

	}
	delete[] val;
	delete[] temp ;
	delete[] temp_max ;

	// set PossCare
	for( int k=0 ; k<row ; k++ )
		PossCare += count[k] ;
	if( iteration == 0 )
		Original_PossCare = PossCare ;
	
	//if( print )
	//	testprint( tempCA , row , "[tempCA] possible care position" );

	// re-order CArray
	//cal_sortArray(count, CArray , 0 , row-1 , parameter);
	
	/*  
	 *
	 *  new operation
	 *
	 */
	// Re-order CArray and tempCA
	int *temp_count = new int[row] ;
	for( int k=0 ; k<row ; k++ )
		temp_count[k] = count[k] ;
	cal_sortArray(count, CArray , 0 , row-1 , parameter);
	cal_sortArray(temp_count, tempCA , 0 , row-1 , parameter);
	delete[] temp_count ;
	Last_Care = count[row-1] ;

	if( print ) 
	{
		testprint( CArray , row , "[CA] covering array after reorder" );
		testprint( tempCA , row , "[CA] tempCA after reorder" );
	}

	// save original tempCA
	int **tmp_tempCA = new int*[row] ;
	for( int i=0 ; i<row ; i++ )
	{
		tmp_tempCA[i] = new int[parameter] ;
		for( int j=0 ; j<parameter ; j++ )
			tmp_tempCA[i][j] = tempCA[i][j] ;
	}

	// the first row that has *
	int begin_star = 0 ;
	for( ; count[begin_star] == 0 ; begin_star++ ) ;

	// these rows have been replaced
	set<int> beenReplace ;

	// if the last row has some fixed values, then find a possible previous position to replace
	if( count[row-1] != parameter && count[row-1] != 0 )
	{
		// sub column
		temp = new int[tway] ;
		temp_max = new int[tway] ;
		for( int k=0 ; k<tway ; k++ )
		{
			temp[k] = k ;
			temp_max[k] = parameter - tway + k ;
		}
		end = tway - 1 ;
		ptr = end ;

		// for each sub column of the last row
		val = new int[tway];
		for( int i = 0 ; i < allsubcol ; i++ )
		{
			// whether it has fixed values
			bool realcomb = true ;
			for( int k=0 ; k<tway ; k++ )
			{
				val[k] = tempCA[row-1][temp[k]] ;
				if( val[k] == -1 )
				{
					realcomb = false ;
					break ;
				}
			}

			// search from row begin_star, find a possible position
			if( realcomb )
			{
				for( int j = begin_star ; j < row - 1 ; j++ )
				{
					realcomb = true ;
					for( int k=0 ; k<tway ; k++ )
					{
						if( tempCA[j][temp[k]] != -1 && tempCA[j][temp[k]] != val[k] )
							realcomb = false ;
					}

					// replace *
					if( realcomb )
					{
						// the position can be replaced if and only if 
						// any values is not same with all rows in beenReplace
						// current row : j , position : temp[]
						bool canreplace = true ;
						for( set<int>::const_iterator st = beenReplace.begin() ; st != beenReplace.end() ; st++ )
						{
							for( int k=0 ; k<tway ; k++ )
							{
								// not consider condition
								// 1) current row and replaced row have the same original real(not *) value 
								// 2) current row is the replaced row 
								if( tmp_tempCA[j][temp[k]] != -1 && tmp_tempCA[(*st)][temp[k]] != -1 
									&& CArray[j][temp[k]] == CArray[(*st)][temp[k]] || j == (*st) )
									continue ;
								
								// false condition
								if( CArray[j][temp[k]] == CArray[(*st)][temp[k]] )
									canreplace = false ;
							}
						}

						if( canreplace )
						{
							for( int k=0 ; k<tway ; k++ )
								tempCA[j][temp[k]] = val[k] ;
							beenReplace.insert(j);
							break ;
						}
					}
				} // end for begin_star

			}  // end if realcomb

			// next sub column
			temp[end] = temp[end] + 1 ;
			ptr = end ;
			while( ptr > 0 )
			{
				if( temp[ptr] > temp_max[ptr] )
				{
					temp[ptr-1] = temp[ptr-1] + 1 ;
					ptr-- ;
				}
				else
					break ;
			}
			if( temp[ptr] <= temp_max[ptr])
			{
				for( int i=ptr+1 ; i<tway ; i++ ) 
					temp[i] = temp[i-1] + 1 ;
			}

		} // end for
		delete[] val;
		delete[] temp ;
		delete[] temp_max ;

	} // end if the last row

	if( print ) 
	{
		testprint( tempCA , row , "[CA] tempCA after replace" );
	}

	// copy tempCA to CArray
	for( int i=0 ; i<row ; i++ )
	{
		for( int j=0 ; j<parameter ; j++ )
		{
			if( tempCA[i][j] != -1 )
				CArray[i][j] = tempCA[i][j] ;
		}
	}

	//if( print )
	//	testprint( CArray , row , "[CA] covering array after reorder" );

	delete[] count ;
	for( int k=0 ; k<row ; k++ )
		delete[] tempCA[k] ;
	delete[] tempCA ;

	for( int k=0 ; k<row ; k++ )
		delete[] tmp_tempCA[k] ;
	delete[] tmp_tempCA ;

	return true ;

}

// find all care positions representing as *
// if there exists a row contains all *, then delete it
bool Reduce::PossibleCareCArray()
{
	// combcover = new CombCover( parameter , value , tway + 1 );
	// combcover->GenerateS();

	int **tempCA = new int*[row] ;
	for( int i=0 ; i<row ; i++ )
	{
		tempCA[i] = new int[parameter] ;
		for( int j=0 ; j<parameter ; j++ )
			tempCA[i][j] = -1 ;
	}

	// the number of * in each row
	int *count = new int[row] ;
	for( int k=0 ; k<row ; k++ )
		count[k] = parameter ;

	// sub column
	int *temp = new int[tway] ;      // index for each sub column representing positions
	int *temp_max = new int[tway];   // the maximum values for the position
	for( int k=0 ; k<tway ; k++ )    // initializarion
	{
		temp[k] = k ;
		temp_max[k] = parameter - tway + k ;
	}
	int end = tway - 1 ;
	int ptr = end ;

	// for each sub column ( C(par,tway) in total )
	int *val = new int[tway];
	int allsubcol = cal_combine(parameter, tway) ;
	for( int i = 0 ; i < allsubcol ; i++ )
	{
		int subcomb = 1 ;
		for( int p=0 ; p<tway ; p++ )
			subcomb = subcomb * value[temp[p]] ;

		int *cover = new int[subcomb] ;
		for( int k=0 ; k<subcomb ; k++ )
			cover[k] = 0 ;

		// for each row of tempCA
		for( int j=0 ; j<row ; j++ )
		{
			for( int k=0 ; k<tway ; k++ )
				val[k] = CArray[j][temp[k]];

			int index = cal_index(temp, val);
			// the first appearance will be set to original values
			if( cover[index] == 0 )        
			{
				cover[index] = 1 ;
				for( int c=0 ; c<tway ; c++ )
				{
					if( tempCA[j][temp[c]] == -1 )
					{
						tempCA[j][temp[c]] = CArray[j][temp[c]];
						count[j]-- ;
					}
				}
			}
		}
		delete[] cover ;

		// next sub column
		temp[end] = temp[end] + 1 ;  // adding 1 to the last position
		ptr = end ;
		while( ptr > 0 )
		{
			if( temp[ptr] > temp_max[ptr] )  // exceed the maximum value
			{
				temp[ptr-1] = temp[ptr-1] + 1 ;  // adding 1 to the previous position
				ptr-- ;
			}
			else
				break ;
		}
		if( temp[ptr] <= temp_max[ptr])  // if the value of this position is not the maximum, adding 1 to the follows based on the previous value
		{
			for( int i=ptr+1 ; i<tway ; i++ ) 
				temp[i] = temp[i-1] + 1 ;
		}

	}
	delete[] val;
	delete[] temp ;
	delete[] temp_max ;

	// test tempCA
	/*
	cout<<"tempCA in PossibleCareCArray"<<endl;
	for( int m=0 ; m<row; m++ )
	{
		for( int n=0 ; n<parameter ; n++ )
			cout<<tempCA[m][n]<<" ";
		cout<<endl;
	}
	cout<<"count for *"<<endl;
	for( int k=0 ; k<row ; k++ )
		cout<<count[k]<<" ";
	cout<<endl;
	*/

	//if( print )
	//	testprint( tempCA , row , "[Care] care position" );

	// delete rows if they contain all *
	bool judge = false ;
	int already = 0 ;    // the number of already deleted rows
	int new_row = row ;  // the number of rows of new CA
	for( int k=0 ; k<row ; k++ )
	{
		Care += count[k] ;
		if( iteration == 0 )
			Original_Care += count[k] ;

		if( count[k] == parameter )
		{
			judge = true ;

			// delete row k
			int del_row = k - already ;
			new_row-- ;
			already++ ;

			//cout<<"before delete, del_row: "<<del_row<<", new_row: "<<new_row<<endl;

			int **ptrCA = new int*[new_row] ;
			for( int p=0 ; p<new_row; p++ )
				ptrCA[p] = new int[parameter] ;
			
			int pi = 0 ;
			for( int i=0 ; i<new_row+1 ; i++ )
			{
				if( i != del_row )
				{
					for( int j=0 ; j<parameter ; j++ )
						ptrCA[pi][j] = tempCA[i][j] ;
					pi++;
				}
			}

			for( int i=0 ; i<new_row+1 ; i++ )
				delete[] tempCA[i] ;
			delete[] tempCA ;
			tempCA = ptrCA ;
		}
	}
	if( already != 0 )
		delite_count.insert(pair<int,int>(iteration,already));
	/*
	if( !judge )
	{
		for( int i=0 ; i<new_row ; i++ )
			delete[] tempCA[i] ;
		delete[] tempCA ;
		return false ;  
	}
	*/

	// replace the other * randomly
	for( int i = 0 ; i < new_row ; i++ )
	{
		for( int j = 0 ; j < parameter ; j++ )
		{
			if( tempCA[i][j] == -1 )
				tempCA[i][j] = rand()%value[j] ;
		}
	}

	// CArray = tempCA
	for( int i=0 ; i<row ; i++ )
		delete[] CArray[i] ;
	delete[] CArray ;
	CArray = tempCA ;
	row = new_row ;

	//if( print )
	//	testprint( CArray , row , "[Delete] delete and then replace value" );

	delete[] count ;
	return true;
}


//
// execution
//
void Reduce::Execution()
{
	SUT *sut = new SUT( parameter , value ,tway );

	while( iteration < all_iteration )
	{
		comb_count.clear();
		PossCare = Care = 0 ;

		char a[10];
		string str ,str1 ;
		_itoa(iteration, a, 10);
		str = a;
		str1 = "iteration " + str ;

		if( print )
			testprint( NULL , 0 , str1 );

		ReorderCArray() ;
		PossibleCareCArray() ;

		//if( print )
		//	testprint( CArray , row , "[CHECK]" );

		if( sut->CheckCoverage( CArray , row , tway ) != 1.0 )
		{
			cout<<endl;
			cout<<"ERROR in iter "<<iteration<<" with coverage "
				<<sut->CheckCoverage( CArray , row , tway )<<endl;
			return ;
		}

		if( print )
			testprint_sta( false );

		// break condition
		/*
		if( PossCare <= 2 * parameter && Last_Care <= parameter / 2 )
		{
			cout<<endl;
			cout<<"Meet break condition in iteration "<<iteration<<endl;
			break ;
		}
		*/

		iteration++;
	}

	if( print )
		testprint_sta( true );
	//
	//cout<<"Final"<<endl;
	//stestprint();
}



// print to file
void Reduce::testprint( int **ca , int r , string str )
{
	ofstream fout("CA_result.txt", ios::app );
	if( ca == NULL )
		fout<<"--------------------------------------------------------------------------------"<<endl;
	else
		fout<<"----------------------------------------"<<endl;
	fout<<str ;
	if( ca == NULL )
		fout<<endl;
	else
		fout<<", row = "<<r<<endl;
	for( int i=0 ; i<r ; i++ )
	{
		for( int j=0 ; j<parameter; j++ )
		{
			if( ca[i][j] == -1 )
				fout<<"* ";
			else
				fout<<ca[i][j]<<" ";
		}
		fout<<endl;
	}
	if( ca == NULL )
		fout<<"--------------------------------------------------------------------------------"<<endl;
	else
		fout<<"----------------------------------------"<<endl;
}

void Reduce::testprint_sta( bool final )
{
	ofstream fout("result.txt", ios::app );
	if( !final )
	{
		fout<<"iteration "<<setw(4)<<iteration<<": ";
		fout<<"PossCare: "<<setw(4)<<PossCare<<", ";
		fout<<"Care: "<<setw(4)<<Care<<", ";
		fout<<"Comb: ";
		for( map<int,int>::const_iterator i = comb_count.begin() ; i != comb_count.end() ; i++ )
			fout<<i->first<<"("<<i->second<<") ";
		fout<<endl;
	}
	else
	{
		fout<<endl;
		fout<<"iteration(number of rows deleted)"<<endl;
		for( map<int,int>::const_iterator i = delite_count.begin() ; i != delite_count.end() ; i++ )
			fout<<i->first<<"("<<i->second<<") ";
		fout<<endl;
		fout<<"--------------------------------------------------------------------------------"<<endl;
		fout<<endl;
	}
}