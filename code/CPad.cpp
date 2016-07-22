// ========================================================================
// CPad.cpp							(c)1997 Jesse Hall. All rights reserved.
// ========================================================================

#include "CLine.h"
#include "CPad.h"
#include "Pad.h"
#include <stdio.h>

// ------------------------------------------------------------------- CPad
CPad::CPad( BEntry *entry )
	: BWindow( BRect( 200.0, 100.0, 500.0, 300.0 ), "Pad", B_TITLED_WINDOW,
			B_NOT_ZOOMABLE ), mEntry( NULL ), mScroll( NULL ),
			mFooter( NULL ), mMenu( NULL ), mCurPage( 0 ) {

	// Create the interface
	SetupWindow();
	
	// Set the size limites of the window
	float minW, minH, maxW, maxH;
	GetSizeLimits( &minW, &maxW, &minH, &maxH );
	minW = 225.0;
	minH = 100.0;
	SetSizeLimits( minW, maxW, minH, maxH );

	// Set mEntry
	if( entry ) {
		mEntry = entry;
	} else {
		mEntry = GetDefaultPad();
	}
	
	// Try to read the pad file
	bool success = true;
	if( !mEntry->Exists() ) {
		success = false;
	} else {
		BFile file( mEntry, B_READ_ONLY );

		// Find out what version it is
		uint32 info[ 3 ];
		if( file.ReadAttr( kInfoAttr, B_RAW_TYPE, 0, &info,
				sizeof( info ) ) == sizeof( info ) ) {

			// Call the appropriate function based on version
			switch( info[ 0 ] ) {
				case 0x0100:		// Version 1.0
					success = ReadPad100( file );
					break;
				default:
					success = false;
					break;
			}
		} else {
			success = false;
		}
		
		// The file exists but isn't able to be read
		if( !success ) {
			BAlert *alert = new BAlert( NULL, "The pad file is corrupted!",
					"Damn", NULL, NULL, B_WIDTH_AS_USUAL, B_WARNING_ALERT );
			alert->Go();
		}
	}
	
	// If we haven't successfully opened a pad by now, create a blank one
	if( !success ) {
		NewPad();
	}
}

// ------------------------------------------------------------------ ~CPad
CPad::~CPad( void ) {
	// Delete the pad file entry
	if( mEntry ) {
		delete mEntry;
	}
	
	// Destroy all pages
	try {
		RemoveChild( mPages.Retrieve( mCurPage - 1 ) );
	} catch( ... ) {}
	while( !mPages.IsEmpty() ) {
		delete mPages.Remove( 0 );
	}
}

// -------------------------------------------------------- MessageReceived
void CPad::MessageReceived( BMessage *msg ) {
	switch( msg->what ) {
		case msg_Next:
			NextPage();
			break;
		case msg_Prev:
			PrevPage();
			break;
		case msg_New:
			NewPage();
			break;
		case msg_Del:
			DelPage();
			break;
		default:
			BWindow::MessageReceived( msg );
			break;
	}
}

// ---------------------------------------------------------- QuitRequested
bool CPad::QuitRequested( void ) {
	// Save the document
	SaveRequested();

	// Tell the application the window has closed
	be_app->PostMessage( msg_PadClosed );
	return true;
}

// ---------------------------------------------------------- SaveRequested
void CPad::SaveRequested( void ) {
	// Open the file
	BFile file( mEntry, B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE );
	
	// Collect the information to put into the info attribute
	uint32 info[ 3 ];
	info[ 0 ] = 0x0100;					// Version 1.0
	info[ 1 ] = mPages.Length();		// Number of pages
	info[ 2 ] = mCurPage;				// Current page
	
	// Step through the pages, writing the page's text to the file and
	// adding its length to an array that will become the pages attribute
	uint32 *pages = new uint32[ info[ 1 ] ];
	for( int i = 0; i < info[ 1 ]; i++ ) {
		pages[ i ] = mPages.Retrieve( i )->TextLength();
		file.Write( mPages.Retrieve( i )->Text(), pages[ i ] );
	}
		
	// Finally, write the attributes
	file.WriteAttr( "BEOS:TYPE", B_MIME_TYPE, 0, kPadMimeType,
			strlen( kPadMimeType ) );
	file.WriteAttr( kInfoAttr, B_RAW_TYPE, 0, &info, sizeof( info ) );
	file.WriteAttr( kPagesAttr, B_RAW_TYPE, 0, pages,
			sizeof( *pages ) * info[ 1 ] );
}

// --------------------------------------------------------------- NextPage
void CPad::NextPage( void ) {
	// If the next page already exists, go to it
	if( mCurPage < mPages.Length() ) {
		GoToPage( mCurPage + 1 );

	// If not, ask to create it
	} else {
		BAlert *alert = new BAlert( NULL,
				"This is the last page.\nCreate a new one?", "No", "Yes",
				NULL, B_WIDTH_AS_USUAL, B_IDEA_ALERT );
		alert->SetShortcut( 2, B_ESCAPE );
		int32 result = alert->Go();
		
		// User wanted a new window
		if( result == 1 ) {
			NewPage();
		}
	}
}

