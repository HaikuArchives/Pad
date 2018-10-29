// ------------------------------------------------------------------------
// CText.h							(c)1997 Jesse Hall. All rights reserved.
// ------------------------------------------------------------------------

#pragma once

#include <TextView.h>

class CText : public BTextView {
public:

	// ====================================================================
	//	CONSTRUCTORS AND DESTRUCTORS
	// ====================================================================
			CText( BRect frame, const char *name, uint32 resizingMode,
					uint32 flags );
	virtual	~CText( void );
	
	// ====================================================================
	// 	INHERITED OVERRIDES
	// ====================================================================
	virtual	void FrameResized( float width, float height );
};
