#include <algorithm>
#include <curses.h>
#include <dirent.h>
#include <stdlib.h>
#include <string>
#include <vector>
using namespace std;

#include "essentials.hpp"

int min( int x, int y ){
	if( x < y) return x;
	else return y;
}

int max( int x, int y ){
	if( x < y) return y;
	else return x;
}

class customMenu{

	private:

	WINDOW *menuWindow;
	int mwX, mwY; // mw => MenuWindow
	vector< string > menuItemsText;
	vector< WINDOW* > menuItems;
	int menuStartColumn, menuStartRow;
	int highlighted, start, end;
	string currDir;

	public:

	void setCurrentDirectory( string currentDirectory ){
		currDir = currentDirectory;
	}

	pair< vector< int >, vector< string > > selection;

	explicit customMenu( WINDOW *window, int startColumn = 0, int startRow = 1 ){

		highlighted = 0, start = 0; end = -1;

		curs_set( 0 );
		noecho( );
		keypad( stdscr, TRUE );

		start_color( );
		init_pair( 1, COLOR_YELLOW, COLOR_WHITE ); // 1 = SELECTED ROW // (ID,TEXT,BG)
		init_pair( 2, COLOR_BLUE, COLOR_WHITE ); // 2 = ORDINARY ROW
		init_pair( 3, COLOR_WHITE, COLOR_BLACK ); // 3 = HIGHLIGHTED ROW
		init_pair( 4, COLOR_YELLOW, COLOR_BLACK ); // 4 = HIGHLIGHTED ROW (SELECTED)
		init_pair( 5, COLOR_WHITE, COLOR_BLUE ); // INVERTED ORDINARY ROW

		menuWindow = window;
		menuStartColumn = startColumn;
		menuStartRow = startRow;

		getmaxyx( menuWindow, mwY, mwX );
		mwY += 2; /*-2*/

	}

	template< class comparable >
	bool equal( vector< comparable > v1, vector< comparable > v2 ){
		if( v1.size( ) != v2.size( ) ) return false;
		for( auto itr1 = v1.begin( ), itr2 = v2.begin( ); itr1 != v1.end( ); itr1++, itr2++ ){
			if( *itr1 != *itr2 ) return false;
		}
		return true;
	}

	void drawMenu( vector< string > itemsText, bool fresh = false, bool forcedCheck = false ){

		int oldStart = start;
		bool folderCheck = true;

		if( equal( menuItemsText, itemsText ) ){ folderCheck = false; }

		menuItemsText.clear( ); menuItems.clear( );
		if( !itemsText.empty( ) ){ menuItemsText = itemsText; }
		else{
			menuItemsText.push_back( string( "Nothing to present to you." ) );
			menuItemsText.push_back( string( "Either a search with no matches was performed or a" ) );
			menuItemsText.push_back( string( "[random filesystem reading] error just occured..." ) );
		}

		menuItems.push_back( derwin( menuWindow, mwY-2 /*-2*/, mwX, 0, 0 ) );
		wbkgd( menuItems.at( 0 ), COLOR_PAIR( 2 ) );

		// Decide which entries to show on the menu!
		// Needed (desperately) to scroll long lists.
		if( fresh ){
			start = 0; highlighted = 0;
			end = mwY-2; folderCheck = true;
		}
		if( highlighted > end or end < 0 ){
			start = 0; end = mwY-2;
		}
		if( menuItemsText.size( ) < mwY-2 ){
			start = 0; end = menuItemsText.size( );
		}
		else if( highlighted > end-1 ){
			start = highlighted-mwY+3;
			end = highlighted+1;
		}
		if( highlighted < start ){
			start = highlighted;
			end = highlighted + min( mwY-2, menuItemsText.size( )-start );
		}

		if( oldStart != start ){ folderCheck = true; }

		for( int i = start; i < end; i++ ){

			// Specify menu size here: 1 for bordered and 2 for plain /*classic*/
			menuItems.push_back( derwin( menuItems.at( 0 ), 1, mwX, i-start, 0 ) ); // 2
			//menuItems.push_back( derwin( menuItems.at( 0 ), 1, mwX-2, i-start+1, 1 ) ); // 1

			if( highlighted == i ) wbkgd( menuItems.at( i-start+1 ), COLOR_PAIR( 3 ) );

			auto itr1 = selection.first.begin( );
			auto itr2 = selection.second.begin( );
			for( ; itr1 != selection.first.end( ); itr1++, itr2++ )
				if( *itr1 == i and *itr2 == addToPath( currDir, menuItemsText.at( i ) ) ){
					if( highlighted == i ) wbkgd( menuItems.at( i-start+1 ), COLOR_PAIR( 4 ) );
					else wbkgd( menuItems.at( i-start+1 ), COLOR_PAIR( 1 ) );
					break;
				}

			if( folderCheck or forcedCheck ){ //OPTIMIZED
				if( opendir( addToPath( currDir, menuItemsText.at( i ) ) ) != NULL )
					wprintw( menuItems.at( i-start+1 ), "%s/", ( menuItemsText.at( i ) ).c_str( ) );
				else wprintw( menuItems.at( i-start+1 ), "%s", ( menuItemsText.at( i ) ).c_str( ) );
				wclrtoeol( menuItems.at( i-start+1 ) );
			}
			else wprintw( menuItems.at( i-start+1 ), "%s", ( menuItemsText.at( i ) ).c_str( ) );

		}

		wrefresh( menuItems.at( 0 ) );

	}