// --------------------------------------------------------------- PrevPage
void CPad::PrevPage( void ) {
	// If this isn't the first page, go to the previous page
	if( mCurPage > 1 ) {
		GoToPage( mCurPage - 1 );
	
	// If not, let the user know
	} else {
		BAlert *alert = new BAlert( NULL, "This is the first page!",
				"OK", NULL, NULL, B_WIDTH_AS_USUAL, B_IDEA_ALERT );
		alert->Go();
	}
}

// ---------------------------------------------------------------- NewPage
void CPad::NewPage( void ) {
	// Create the new page and add it to the page list
	BRect bounds( 0, mMenu->Frame().bottom + 1,
			mScroll->Frame().left - 1, mFooter->Frame().top - 2 );
	mPages.Add( mCurPage, new CText( bounds, "Text", B_FOLLOW_ALL_SIDES,
			B_WILL_DRAW | B_NAVIGABLE ) );

	// Go to the new page
	NextPage();
}

// ---------------------------------------------------------------- DelPage
void CPad::DelPage( void ) {
	// Make sure user wants to do this
	BAlert *alert = new BAlert( NULL,
			"Deleting a note is not undoable.\nDo you really want to do this?",
			"Cancel", "OK", NULL, B_WIDTH_AS_USUAL, B_WARNING_ALERT );
	alert->SetShortcut( 1, B_ENTER );
	alert->SetShortcut( 0, B_ESCAPE );
	int32 result = alert->Go();

	// If they really want to delete the page
	if( result == 1 ) {
	
		uint16 page = mCurPage;
		// If this is the first page...
		if( mCurPage == 1 ) {
			// If its also the only page, create a new one
			if( mPages.Length() == 1 ) {
				NewPage();
			
			// If not, simply move to the next one
			} else {
				NextPage();
			}
			mCurPage--;
		
		// If its the last page (and not the first)...
		} else {
			PrevPage();
		}
		
		// Destroy the page and set the page number
		delete mPages.Remove( page - 1 );
		SetPageNum();
	}
}

// --------------------------------------------------------------- GoToPage
void CPad::GoToPage( uint16 num ) {
	// If there is currently a page shown, remove it
	BRect frame;
	CText *page;
	if( mCurPage > 0 ) {
		page = mPages.Retrieve( mCurPage - 1 );
		page->Select( 0, 0 );
		page->ScrollToSelection();
		frame = page->Frame();
		RemoveChild( page );
	} else {
		frame.Set( 0, mMenu->Frame().bottom + 1, mScroll->Frame().left - 1,
				mFooter->Frame().top - 3 );
	}
		
	// Add the page requested
	page = mPages.Retrieve( num - 1 );
	mScroll->SetTarget( page );
	AddChild( page );
	page->ResizeTo( frame.right - frame.left, frame.bottom - frame.top );
	page->MakeFocus();
	mCurPage = num;
	SetPageNum();
}

// ---------------------------------------------------------- GetDefaultPad
BEntry *CPad::GetDefaultPad( bool /*create*/ ) {
	// For now, simply put default pad in /boot/system/settings
	return new BEntry( "/boot/system/settings/Pad_default" );
}

// ----------------------------------------------------------------- NewPad
void CPad::NewPad( void ) {
	// Create a single page and add it to the list
	BRect bounds( 0, mMenu->Frame().bottom + 1, mScroll->Frame().left - 1,
			mFooter->Frame().top - 2 );
	CText *page = new CText( bounds, "Text", B_FOLLOW_ALL_SIDES,
			B_WILL_DRAW | B_NAVIGABLE );
	mPages.Add( 0, page );
	
	// Go to the page
	mCurPage = 1;
	GoToPage( mCurPage );
}

// ------------------------------------------------------------- ReadPad100
bool CPad::ReadPad100( BFile &file ) {
	bool success = true;

	// Read the number of pages and current page
	uint32 info[ 3 ];
	if( file.ReadAttr( kInfoAttr, B_RAW_TYPE, 0, &info, sizeof( info ) )
			== sizeof( info ) ) {
		
		// Get the array of page sizes
		uint32 *pages = new uint32[ info[ 1 ] ];
		if( file.ReadAttr( kPagesAttr, B_RAW_TYPE, 0, pages, info[ 1 ] *
				sizeof( *pages ) ) == info[ 1 ] * sizeof( *pages ) ) {
			
			// Create the pages
			char *text = NULL;
			for( int i = 0; i < info[ 1 ]; i++ ) {
				// Read the page text
				text = new char[ pages[ i ] ];
				file.Read( text, pages[ i ] );

				// Create the new page and add it to the page list
				BRect bounds( 0, mMenu->Frame().bottom + 1,
						mScroll->Frame().left - 1, mFooter->Frame().top - 2 );
				mPages.Add( i, new CText( bounds, "Text", B_FOLLOW_ALL_SIDES,
						B_WILL_DRAW | B_NAVIGABLE ) );

				// Set the page's text
				mPages.Retrieve( i )->SetText( text, pages[ i ] );
				delete [] text;
			}
			
			// Go to the current page
			GoToPage( info[ 2 ] );
		
		// Failed reading pages attribute
		} else {
			success = false;
		}
	// Failed reading info	attribute
	} else {
		success = false;
	}
	return success;
}

