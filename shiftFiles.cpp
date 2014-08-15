#include "shiftFiles.hpp"

bool shift_files( string command, vector< string >& file_paths, string destination ){
	string concatenation = command + string( " " );
	if( command == string( "cp" ) ) concatenation.append( string( "-r " ) );
	for( auto itr = file_paths.begin( ); itr != file_paths.end( ); itr++ )
		concatenation.append( enclosed( *itr ) + string( " " ) );
	return not system( ( concatenation + enclosed( destination ) ) );
}

/*
int main( ){

	cout << not system( string( "mkdir test1" ) ) << endl;
	system( string( "mkdir test2" ) );
	vector< string > file_names = { "./a.out", "./test2" };
	cout << shift_files( "mv", file_names, "./test1" ) << endl;
	return 0;

}
*/