	void deleteMenu( ){

		int end = mwY-2;
		if( menuItems.size( )-1 < end )
			end = menuItems.size( )-1;
		for( unsigned int i = 0; i < end; i++ )
			delwin( menuItems.at( i ) );
		menuItems.clear( );
		wrefresh( menuWindow );

	}

	pair< int, string > browseMenu( bool highlightStart = false, bool continueBrowsing = false ){

		if( !continueBrowsing ){

			wbkgd( menuItems.at( 1 ), COLOR_PAIR( 3 ) );

			highlighted = start; // highlighted->index, 0th index is the menu window!

			if( highlightStart ){

				wbkgd( menuItems.at( 1 ), COLOR_PAIR( 3 ) );
				auto itr1 = selection.first.begin( );
				auto itr2 = selection.second.begin( );
				for( ; itr1 != selection.first.end( ); itr1++, itr2++ )
					if( *itr1 == 0 and *itr2 == addToPath( currDir, menuItemsText.at( 0 ) ) ){
						wbkgd( menuItems.at( 1 ), COLOR_PAIR( 4 ) );
						break;
					}
				wnoutrefresh( menuItems.at( 1 ) );

				highlightStart = false;

			}

			wrefresh( menuItems.at( 0 ) );

		}

		for( int key = 'o'; 1; key = getch( ) ){

			if( key == KEY_DOWN or key == KEY_UP or key == PG_UP
				or key == PG_DOWN or key == KEY_HOME or key == KEY_END ){

				wbkgd( menuItems.at( highlighted-start+1 ), COLOR_PAIR( 2 ) );
				auto itr1 = selection.first.begin( );
				auto itr2 = selection.second.begin( );
				for( ; itr1 != selection.first.end( ); itr1++, itr2++ )
					if( *itr1 == highlighted and *itr2 == addToPath( currDir, menuItemsText.at( highlighted ) ) ){
						wbkgd( menuItems.at( highlighted-start+1 ), COLOR_PAIR( 1 ) );
						break;
					}
				wnoutrefresh( menuItems.at( highlighted-start+1 ) );

				if( key == KEY_DOWN )
					highlighted = ( highlighted+1 ) % ( menuItemsText.size( ) );
				else if( key == KEY_UP )
					highlighted = ( highlighted-1+menuItemsText.size( ) ) % ( menuItemsText.size( ) );
				else if( key == PG_DOWN ){
					if( highlighted == end-1 )
						highlighted = min( highlighted+(mwY-2)-1, menuItemsText.size( )-1 );
					else highlighted = end-1;
				}
				else if( key == PG_UP ){
					if( highlighted == start )
						highlighted = max( highlighted-(mwY-2)+1, 0 );
					else highlighted = start;
				}
				else if( key == KEY_HOME )
					highlighted = 0;
				else// if( key == KEY_END )
					highlighted = menuItemsText.size( )-1;

				deleteMenu( );
				drawMenu( menuItemsText ); //ff
				refresh( );

				wbkgd( menuItems.at( highlighted-start+1 ), COLOR_PAIR( 3 ) );
				itr1 = selection.first.begin( );
				itr2 = selection.second.begin( );
				for( ; itr1 != selection.first.end( ); itr1++, itr2++ )
					if( *itr1 == highlighted and *itr2 == addToPath( currDir, menuItemsText.at( highlighted ) ) ){
						wbkgd( menuItems.at( highlighted-start+1 ), COLOR_PAIR( 4 ) );
						break;
					}
				wnoutrefresh( menuItems.at( highlighted-start+1 ) );

				doupdate( );

			}
			else if( key == KEY_RIGHT or key == KEY_LEFT or key == ENTER ){

				if( key == KEY_LEFT ) highlighted = 0;
				if( key != KEY_RIGHT or ( key == KEY_RIGHT and /**/
					/**/ opendir( addToPath( currDir, menuItemsText.at( highlighted ) ) ) != NULL ) ){
					string nextPath = menuItemsText.at( highlighted );
					highlighted = 0;
					return make_pair( 'o', nextPath );
				}

			}
			else if( key == ESCAPE ){

				selection.first.clear( );
				selection.second.clear( );
				deleteMenu( );
				touchwin( menuWindow );
				refresh( );
				start = 0; highlighted = 0;
				end = min( mwY-2, menuItemsText.size( ) );
				drawMenu( menuItemsText, false, true );
				return browseMenu( true );

			}
			else if( key == ' ' ){
				if( menuItemsText.at( highlighted ) != ".." ){

					selection.first.push_back( highlighted );
					selection.second.push_back( addToPath( currDir, menuItemsText.at( highlighted ) ) );

					wbkgd( menuItems.at( highlighted-start+1 ), COLOR_PAIR( 4 ) );
					auto itr1 = selection.first.begin( );
					auto itr2 = selection.second.begin( );
					for( ; itr1 != selection.first.end( )-1; itr1++, itr2++ )
						if( *itr1 == highlighted and *itr2 == addToPath( currDir, menuItemsText.at( highlighted ) ) ){
							selection.first.erase( itr1 );
							selection.first.erase( selection.first.end( )-1 );
							selection.second.erase( itr2 );
							selection.second.erase( selection.second.end( )-1 );
							wbkgd( menuItems.at( highlighted-start+1 ), COLOR_PAIR( 3 ) );
							break;
						}
					wnoutrefresh( menuItems.at( highlighted-start+1 ) );
					doupdate( );

				}
			}
			else if( key == 'p' ){

				return make_pair( 'p', menuItemsText.at( highlighted ) );

			}
			else if( key != 'o' ){

				return make_pair( key, string( "DUMMY" ) );

			}

		}

	}

};
