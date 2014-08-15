#ifndef generalTreeH
#define generalTreeH

#include <vector>
#include <string>
using namespace std;

// Templatized structure for tree-nodes.
template< class elementType >
struct node{

	elementType value;
	node< elementType > *parent;
	node< elementType > *leftChild;
	node< elementType > *leftSibling;
	node< elementType > *rightSibling;

};

// Templatized general-tree class.
template< class eType >
class tree{

	private:

		node< eType > *treeRoot;
		string& serializeTree( node< eType >& );

	public:

		tree( );
		node< eType >& root( );
		node< eType >* getLastChild( node< eType >& );
		bool setElement( node< eType >&, eType );
		bool addNextSibling( node< eType >&, eType );
		bool addSibling( node< eType >&, eType );
		bool addSiblings( node< eType >&, vector< eType >&, int index = 0 );
		bool addOnlyChild( node< eType >&, eType );
		bool addChild( node< eType >&, eType );
		bool addChildren( node< eType >&, vector< eType >& );
		string& serializeTree( );
		void deserialize( string );

};

template< class eType >
tree< eType >::tree( ){

	treeRoot = new node< eType >;
	treeRoot->parent = NULL;
	treeRoot->leftChild = NULL;
	treeRoot->leftSibling = NULL;
	treeRoot->rightSibling = NULL;

}

template< class eType >
node< eType >& tree< eType >::root( ){

	return *treeRoot;

}

template< class eType >
node< eType >* tree< eType >::getLastChild( node< eType >& v ){
	if( &v != NULL )
		if ( v.leftChild == NULL )
			return NULL;
		else{
			node< eType >* scanner = v.leftChild;
			while( scanner->rightSibling != NULL )
				scanner = scanner->rightSibling;
			return scanner;
		}
}

template< class eType >
bool tree< eType >::setElement( node< eType >& v, eType e ){

	if( &v != NULL ){

		v.value = e;
		return true;

	}

	return false;

}

template< class eType >
bool tree< eType >::addNextSibling( node< eType >& v, eType e ){

	if( &v != NULL and v.rightSibling == NULL ){

		node< eType > *newNode = new node< eType >;
		newNode->value = e;
		newNode->parent = v.parent;
		newNode->leftChild = NULL;
		newNode->leftSibling = &v;
		newNode->rightSibling = NULL;
		v.rightSibling = newNode;
		return true;

	}

	return false;

}

template< class eType >
bool tree< eType >::addSibling( node< eType >& v, eType e ){

	if( &v != NULL ){

		node< eType >* tmp = &v;
		for( ; tmp->rightSibling != NULL; tmp = tmp->rightSibling );
		return addNextSibling( *tmp, e );

	}

	return false;

}

template< class eType >
bool tree< eType >::addSiblings( node< eType >& v, vector< eType >& e, int index ){

	if( &v != NULL ){

		auto itr = e.begin( );
		itr = itr + index;
		for( node< eType > *tmp = &v; itr != e.end( ); itr++, tmp = tmp->rightSibling )
			addSibling( *tmp, *itr );

		return true;

	}

	return false;

}

template< class eType >
bool tree< eType >::addOnlyChild( node< eType >& v, eType e ){

	if( &v != NULL and v.leftChild == NULL ){

		node< eType > *newNode = new node< eType >;
		newNode->value = e;
		newNode->parent = &v;
		newNode->leftChild = NULL;
		newNode->leftSibling = NULL;
		newNode->rightSibling = NULL;
		v.leftChild = newNode;
		return true;

	}

	return false;

}

template< class eType >
bool tree< eType >::addChild( node< eType >& v, eType e ){

	if( &v != NULL ){

		if( v.leftChild == NULL ){

			return addOnlyChild( v, e );

		}

		node< eType > *tmp;
		for( tmp = v.leftChild; tmp->rightSibling != NULL; tmp = tmp->rightSibling );
		return addSibling( *tmp, e );

	}

	return false;

}

template< class eType >
bool tree< eType >::addChildren( node< eType >& v, vector< eType >& e ){

	if( e.empty( ) or &v == NULL ){

		return false;

	}

	if( v.leftChild != NULL ){

		node< eType > *tmp;
		for( tmp = v.leftChild; tmp != NULL; tmp = tmp->rightSibling );
		return addSiblings( *tmp, e, 0 );

	}

	else{

		if( addOnlyChild( v, e[ 0 ] ) ){

			return addSiblings( *( v.leftChild ), e, 1 );

		}

		return false;

	}

}

template< class eType >
string& tree< eType >::serializeTree( node< eType >& v ){

	string *result = new string;

	if( &v == NULL ){

		*result = string( "\\" );
		return *result;

	}

	else{

		/*********************************************
		 * We use / and \ as the separators			 *
		 * / -> separating files					 *
		 * \ -> identifying 'blocks' of leaf nodes	 *
		 * (end of child marker)					 *
		 *********************************************/

		result->append( v.value + string( "/" ) );
		for( node< eType > *tmp = v.leftChild; tmp != NULL; tmp = tmp->rightSibling )
			result->append( serializeTree( *tmp ) );
		result->append( string( "\\" ) );
		return *result;

	}

}

//Returns a string representing the tree in serialized format
template< class eType >
string& tree< eType >::serializeTree( ){

	string *result = &( serializeTree( *treeRoot ) );
	return *result;

}

// To be called by an empty tree only.
// Takes s (the encoded or 'serialized' form of the tree) and populates the tree
template< class eType >
void tree< eType >::deserialize( string s ){

	node< eType > *currNode = treeRoot;

	int i = 0;

	string temp;
	while ( s[i] != '/' ) {
		temp.push_back( s[i] );
		i++;
	}

	setElement( *currNode, temp );
	i++;

	while ( s[i] == '\\' ) {
		currNode = currNode->parent;
		i++;
	}

	while ( i < s.size() ) {

		string temp = "";
		while ( s[i] != '/' and i < s.size() ) {
			temp.push_back( s[i] );
			i++;
		}
		i++;

		addChild( *currNode, temp );

		currNode = currNode->leftChild;
		while ( currNode->rightSibling != NULL )
			currNode = currNode->rightSibling;

		while ( s[i] == '\\' ) {

			currNode = currNode->parent;
			i++;

		}
	}

}

#endif
