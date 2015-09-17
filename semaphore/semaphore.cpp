#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // sleep
#include <vector>
#include <time.h> 
#include <string>
#define PAUSE cout << "Press any key to continue..." << endl; fgetc(stdin);
using namespace std;

enum Component{ battery, snesor, wifi, crawler };
vector< Component > comp;

int total = 0;	// number of robots
int countb = 0;	// number of producer1 create robots. producer1 don't have battery
int counts = 0;	// number of producer2 create robots. producer2 don't have snesor
int countw = 0;	// number of producer3 create robots. producer3 don't have wifi
int countc = 0;	// number of producer4 create robots. producer4 don't have crawler
int	battery_n1 = 0;	// number of dispatcher(A) create battery
int	snesor_n1 = 0;	// number of dispatcher(A) create snesor
int	wifi_n1 = 0;	// number of dispatcher(A) create wifi
int	crawler_n1 = 0;	// number of dispatcher(A) create crawler
int	battery_n2 = 0;	// number of dispatcher(B) create battery
int	snesor_n2 = 0;	// number of dispatcher(B) create snesor
int	wifi_n2 = 0;	// number of dispatcher(B) create wifi
int	crawler_n2 = 0;	// number of dispatcher(B) create crawler

sem_t empty;	// dispatcher can do 3 times, then wait and let producer can do once.
sem_t full;		// producer do once, then wait and let dispatcher can do 3 times. 
sem_t mux;		// mutex

Component getComponent();	// get not identical Component
bool identical( Component c );	// return Component c is in comp or not
void printComponent( Component c );	// print Component and dispatcher(A) which is created plus 1.
void printComponent2( Component c );// print Component and dispatcher(B) which is created plus 1.

void *dispatcher( void *arg );
void *dispatcher2( void *arg );
void *producer1( void *arg );	// have battery
void *producer2( void *arg );	// have snesor
void *producer3( void *arg );	// have wifi
void *producer4( void *arg );	// have crawler

int main( int argc, char *argv[] ){
	srand( time(0) );
	sem_init( &empty, 0, 3 );
	sem_init( &full, 0, 0 );
	sem_init( &mux, 0, 1 );
	pthread_t d1, d2, p1, p2, p3, p4;
	if ( argv[1][0] == '1' ){
		pthread_create( &d1, NULL, dispatcher, NULL);
		pthread_create( &p1, NULL, producer1, NULL);
		pthread_create( &p2, NULL, producer2, NULL);
		pthread_create( &p3, NULL, producer3, NULL);
		pthread_create( &p4, NULL, producer4, NULL);
		pthread_join( d1, NULL );
		cout << endl;
		cout << "Dispatcher ( battery ) number : " << battery_n1 << endl;
		cout << "Dispatcher ( snesor ) number : " << snesor_n1 << endl;
		cout << "Dispatcher ( wifi ) number : " << wifi_n1 << endl;
		cout << "Dispatcher ( crawler ) number : " << crawler_n1 << endl << endl;
		cout << "Producer ( battery ) number : " << countb << " robot(s)\n";
		cout << "Producer ( snesor ) number : " << counts << " robot(s)\n";
		cout << "Producer ( wifi ) number : " << countw << " robot(s)\n";
		cout << "Producer ( crawler ) number : " << countc << " robot(s)\n";
		}
	else if( argv[1][0] == '2' ){
		pthread_create( &d1, NULL, dispatcher, NULL);
		pthread_create( &d2, NULL, dispatcher2, NULL);
		pthread_create( &p1, NULL, producer1, NULL);
		pthread_create( &p2, NULL, producer2, NULL);
		pthread_create( &p3, NULL, producer3, NULL);
		pthread_create( &p4, NULL, producer4, NULL);
		pthread_join( d1, NULL );
		pthread_join( d2, NULL );
		cout << endl;
		cout << "Dispatcher( A ) ( battery ) number : " << battery_n1 << endl;
		cout << "Dispatcher( A ) ( snesor ) number : " << snesor_n1 << endl;
		cout << "Dispatcher( A ) ( wifi ) number : " << wifi_n1 << endl;
		cout << "Dispatcher( A ) ( crawler ) number : " << crawler_n1 << endl << endl;
		cout << "Dispatcher( B ) ( battery ) number : " << battery_n2 << endl;
		cout << "Dispatcher( B ) ( snesor ) number : " << snesor_n2 << endl;
		cout << "Dispatcher( B ) ( wifi ) number : " << wifi_n2 << endl;
		cout << "Dispatcher( B ) ( crawler ) number : " << crawler_n2 << endl << endl;
		cout << "Producer ( battery ) number : " << countb << " robot(s)\n";
		cout << "Producer ( snesor ) number : " << counts << " robot(s)\n";
		cout << "Producer ( wifi ) number : " << countw << " robot(s)\n";
		cout << "Producer ( crawler ) number : " << countc << " robot(s)\n";
	}
}

