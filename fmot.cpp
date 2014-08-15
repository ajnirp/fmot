#include "menu&Interface.hpp"
#include "luaSearching.hpp"
#include "fileIndexing.hpp"
#include "renaming.hpp"
#include "shiftFiles.hpp"
#include "fileHandling.hpp"

#include <algorithm>
#include <curses.h>
#include <string>
#include <unistd.h>
#include <vector>
using namespace std;

WINDOW *menuWindow, *titleWindow, *bottomWindow, *messageBar, *statusBar;
database home;

void printInBar( string text, WINDOW *xBar ){

	wmove( xBar, 0, 0 );
	wclrtoeol( xBar );
	wattron( xBar, A_BOLD );
	mvwprintw( xBar, 0, 1, text.c_str( ) );
	wattroff( xBar, A_BOLD );
	wrefresh( xBar );

}

void uglyWindowKeeping( string currDir ){

	curs_set( 0 );
	wrefresh( titleWindow );
	wattron( titleWindow, A_BOLD );
	wmove( titleWindow, 0, (COLS-31)/2 );
	wprintw( titleWindow, "FMoT (File Manager on Terminal)" );
	wattroff( titleWindow, A_BOLD );
	printInStatusBar( currDir );
	wrefresh( bottomWindow );
	wrefresh( messageBar );
	wrefresh( statusBar );
	werase( menuWindow );

}

WINDOW * showWindow( int height, int width, string title, vector< string >& dialog, bool centralized = true ){

	WINDOW * popup = newwin( height, width, (LINES-height)/2, (COLS-width)/2 );
	box( popup, ACS_VLINE, ACS_HLINE );
	WINDOW * popupTitle = derwin( popup, 1, width, 0, 0 );
	WINDOW * popupDialog = derwin( popup, height-2, width-2, 1, 1 );
	wbkgd( popup, COLOR_PAIR( 2 ) );
	wbkgd( popupTitle, COLOR_PAIR( 5 ) );
	wbkgd( popupDialog, COLOR_PAIR( 2 ) );
	wattron( popupTitle, A_BOLD );
	wmove( popupTitle, 0, 0 ); wclrtoeol( popupTitle );
	wmove( popupTitle, 0, ( width-title.length( ) )/2 );
	wprintw( popupTitle, title.c_str( ) );
	wattroff( popupTitle, A_BOLD );

	for( int i = 0, end = min( height-2, dialog.size( ) ); i < end ; i++ ){
		if( width-dialog[ i ].length( ) > 0 )
			wmove( popupDialog, i, ( width-dialog[ i ].length( ) )/2 );
		else
			wmove( popupDialog, i, 0 );
		wprintw( popupDialog, dialog[ i ].c_str( ) );
	}

	wrefresh( popup );
	doupdate( );
	refresh( );

	return popup;

}

