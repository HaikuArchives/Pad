// ------------------------------------------------------------------------
// CText.cpp						(c)1997 Jesse Hall. All rights reserved.
// ------------------------------------------------------------------------

#include "CText.h"

// ------------------------------------------------------------------ CText
CText::CText( BRect frame, const char *name, uint32 resizingMode,
		uint32 flags )
	: BTextView( frame, name, BRect( 1, 1, frame.right - frame.left - 1,
			frame.bottom - frame.top - 1 ), resizingMode, flags ) {
}

// ----------------------------------------------------------------- ~CText
CText::~CText( void ) {
}

// ----------------------------------------------------------- FrameResized
void CText::FrameResized( float width, float height ) {
//	SetTextRect( BRect( 1, 1, width - 1, height - 1 ) );
}
