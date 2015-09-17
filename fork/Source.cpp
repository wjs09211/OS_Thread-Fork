#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <stdlib.h>

#define PAUSE cout << "Press any key to continue..." << endl; fgetc(stdin);

using namespace std;

int row, column, x, y;
int fd[100][2], nbytes;
string map[30];

void Load_Map( char *argv[] ); // load map
void Load_SMap( bool &); //Ū�^�������|�M�O�_���
void Save_Map( bool ); //�������|�M�O�_���
void Load_SMap2( bool &); //Ū�^�������|�M�O�_���  pipe
void Save_Map2( bool ); //�������|�M�O�_���    pipe 

void Find_S(); //find S ( x,y ) (left-top( 0,0 ))
int NumberOfRoad(); // find how many road
bool GoUp(); // if up can go, go up
bool GoRight();// if right can go, go right
bool GoDown();// if down can go, go down
bool GoLeft();// if left can go, go left
void Go(); //�e�i
void PrintMap();
void CreatPipe(); /// pipe

int main(int argc, char *argv[])
{
	bool find = false;
	int state = 1;
	Load_Map( argv );// load
	Find_S();//find S
	CreatPipe();
	pid_t pid = fork();
	while ( true ){
		if ( pid < 0 ){
			cout << "error" << endl;
		}
		else if ( pid == 0 ) { // chrid
			if ( NumberOfRoad() == 0 || map[ y ][ x ] == 'K' ) { // �S���Χ��K
				if ( map[ y ][ x ] != 'K' ) { 
					cout << getpid() << "(" << y << "," << x << ")" << " None!\n"; //�����C�X�䤣��
					map[ y ][ x ] = '0'; //���L��0
				}
				else {
					cout << getpid() << "(" << y << "," << x << ")" << " Found!\n";  //�L�X���K����m�Mparent��pid
					find = true; 
				}
				Save_Map( find ); //�s�ɬ���
				break; // chrid���� �� parent wait�H�᪺���� 
			}
		
			else if ( NumberOfRoad() >= 2 ){ //�p�G���e��
				if ( state == 1 ) { //��state�קK�L�uloop
					pid = fork();
					state++;
				}
				else {
					Go();
					state = 1;
				}
			}
			else { // �u���@�Ӥ�V
				Go();
				/*PrintMap();
				PAUSE;*/
			}
		}
		else { //parent
			cout << "[pid=" << pid << "]: (" << y << "," << x << ")\n";  //�C�X parent �� pid
			wait(NULL); //�� chrid �]��
			Load_SMap( find ); //Ū�^�O����
			if ( NumberOfRoad() != 0 ) { // �������n�~��fork
				pid = fork();
				state = 1;
			}
			else {
				if ( find == true ){ //�p�G��������� �h�L�X parent��pid �M Found
					cout << pid << "(" << y << "," << x << ")" << " Found!\n";
				}
				break;
			}
		}

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
void Load_SMap( bool &find )
{
	fstream indata ( "smap.txt", ios::in );
	for ( int i = 0 ; i < row ; i++ ){
		getline( indata, map[ i ], '\n' );
	}
	indata >> find;
}
void Save_Map( bool find )
{
	fstream outdata ( "smap.txt", ios::out );
	for ( int i = 0 ; i < row - 1 ; i++ ){
		outdata << map[ i ] << endl;
	}
	outdata << map[ row - 1 ] << endl;
	outdata << find;
}
void Find_S()
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
int NumberOfRoad()
{
	int number = 0;
	if ( y - 1 > 0 ){
		if ( map[ y - 1 ][ x ] == ' ' || map[ y - 1 ][ x ] == 'K' ){ //up
			number++;
		}
	}
	if ( x + 1 < column ){
		if ( map[ y ][ x + 1 ] == ' ' || map[ y ][ x + 1 ] == 'K'){ //right
			number++;
		}
	}
	if ( y + 1 < row ){
		if ( map[ y + 1 ][ x ] == ' ' || map[ y + 1 ][ x ] == 'K'){ //down
			number++;
		}
	}
	if ( x - 1 > 0 ){
		if ( map[ y ][ x - 1 ] == ' ' || map[ y ][ x  - 1 ] == 'K'){ //left
			number++;
		}
	}
	return number;
} 
void Go()
{
	if ( GoUp() ){
		map[ y ][ x ] = '0';
		y--;
	}
	else if ( GoLeft() ){
		map[ y ][ x ] = '0';
		x--;
	}
	else if ( GoRight() ){
		map[ y ][ x ] = '0';
		x++;
	}
	else if ( GoDown() ){
		map[ y ][ x ] = '0';
		y++;
	}
}
bool GoUp()
{
	if ( y - 1 > 0 ){
		if ( map[ y - 1 ][ x ] == ' ' || map[ y - 1 ][ x ] == 'K' ){ //up
			return true;
		}
	}
	return false;
}
bool GoRight()
{
	if ( x + 1 < column ){
		if ( map[ y ][ x + 1 ] == ' ' || map[ y ][ x + 1 ] == 'K' ){ //right
			return true;
		}
	}
	return false;
}
bool GoDown()
{
	if ( y + 1 < row ){
		if ( map[ y + 1 ][ x ] == ' ' || map[ y + 1 ][ x ] == 'K' ){ //down
			return true;
		}
	}
	return false;
}
bool GoLeft()
{
	if ( x - 1 > 0 ){
		if ( map[ y ][ x - 1 ] == ' ' || map[ y ][ x - 1 ] == 'K' ){ //left
			return true;
		}
	}
	return false;
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
void CreatPipe()
{
	for ( int i = 0 ; i < row + 1 ; i++ ){
		pipe(fd[i]);
	}
}
void Load_SMap2( bool &find)
{
	for ( int i = 0 ; i < row ; i++ ){
		close( fd[ i ][ 1 ] );
		nbytes = read( fd[ i ][ 0 ], &map[ i ], sizeof(map[ i ]));
	}
		close( fd[ row ][ 1 ] );
		nbytes = read( fd[ row ][ 0 ], &find, sizeof(find) );

} //Ū�^�������|�M�O�_���

void Save_Map2( bool find)
{
	for ( int i = 0 ; i < row ; i++ ){
		close( fd[ i ][ 0 ] );
		write( fd[ i ][ 1 ], &map[ i ], map[ i ].size() + 1 );
	}
		close( fd[ row ][ 0 ] );
		write( fd[ row ][ 1 ], &find, 1 );

} //�������|�M�O�_���