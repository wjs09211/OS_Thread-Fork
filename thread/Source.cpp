#include<pthread.h>
#include<iostream>
#include <stdio.h>
#include <string>
#include<vector>
#include <stdlib.h> 
#include <fstream>
#include<time.h>

#define PAUSE cout << "Press any key to continue..." << endl; fgetc(stdin);
using namespace std;

struct point{
	int next_x;
	int next_y;
};
struct Data{
	int x;
	int y;
	vector<point> next_point;
	bool find;
};

string map[30];
int row = 0;
int column = 0;

void Load_Map( char *argv[] ); 
void PrintMap();
void Find_S( int &x, int &y ); //find S ( x,y ) (left-top( 0,0 ))
vector<point> next_dir( int x, int y  ); // find how many road
bool GoUp( int x, int y  ); // if up can go, go up
bool GoRight( int x, int y  );// if right can go, go right
bool GoDown( int x, int y  );// if down can go, go down
bool GoLeft( int x, int y  );// if left can go, go left

void *execute( void *param );

int main(int argc, char *argv[])
{
	clock_t start, end;
	double cpu_time_used;
	start = clock();
	Load_Map( argv );// load
	//PrintMap();
	
	Data *first_data = new Data();
	Find_S( first_data->x, first_data->y );
	first_data->find = false;
	first_data->next_point = next_dir( first_data->x, first_data->y );

	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init( &attr );
	pthread_create( &tid, &attr, execute, first_data );
	pthread_join( tid, NULL );
	
	//PrintMap();
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	cout << "Total: " << cpu_time_used * 1000 << " ms" << endl;
}


void *execute( void *param )
{
	Data *last_data = (Data *)param;
	Data *this_data = new Data();
	Data *buffer_data = new Data[3];
	this_data->x = last_data->x;
	this_data->y = last_data->y;
	this_data->find = last_data->find;
	this_data->next_point = last_data->next_point;
	cout << "[tid = " << pthread_self() << " ]:(" << this_data->y << "," << this_data->x << ")" << endl ;
	while( true ) {
		if ( this_data->next_point.size() == 0 || map[ this_data->y ][ this_data->x ] == 'K' ){
			if ( map[ this_data->y ][ this_data->x ] == 'K' ){
				cout << "[tid = " << pthread_self() << " ]:(" << this_data->y << "," << this_data->x << ") Found!" << endl ;
				this_data->find = true;
				last_data->find = true;			
			}
			else {
				cout << "[tid = " << pthread_self() << " ]:(" << this_data->y << "," << this_data->x << ") None!" << endl ;
			}			
			break;
		}
		else if ( this_data->next_point.size() == 1 ){
			map[ this_data->y ][ this_data->x ] = '0';
			this_data->x = this_data->next_point[0].next_x;
			this_data->y = this_data->next_point[0].next_y;
			this_data->next_point = next_dir( this_data->x, this_data->y );
			/*PrintMap();
			PAUSE;*/
		}
		else if ( this_data->next_point.size() >= 2 ){
			int num = this_data->next_point.size();
			pthread_t tid[3];
			pthread_attr_t attr;
			pthread_attr_init( &attr );
			vector< point > p;
			for ( int i = 0 ; i < num ; i++ ){
				p.push_back( this_data->next_point[ i ] );
			}
			for ( int i = 0 ; i < num ; i++ ){
				this_data->next_point.clear();
				this_data->next_point.push_back( p[ i ] );
				buffer_data[i].x = this_data->x;
				buffer_data[i].y = this_data->y;
				buffer_data[i].find = this_data->find;
				buffer_data[i].next_point = this_data->next_point;
				pthread_create( &tid[ i ], &attr, execute, &buffer_data[i] );
			}
			for ( int i = 0 ; i < num ; i++ ){
				pthread_join( tid[ i ], NULL );
			}
			break;
		}
	}
	if ( this_data->find == true || buffer_data[0].find == true || buffer_data[1].find == true || buffer_data[2].find == true ){
		last_data->find = true;
	}
	if( last_data->find == true ){
		cout << "[tid = " << pthread_self() << " ]:(" << last_data->y << "," << last_data->x << ") Found!" << endl ;
	}



}


void Load_Map( char *argv[] )
{
	fstream indata ( argv[1] , ios::in );
	row = 0;
	while ( getline( indata, map[ row ], '\n' ) ){
		row++;
	}
	column = map[ 0 ].size();
}
void PrintMap()
{
	for ( int i = 0 ; i < row ; i++ ){
		for ( int j = 0 ; j < column ; j++ ){
			cout << map[ i ][ j ];
		}
		cout << endl;
	}
	cout << endl;
}

void Find_S( int &x, int &y  )
{
	for ( int i = 0 ; i < row ; i++ ){
		for ( int j = 0 ; j < column ; j++ ){
			if ( map[i][j] == 'S') {
				x = j;
				y = i;
				return;
			}
		}
	}
}
vector<point> next_dir( int x, int y  )
{
	vector<point> dir;
	point p;
	if ( y - 1 > 0 ){
		if ( map[ y - 1 ][ x ] == ' ' || map[ y - 1 ][ x ] == 'K' ){ //up
			p.next_y = y - 1;
			p.next_x = x;
			dir.push_back( p );
		}
	}
	if ( x + 1 < column ){
		if ( map[ y ][ x + 1 ] == ' ' || map[ y ][ x + 1 ] == 'K'){ //right
			p.next_y = y;
			p.next_x = x + 1;
			dir.push_back( p );
		}
	}
	if ( y + 1 < row ){
		if ( map[ y + 1 ][ x ] == ' ' || map[ y + 1 ][ x ] == 'K'){ //down
			p.next_y = y + 1;
			p.next_x = x;
			dir.push_back( p );
		}
	}
	if ( x - 1 > 0 ){
		if ( map[ y ][ x - 1 ] == ' ' || map[ y ][ x  - 1 ] == 'K'){ //left
			p.next_y = y;
			p.next_x = x - 1;
			dir.push_back( p );
		}
	}
	return  dir;
} 
bool GoUp( int x, int y  )
{
	if ( y - 1 > 0 ){
		if ( map[ y - 1 ][ x ] == ' ' || map[ y - 1 ][ x ] == 'K' ){ //up
			return true;
		}
	}
	return false;
}
bool GoRight( int x, int&y  )
{
	if ( x + 1 < column ){
		if ( map[ y ][ x + 1 ] == ' ' || map[ y ][ x + 1 ] == 'K' ){ //right
			return true;
		}
	}
	return false;
}
bool GoDown( int x, int y  )
{
	if ( y + 1 < row ){
		if ( map[ y + 1 ][ x ] == ' ' || map[ y + 1 ][ x ] == 'K' ){ //down
			return true;
		}
	}
	return false;
}
bool GoLeft( int x, int y  )
{
	if ( x - 1 > 0 ){
		if ( map[ y ][ x - 1 ] == ' ' || map[ y ][ x - 1 ] == 'K' ){ //left
			return true;
		}
	}
	return false;
}

