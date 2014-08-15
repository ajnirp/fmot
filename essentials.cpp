#include "essentials.hpp"

bool notHotkey( int temp ){
	return ( temp != 'q' and temp != 'h' and temp != 'o' and temp != TAB
			  and temp != 'g' and temp != 'f' and temp != '/' and temp != 'r'
			  and temp != KEY_F(2) and temp != '?' and temp != 'p' and temp != KEY_DC
			  and temp != 's' and temp != 'm' and temp != 'c' and temp != 'b' );
}

void replaceRecursively( string& dirty, string substring, string replacement ){

	for( size_t temp; ( temp = dirty.find( substring ) ) != string::npos; /**/
		/**/ dirty.replace( temp, substring.length( ), replacement ) );

}

void cleanPath( string& dirty ){
	replaceRecursively( dirty, "/./", "/" );
	replaceRecursively( dirty, "//", "/" );
}

bool goBack( string& path ){

	// Defaulting back to the wd not desired!
	if( path == string( "/" ) )
		return true;

	cleanPath( path );

	char removed;
	while( path.size( ) > 0 and ( removed =
		path.at( path.size( )-1 ) ) == '/' )
		path.erase( path.end( )-1 );
	if( removed == '.' ){
		if( path.empty( ) )
			path = string( "/" );
		return false;
	}
	for( int i = path.size( )-1; i > 0; i-- ){
		removed = path.at( path.size( )-1 );
		path.erase( path.end( )-1 );
		if( removed == '/' )
			break;
	}
	if( path.empty( ) )
		path = string( "/" );
	return true;

}

string& extractDirName( string& path ){

	string *dirName = new string;
	for( int i = path.size( )-1; i != 0 and path[ i ] != '/'; i-- )
		dirName->push_back( path[ i ] );
	for( int i = 0, end = dirName->size( )/2, /**/
		/**/ nameSize = dirName->size( )-1, temp; i < end; i++ ){
		temp = dirName->at( i );
		dirName->at( i ) = dirName->at( nameSize-i );
		dirName->at( nameSize-i ) = temp;
	}
	return *dirName;

}

string& addToPath( const string oldPath, const string nextFileName ){

	string *newPath = new string;
	*newPath = oldPath;
	if( nextFileName == string( ".." ) )
		if( goBack( *newPath ) )
			return *newPath;
	if( newPath->size( ) != 0 and newPath->at( newPath->size( )-1 ) != '/' )
		newPath->append( "/" );
	newPath->append( nextFileName );
	return *newPath;

}

string reduced( string path, int width ){

	if( path.length( ) > width )
		return ( string( "~" ) + path.substr( path.size( )-width+1, width-1 ) );
	return path;

}

string& returnPath( node< string >* v ){

	string *path = new string;
	if( v != NULL ){
		path->append( v->value );
		for( node< string > *tmp = v->parent; tmp != NULL and tmp->parent != NULL; tmp = tmp->parent )
			*path = tmp->value + string( "/" ) + *path;
	}
	return *path;

}
