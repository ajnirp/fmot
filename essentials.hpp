#ifndef essentialsH
#define essentialsH

#include <curses.h>
#include <string>
using namespace std;

#include "generalTree.hpp"

#define system(x) system(x.c_str())
#define opendir(x) opendir(x.c_str())
#define enclosed(x) (string("\"")+x+string("\""))
#define printInStatusBar(x) printInBar(string("Now in: ")+reduced(x),statusBar)
#define printInMessageBar(x) printInBar(x,messageBar)
#define TAB 9
#define PG_UP KEY_PPAGE
#define PG_DOWN KEY_NPAGE
#define ENTER 10
#define ESCAPE 27

bool notHotkey( int );

// Path modifying functions.
void replaceRecursively( string&, string, string );
void cleanPath( string& );
bool goBack( string& );
string& extractDirName( string& );
string& addToPath( const string, const string );

// To truncate path to fit (terminal)
// width (if required) replacing the
// starting "part" part by a ~ symbol.
string reduced( string path, int width = COLS-10 );

// Given a node, returns a string describing
// the path (from the root node to that node).
string& returnPath( node< string >* );

#endif