// ------------------------------------------------------------- SetPageNum
void CPad::SetPageNum( void ) {
	char pagestr[ 31 ];
	sprintf( pagestr, "page %d of %d", mCurPage, mPages.Length() );
	mFooter->SetText( pagestr );
}

// ------------------------------------------------------------ SetupWindow
void CPad::SetupWindow( void ) {
	BRect bounds = Bounds();

	// Create menu bar
	mMenu = new BMenuBar( BRect( bounds.left, bounds.top, bounds.right,
			bounds.top + 2 ), "MenuBar" );

	// File menu
	BMenu *menu = new BMenu( "File" );
	BMenuItem *item = new BMenuItem( "About Pad...",
			new BMessage( B_ABOUT_REQUESTED ) );
	item->SetTarget( be_app );
	menu->AddItem( item );

	item = new BMenuItem( "Quit", new BMessage( B_QUIT_REQUESTED ) );
	item->SetTarget( be_app );
	menu->AddItem( item );
	
	mMenu->AddItem( menu );
	
	// Edit menu
	menu = new BMenu( "Edit" );
	item = new BMenuItem( "Cut", new BMessage( B_CUT ), 'X' );
	menu->AddItem( item );
	item->SetTarget( NULL, this );
	
	item = new BMenuItem( "Copy", new BMessage( B_COPY ), 'C' );
	menu->AddItem( item );
	item->SetTarget( NULL, this );
	
	item = new BMenuItem( "Paste", new BMessage( B_PASTE ), 'V' );
	menu->AddItem( item );
	item->SetTarget( NULL, this );
	
	mMenu->AddItem( menu );
	
	// Note menu
	menu = new BMenu( "Note" );
	item = new BMenuItem( "Next note", new BMessage( msg_Next ) );
	menu->AddItem( item );
	
	item = new BMenuItem( "Previous note", new BMessage( msg_Prev ) );
	menu->AddItem( item );
	
	menu->AddSeparatorItem();
	
	item = new BMenuItem( "New note", new BMessage( msg_New ) );
	menu->AddItem( item );
	
	item = new BMenuItem( "Delete note", new BMessage( msg_Del ) );
	menu->AddItem( item );
	
	mMenu->AddItem( menu );
	
	// Add the menu bar
	AddChild( mMenu );
	BRect menuframe = mMenu->Frame();
		
	// Create the page buttons
	float width, height;
	BButton *button1 = new BButton( BRect( 0, bounds.bottom - 2,
			2, bounds.bottom ), "Prev", "Prev", new BMessage( msg_Prev ),
			B_FOLLOW_LEFT | B_FOLLOW_BOTTOM, B_WILL_DRAW );
	AddChild( button1 );
	button1->GetPreferredSize( &width, &height );
	button1->MoveTo( 0, bounds.bottom - height );
	button1->ResizeTo( width, height );
	BRect b1frame = button1->Frame();

	BButton *button2 = new BButton( BRect( bounds.right - 2,
			bounds.bottom - 2, bounds.right, bounds.bottom ), "Next",
			"Next", new BMessage( msg_Next ), B_FOLLOW_RIGHT |
			B_FOLLOW_BOTTOM, B_WILL_DRAW );
	AddChild( button2 );
	button2->GetPreferredSize( &width, &height );
	button2->MoveTo( bounds.right - width, bounds.bottom - height );
	button2->ResizeTo( width, height );
	BRect b2frame = button2->Frame();
	
	// Create footer
	mFooter = new BStringView( BRect( b1frame.right + 1,
			bounds.bottom - height, b2frame.left - 1,
			bounds.bottom ), "Footer", 0, B_FOLLOW_LEFT_RIGHT |
			B_FOLLOW_BOTTOM, B_WILL_DRAW );
	mFooter->SetAlignment( B_ALIGN_CENTER );
	AddChild( mFooter );
	
	// Create line
	CLine *line = new CLine( BRect( 0, b1frame.top - 1, bounds.right,
			b1frame.top - 1 ), "Line", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM,
			0, CLine::horiz );
	AddChild( line );
	
	// Create scrollbar
	mScroll = new BScrollBar( BRect( bounds.right - B_V_SCROLL_BAR_WIDTH,
			menuframe.bottom + 1, bounds.right, b2frame.top - 2 ),
			"ScrollBarV", NULL, 0.0, 0.0, B_VERTICAL );
	AddChild( mScroll );
}
