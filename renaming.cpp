#include "renaming.hpp"

int rename_files( list_of_files& original_names, list_of_files& new_names ){

	if( original_names.size( ) != new_names.size( ) )
		return 0;

	int successes = 0;
	auto itr1 = original_names.begin( );
	auto itr2 = new_names.begin( );
	for( int rename_result = 0; itr1 != original_names.end( ); itr1++, itr2++ ){
		rename_result = system( ( string( "mv " ) + enclosed( *itr1 ) + string( " " ) + enclosed( *itr2 ) ) );
		if( rename_result == 0 ) successes++;
	}

	return successes;

}

bool rename_file( string original_name, string new_name ){

	list_of_files o, n;
	o.push_back( original_name );
	n.push_back( new_name );
	int res = rename_files( o, n );
	return res;

}

string create_backup( string filename ){

	char numTag[6]; string resultant_name;
	for( int i = 0; i < 100000; i++ ){
		sprintf( numTag, "%5d", i );
		resultant_name = ( filename + string( numTag ) );
		if( not system( ( string( "test -e " ) + enclosed( resultant_name ) ) ) )
			if( rename_file( filename, resultant_name ) )
				return resultant_name;
	}
	return string( "" );

}
