// ------------------------------------------------------------------------
// CLine.cpp						(c)1997 Jesse Hall. All rights reserved.
// ------------------------------------------------------------------------

#include "CLine.h"

// ------------------------------------------------------------------ CLine
CLine::CLine( BRect frame, const char *name, uint32 resizingMode,
		uint32 flags, uint8 dir )
	: BView( frame, name, resizingMode, flags | B_WILL_DRAW ) {
	
	// Set direction
	if( dir == horiz ) {
		mDir = horiz;
	} else {
		mDir = vert;
	}
}

// ----------------------------------------------------------------- ~CLine
CLine::~CLine( void ) {
}

// ------------------------------------------------------------------- Draw
void CLine::Draw( BRect ) {
	BRect bounds = Bounds();
	if( mDir == horiz ) {
		StrokeLine( BPoint( 0, bounds.bottom / 2.0 ), BPoint( bounds.right,
				bounds.bottom / 2.0 ) );
	} else {
		StrokeLine( BPoint( bounds.right / 2.0, 0 ), BPoint(
				bounds.right / 2.0, bounds.bottom ) );
	}
}