void *dispatcher( void *arg )
{
	while( true ){
		sem_wait( &empty );
		sem_wait( &mux );
		if ( total == 40 ){
			sem_post( &mux );
			break;
		}
		Component c = getComponent();
		comp.push_back ( c );
		
		cout << "Dispatcher(A) : ";
		printComponent( c );
		cout << endl;
		if ( comp.size() == 3 ){
			sem_post( &full );
		}
		sem_post( &mux );
	}
}
void *dispatcher2( void *arg )
{
	while( true ){
		sem_wait( &empty );
		sem_wait( &mux );
		if ( total == 40 ){
			sem_post( &mux );
			break;
		}
		Component c = getComponent();
		comp.push_back ( c );
		
		cout << "Dispatcher(B) : ";
		printComponent2( c );
		cout << endl;
		if ( comp.size() == 3 ){
			sem_post( &full );
		}
		sem_post( &mux );
	}
}
Component getComponent()
{
	int r;
	while( true ){
		r = rand()%4;
		if ( !identical( ( Component )r ) )
			break;
	}
	return ( Component )r;
}
bool identical( Component c )
{
	for ( int i = 0 ; i < comp.size() ; i++ ){
		if( c == comp[ i ] )
			return true;
	}
	return false;
}
void *producer1( void *arg )
{
	while( true ){
		sem_wait( &full );
		if ( !identical( battery ) ){
			countb++;
			total++;
			cout << "Producer ( battery ) : OK, " << total << " robot(s)\n";
			comp.clear();
			sem_post( &empty );
			sem_post( &empty );
			sem_post( &empty );
		}
		else{
			sem_post( &full );
		}
	}
}
void *producer2( void *arg )
{
	while( true ){
		sem_wait( &full );
		if ( !identical( snesor ) ){
			counts++;
			total++;
			cout << "Producer ( snesor ) : OK, " << total << " robot(s)\n";
			comp.clear();
			sem_post( &empty );
			sem_post( &empty );
			sem_post( &empty );
		}
		else{
			sem_post( &full );
		}
	}
}
void *producer3( void *arg )
{
	while( true ){
		sem_wait( &full );
		if ( !identical( wifi ) ){
			countw++;
			total++;
			cout << "Producer ( wifi ) : OK, " << total << " robot(s)\n";
			comp.clear();
			sem_post( &empty );
			sem_post( &empty );
			sem_post( &empty );
		}
		else{
			sem_post( &full );
		}
	}
}
void *producer4( void *arg )
{
	while( true ){
		sem_wait( &full );
		if ( !identical( crawler ) ){
			countc++;
			total++;
			cout << "Producer ( crawler ) : OK, " << total << " robot(s)\n";
			comp.clear();
			sem_post( &empty );
			sem_post( &empty );
			sem_post( &empty );
		}
		else{
			sem_post( &full );
		}
	}
}
void printComponent( Component c )
{
	switch( c ){
		case battery:
			cout << "battery";
			battery_n1++;
			break;
		case snesor:
			cout << "snesor";
			snesor_n1++;
			break;
		case wifi:
			cout << "wifi";
			wifi_n1++;
			break;
		case crawler:
			cout << "crawler";
			crawler_n1++;
			break;
		default:
			cout << "error";
			break;
	}
}
void printComponent2( Component c )
{
	switch( c ){
		case battery:
			cout << "battery";
			battery_n2++;
			break;
		case snesor:
			cout << "snesor";
			snesor_n2++;
			break;
		case wifi:
			cout << "wifi";
			wifi_n2++;
			break;
		case crawler:
			cout << "crawler";
			crawler_n2++;
			break;
		default:
			cout << "error";
			break;
	}
}