int main( ){

	bool helpFlag = true;

	char wd[ 10000 ];
	getcwd( wd, 10000 );
	string currDir( wd );

	initscr( );

	menuWindow = newwin( LINES-4, COLS, 1, 0 );
	titleWindow = newwin( 1, COLS, 0, 0 );
	bottomWindow = newwin( 3, COLS, LINES-3, 0 );
	messageBar = derwin( bottomWindow, 1, COLS, 1, 0 );
	statusBar = derwin( bottomWindow, 1, COLS, 0, 0 );
	WINDOW * staticBar = derwin( bottomWindow, 1, COLS, 2, 0 );
	customMenu fileBrowser( menuWindow );

	wbkgd( bottomWindow, COLOR_PAIR( 5 ) );
	wbkgd( messageBar, COLOR_PAIR( 2 ) );
	wbkgd( statusBar, COLOR_PAIR( 5 ) );
	wbkgd( staticBar, COLOR_PAIR( 5 ) );
	wbkgd( titleWindow, COLOR_PAIR( 5 ) );

	bkgd( COLOR_PAIR( 2 ) );

	wattron( titleWindow, A_BOLD );
	wmove( titleWindow, 0, (COLS-31)/2 );
	wprintw( titleWindow, "FMoT (File Manager on Terminal)" );
	wattroff( titleWindow, A_BOLD );

	printInStatusBar( currDir );

	printInBar( "(h)elp (<-) up one dir (->) into dir (SPACE) select (ENTER) open (q)uit", staticBar );

	doupdate( );
	refresh( );

	string title = "Visual Terminal File Manager";
	vector< string > remark;
	remark.push_back( string( "Welcome to FMoT!" ) );
	remark.push_back( string( "Toggle help dialogs by pressing '\?'" ) );
	WINDOW * temp = showWindow( 4, COLS/2, title, remark );
	if( getch( ) == 'q' ){ endwin( ); return 1; }
	delwin( temp );

	bool openDialog = false, showFiles = true;
	for( int key = 'r', keyPrev = 'b'; key != 'q'; key = getch( ) ){

		string nextPath;
		vector< string > files;

		skipGetCH:

		if( key == 'o' ){

			wclear( menuWindow );
			if( extractDirName( nextPath ) == ".." ){
				if( !goBack( currDir ) )
					currDir = string( wd );
			}
			else if( opendir( nextPath ) != NULL ){
				currDir = nextPath;
			}
			else{

				open_file( nextPath );

				/*if( openDialog ){
					string title( "Open Operation Requested" );
					vector< string > remark;
					remark.push_back( string( "Trying to open file: " ) + extractDirName( nextPath ) );
					remark.push_back( "Press 'ENTER' to close this dialog" );
					remark.push_back( "or 'ESCAPE' to disable it permanently!" );
					WINDOW * temp = showWindow( 5, max( 42, extractDirName( nextPath ).length( )+25 ), title, remark );
					for( int key = 0; 1; key = getch( ) ){
						if( key == ENTER or key == ESCAPE ){
							if( key == ESCAPE )
								openDialog = false;
							break;
						}
					}
					delwin( temp );
				}*/

				// Retain the highlighting
				// as merely opening file!
				//key = TAB; keyPrev = TAB;

			}
			cleanPath( currDir );
			printInStatusBar( currDir );

		}
		if( key == 'q' ){

			break;

		}
		else{

			uglyWindowKeeping( currDir );

			if( key == '?' ){

				string title; vector< string > remark;
				remark.push_back( string( "Press 'ENTER' to confirm." ) );
				if( helpFlag ) title = "Disable help?";
				else title = "Enable help?";
				WINDOW * temp = showWindow( 3, COLS/2, title, remark );
				if( getch( ) == ENTER ) helpFlag = !helpFlag;
				delwin( temp );
				if( helpFlag ){
					printInMessageBar( string( "Help currently enabled!" ) );
				}
				else{
					printInMessageBar( string( "Help currently disabled!" ) );
				}
				showFiles = false;
				goto displayDefault;

			}
			else if( key == 'h' ){
				if( helpFlag ){

					printInMessageBar( string( "Waiting for you to close help..." ) );

					string title = "Help Page";
					vector< string > remark;

					remark.push_back( string( "(f)ilter: searches files in current directly only." ) );
					remark.push_back( string( "(s)earch: recursive, reflects real-time changes." ) );
					remark.push_back( string( "(/)search: using an index built only on changing dir." ) );
					remark.push_back( string( "(g)o to: jumps to any absolute/relative path." ) );
					remark.push_back( string( "(r)efresh/(b)ack: reflects real-time changes in dir." ) );
					remark.push_back( string( "(<-)left arrow key: goes to parent dir." ) );
					remark.push_back( string( "(->)right arrow key: opens highlighted folder." ) );
					remark.push_back( string( "(ENTER): opens a file (using associated..." ) );
					remark.push_back( string( "...program, if any) or folder (in terminal)." ) );
					remark.push_back( string( "(p): displays properties of a file." ) );
					remark.push_back( string( "(c)opy: copy selected files/folders to current dir." ) );
					remark.push_back( string( "(m)ove: move selected files/folders to current dir." ) );
					remark.push_back( string( "(DEL)ete: removes selected files/folders from system." ) );
					remark.push_back( string( "(F2): rename selected file (not in batch)." ) );
					remark.push_back( string( "(ESC): clear selection (if any)." ) );

					remark.push_back( string( "-----Press 'h' (again) to close-----" ) );

					WINDOW * temp = showWindow( min( LINES*3/4, remark.size( )+2 ), /**/
						/**/ COLS*3/4, title, remark, true );
					while( getch( ) != 'h' );
					delwin( temp );

					char fileCount[ 100 ];
					sprintf( fileCount, "Files: %d", max( files.size( )-1, 0 ) );
					printInMessageBar( string( fileCount ) );

				}
				else{

					printInMessageBar( string( "Help is disabled. Please re-enable it to see page!" ) );
					showFiles = false;

				}
				goto displayDefault;
			}
			else if( key == 'p' ){

				if( opendir( nextPath ) != NULL ){

					printInMessageBar( string( "Shows attributes of files only :(" ) );

					string title = "File Properties";
					vector< string > remark;
					remark.push_back( string( "" ) );
					remark.push_back( string( "Folders not yet supported :(" ) );
					remark.push_back( string( "" ) );
					remark.push_back( string( "Press any key to close to close." ) );
					remark.push_back( string( "" ) );

					WINDOW * temp = showWindow( min( LINES*3/4, remark.size( )+2 ), /**/
						/**/ min( 40, COLS-4 ), title, remark, false );
					getch( );
					delwin( temp );

				}
				else{

					printInMessageBar( string( "Showing file attributes..." ) );

					string title = "File Properties";
					vector< string > remark;
					remark.push_back( string( "" ) );
					string attrib = file_data( nextPath );
					remark.push_back( attrib );
					remark.push_back( string( "" ) );
					remark.push_back( string( "Press any key to close." ) );
					remark.push_back( string( "" ) );

					WINDOW * temp = showWindow( min( LINES*3/4, remark.size( )+2 ), /**/
						/**/ min( attrib.length( )+4, COLS-4 ), title, remark, false );
					getch( );
					delwin( temp );

				}

				goto displayDefault;

			}
			else if( key == 'm' or key == 'c' or key == KEY_F(2) or key == KEY_DC ){

				if( helpFlag ){}

				int minW;
				WINDOW *temp;

				if( fileBrowser.selection.second.size( ) == 0 ){

					string title = "Redundant Operation Requested";
					vector< string > remark;
					if( key == 'm' ) remark.push_back( string( "Nothing to move :(" ) );
					else if( key == 'c' ) remark.push_back( string( "Nothing to copy :(" ) );
					else if( key == KEY_DC ) remark.push_back( string( "Nothing to delete :(" ) );
					else remark.push_back( string( "Nothing selected to rename :(" ) );
					remark.push_back( string( "Press ENTER to close." ) );
					WINDOW * temp = showWindow( min( 4, LINES/2 ), 40, title, remark, false );
					while( getch( ) != ENTER );
					delwin( temp );
					if( key == 'm' ){ printInMessageBar( string( "Nothing to move." ) ); }
					else if( key == 'c' ){ printInMessageBar( string( "Nothing to copy." ) ); }
					else if( key == KEY_DC ){ printInMessageBar( string( "Nothing to delete." ) ); }
					else{ printInMessageBar( string( "Nothing selected to rename." ) ); }
					showFiles = false;
					goto displayDefault;

				}
				else{

					string title;
					if( key == 'm' ) title = "Move Operation Requested";
					else if( key == 'c' ) title = "Copy Operation Requested";
					else if( key == KEY_DC ) title = "Delete Operation Requested";
					else title = "Rename Operation Requested";

					vector< string > remark;
					if( key == KEY_F(2) ){

						if( fileBrowser.selection.second.size( ) != 1 ) goto skipRename;

						remark.push_back( string( "Confirm renaming the file" ) );
						remark.push_back( enclosed( extractDirName( fileBrowser.selection.second[ 0 ] ) ) );
						remark.push_back( string( "by pressing 'ENTER'. Then" ) );
						remark.push_back( string( "type the new name below..." ) );

					}
					else{
						if( key == KEY_DC ) remark.push_back( string( "Confirm deleting files?" ) );
						else{
							if( key == 'm' ) remark.push_back( string( "Confirm moving files to the current" ) );
							else remark.push_back( string( "Confirm copying files to the current" ) );
							remark.push_back( string( "directory: " ) + enclosed( extractDirName( currDir ) ) );
						}
						remark.push_back( string( "Press 'ENTER' to confirm operation." ) );
						string fileNames( "( Files: " );
						int i = 0;
						while( i < fileBrowser.selection.second.size( )-1 )
							fileNames.append( extractDirName( fileBrowser.selection.second.at( i++ ) ) + string( ", " ) );
						fileNames.append( extractDirName( fileBrowser.selection.second.at( i ) ) + string( " )" ) );
						remark.push_back( fileNames );
					}

					minW = COLS/2;
					for( auto itr = remark.begin( ); itr != remark.end( ); itr++ )
						minW = max( minW, itr->size( )+4 );
					temp = showWindow( min( max( fileBrowser.selection.second.size( )/2, 1 )+remark.size( )+1, LINES/2 ),
						min( minW, COLS*3/4 ), title, remark, false );

					if( getch( ) == ENTER ){

						if( key == 'm' ){

							printInMessageBar( string( "Move operation confirmed and started." ) );
							if( shift_files( "mv", fileBrowser.selection.second, currDir ) ){
								fileBrowser.selection.first.clear( );
								fileBrowser.selection.second.clear( );
								printInMessageBar( string( "Move operation successful :)" ) );
							}
							else{ printInMessageBar( string( "Move operation partially/fully failed :(" ) ); }

						}
						else if( key == KEY_DC ){

							printInMessageBar( string( "Delete operation confirmed and started." ) );
							bool success = true;
							for( auto itr = fileBrowser.selection.second.begin( ); itr != fileBrowser.selection.second.end( ); itr++ )
								if( not remove_file( *itr ) ) success = false;
							if( success ){
								fileBrowser.selection.first.clear( );
								fileBrowser.selection.second.clear( );
								printInMessageBar( string( "Delete operation successful :)" ) );
							}
							else{ printInMessageBar( string( "Delete operation partially/fully failed :(" ) ); }

						}
						else if( key == 'c' ){

							printInMessageBar( string( "Copy operation confirmed and started." ) );
							if( shift_files( "cp", fileBrowser.selection.second, currDir ) ){
								fileBrowser.selection.first.clear( );
								fileBrowser.selection.second.clear( );
								printInMessageBar( string( "Copy operation successful :)" ) );
							}
							else{ printInMessageBar( string( "Copy operation partially/fully failed :(" ) ); }

						}
						else{

							printInMessageBar( string( "Rename to: " ) );
							char newName[ 256 ]; echo( ); wgetstr( messageBar, newName ); noecho( );
							printInMessageBar( string( "Rename operation confirmed and started." ) );
							string wDir = fileBrowser.selection.second[ 0 ];
							goBack( wDir );
							if( rename_file( fileBrowser.selection.second[ 0 ], addToPath( wDir, string( newName ) ) ) ){
								fileBrowser.selection.first.clear( );
								fileBrowser.selection.second.clear( );
								printInMessageBar( string( "Rename operation successful :)" ) );
							}
							else{ printInMessageBar( string( "Rename operation failed :(" ) ); }

						}

					}
					else{

						if( key == 'm' ){ printInMessageBar( string( "Move operation cancelled." ) ); }
						else if( key == 'c' ){ printInMessageBar( string( "Copy operation cancelled." ) ); }
						else if( key == KEY_DC ){ printInMessageBar( string( "Delete operation cancelled." ) ); }
						else{ printInMessageBar( string( "Rename operation cancelled." ) ); }

					}

					delwin( temp );

					skipRename:

					showFiles = false;
					goto displayDefault;

				}

			}
			else if( key == 'g' ){

				printInMessageBar( string( "Go to: " ) );

				char goToDir[ 256 ]; echo( );
				wgetstr( messageBar, goToDir ); noecho( );

				if( goToDir[ 0 ] != '/' ){
					if( listingFilePossible( addToPath( currDir, string( goToDir ) ) ) )
						currDir = addToPath( currDir, string( goToDir ) );
				}
				else if( listingFilePossible( string( goToDir ) ) ){
					currDir.clear( );
					currDir = string( goToDir );
				}
				cleanPath( currDir );
				printInStatusBar( currDir );

				fileBrowser.setCurrentDirectory( currDir );

				listFilesInDirectory( files, currDir );
				fileBrowser.drawMenu( files, true );

				auto tmp = fileBrowser.browseMenu( true );
				while( notHotkey( tmp.first ) ) tmp = fileBrowser.browseMenu( false, true );
				key = tmp.first; nextPath = addToPath( currDir, tmp.second );

				if( key != TAB )
					goto skipGetCH;

			}
			else if( key == 'f' or key == 's' ){

				if( key == 'f' ){ printInMessageBar( string( "Filter: " ) ); }
				else{ printInMessageBar( string( "Search: " ) ); }

				echo( );
				string filterPattern;
				for( int c = wgetch( messageBar ); c != '\n'; c = wgetch( messageBar ) ){

					filterPattern.push_back( ( char ) c );

					uglyWindowKeeping( currDir );

					vector< string > filteredFiles;
					if( key == 'f' ) simpleSearch( currDir, filterPattern, filteredFiles, true );
					else simpleSearch( currDir, filterPattern, filteredFiles, false );

					fileBrowser.setCurrentDirectory( string( "" ) );
					fileBrowser.drawMenu( filteredFiles, true );

					doupdate( );
					touchwin( menuWindow );
					refresh( );
					wrefresh( messageBar );
					wrefresh( statusBar );

				}
				noecho( );

				auto tmp = fileBrowser.browseMenu( true );
				while( notHotkey( tmp.first ) ) tmp = fileBrowser.browseMenu( false, true );
				key = tmp.first; nextPath = tmp.second;

				if( key != TAB )
					goto skipGetCH;

			}
			else if( key == '/' ){

				printInMessageBar( string( "Indexing, please wait..." ) );

				if( home.build( currDir ) ){ printInMessageBar( string( "Lookup fresh: " ) ); }
				else{ printInMessageBar( string( "Lookup index: " ) ); }

				char patternSearch[ 256 ]; echo( );
				wgetstr( messageBar, patternSearch ); noecho( );

				files.clear( );
				files = home.search( string( patternSearch ) );

				fileBrowser.drawMenu( files, true );

				auto tmp = fileBrowser.browseMenu( true );
				while( notHotkey( tmp.first ) ) tmp = fileBrowser.browseMenu( false, true );
				key = tmp.first; nextPath = tmp.second;

				if( key != TAB )
					goto skipGetCH;

			}
			else{ // Resort to file browsing.

				char fileCount[ 100 ];
				sprintf( fileCount, "Files: %d", files.size( ) );
				if( showFiles ){
					printInMessageBar( string( fileCount ) );
				}
				else showFiles = true;

				displayDefault:

				fileBrowser.setCurrentDirectory( currDir );

				//if( key != TAB or ( key == TAB and keyPrev == TAB ) ){
				if( key != TAB or keyPrev == TAB or key == 'r' or key == 'b' ){ // TAB then TAB WORKS!

					listFilesInDirectory( files, currDir );

					char fileCount[ 100 ];
					sprintf( fileCount, "Files: %d", max( files.size( )-1, 0 ) );
					if( showFiles ){
						printInMessageBar( string( fileCount ) );
					}
					else showFiles = true;

					fileBrowser.drawMenu( files, keyPrev != TAB );

				}
				else{

					fileBrowser.drawMenu( files, true, true );

				}

				auto tmp = fileBrowser.browseMenu( keyPrev != TAB, keyPrev == TAB );
				while( notHotkey( tmp.first ) ) tmp = fileBrowser.browseMenu( false, true );
				key = tmp.first; nextPath = addToPath( currDir, tmp.second );

				if( key != TAB )
					goto skipGetCH;

			}

			vector< string > selectedItems = fileBrowser.selection.second;

			wmove( messageBar, 0, 1 );
			wclrtoeol( messageBar );

			if( selectedItems.empty( ) )
				wprintw( messageBar, "Nothing selected." );
			else{
				wprintw( messageBar, "Selected file(s): " );
				int i = 0; while( i < selectedItems.size( )-1 )
					wprintw( messageBar, "%s, ", extractDirName( selectedItems.at( i++ ) ).c_str( ) );
				wprintw( messageBar, "%s.", extractDirName( selectedItems.at( i ) ).c_str( ) );
			}

			doupdate( );
			touchwin( menuWindow );
			refresh( );
			wrefresh( messageBar );
			wrefresh( statusBar );

		}

		keyPrev = key;

	}

	system( string( "rm -f 2>/dev/null database" ) );

	endwin( );
	return 0;

}
