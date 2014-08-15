#include "fileHandling.hpp"

string& file_data( string file_path ){

	//wipe the string clean in case it wasn't the null string to begin with
	string *data_string = new string;

	//create a file whose name is a random string of digits
	srand( time( NULL ) );
	char file_data[ 10 ];
	sprintf( file_data, "%d", rand( ) );

	//get the required attributes and store them in file_data
	system( ( "ls -l 2>/dev/null " + enclosed( file_path ) + " 1>" + file_data ) );

	//set up a pointer to the file to read its contents
	FILE *reader = fopen( file_data, "r" );

	//transfer the contents of file_data to data_string
	for( char c; ( c = getc( reader ) ) != EOF; data_string->push_back( c ) );

	//clean up
	fclose( reader );
	system( ( string( "rm -f 2>/dev/null " ) + enclosed( string( file_data ) ) ) );

	//return the modified data_string
	return *data_string;

}

bool create_blank_file( string name, bool overwrite ){

	if( not system( ( "test -e " + enclosed( name ) ) ) )
		if( overwrite ){
			if( system( ( string( "rm -f 2>/dev/null " ) + enclosed( name ) ) ) )
				return false;
		}
		else return false;
	return ( not system( ( "touch " + enclosed( name ) ) ) );

}

bool create_empty_folder( string name_path ){

	if( not system( ( "test -d " + enclosed( name_path ) ) ) )
		if( not system( ( "mkdir " + enclosed( name_path ) ) ) )
			return true;
	return false;

}

bool remove_file( string file_name ){

	return not system( ( string( "rm -r -f 2>/dev/null " ) + enclosed( file_name ) ) );

}

bool open_file( string file ){

	// we need to know if the current user is root, b'coz
	// xdg-open should not be used as root in general.
	srand( time( NULL ) );
	char user_name_file [ 10 ];
	sprintf( user_name_file , "%d", rand( ) );

	system( ( string( "echo $USER > " ) + string( user_name_file ) ) );

	ifstream reader( user_name_file );
	string user_name;
	reader >> user_name;

	reader.close( );
	system( ( string( "rm -f 2>/dev/null " ) + enclosed( string( user_name_file ) ) ) );

	return ( user_name != "root" and not system( ( string( "xdg-open " ) + enclosed( file ) ) ) );

}

bool listingFilePossible( string currDir ){

	DIR *dp = opendir( currDir );
	if( dp != NULL ){
		(void) closedir( dp );
		return true;
	}
	return false;

}

void listFilesInDirectory( vector< string >& files, string& currDir ){

	files.clear( );

	DIR *dp = opendir( currDir );
	dirent * ep;
	if( dp != NULL ) {
		while( ep = readdir( dp ) )
			if( ep->d_name != string( "." ) )
				files.push_back( ep->d_name );
		(void) closedir( dp );
		sort( files.begin( ), files.end( ) );
	}

}
