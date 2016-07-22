// ------------------------------------------------------------------------
// CPadApp.cpp						(c)1997 Jesse Hall. All rights reserved.
// ------------------------------------------------------------------------

#include "CPadApp.h"
#include "Pad.h"
#include <Alert.h>
#include <Entry.h>

// ---------------------------------------------------------------- CPadApp
CPadApp::CPadApp( void )
	: BApplication( "application/x-jessehall-pad" ), mPad( NULL ),
			mIgnorePadClosed( false ) {
}

// --------------------------------------------------------------- ~CPadApp
CPadApp::~CPadApp( void ) {
}

// --------------------------------------------------------- AboutRequested
void CPadApp::AboutRequested( void ) {
	BAlert *alert = new BAlert( NULL, "Pad 1.00\nby Jesse Hall", "OK" );
	alert->Go( NULL );
}

// -------------------------------------------------------- MessageReceived
void CPadApp::MessageReceived( BMessage *msg ) {
	switch( msg->what ) {
		case msg_PadClosed:
			if( !mIgnorePadClosed ) {
				Quit();
			} else {
				mIgnorePadClosed = false;
			}
			break;
		default:
			BApplication::MessageReceived( msg );
			break;
	}
}

// ------------------------------------------------------------- ReadyToRun
void CPadApp::ReadyToRun( void ) {
	// If no pad has been created yet, create the default one
	if( !mPad ) {
		mPad = new CPad;
		mPad->Show();
	}
}

// ----------------------------------------------------------- RefsReceived
void CPadApp::RefsReceived( BMessage *msg ) {
	entry_ref ref;
	uint32	type;
	int32	count;
	
	// Get ref information
	msg->GetInfo( "refs", &type, &count );
	if( type == B_REF_TYPE ) {

		// Loop through the refs until we get to a Pad file
		bool good = false;
		BEntry *entry = NULL;
		for( int i = 0; i < count && !good; i++ ) {
			if( msg->FindRef( "refs", i, &ref ) == B_OK ) {

				// Create a BEntry from the ref
				entry = new BEntry( &ref );
				if( entry->IsFile() ) {
					good = true;
				} else {
					delete entry;
					entry = NULL;
				}
			}
		}		
		
		// If we found a good file
		if( good ) {

			// If a pad is already open, close it
			if( mPad ) {
				mIgnorePadClosed = true;
				mPad->PostMessage( B_QUIT_REQUESTED, mPad );
			}
	
			// Create a new pad from the file
			mPad = new CPad( entry );
			mPad->Show();
		}
	}
}