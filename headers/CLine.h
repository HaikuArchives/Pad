// ------------------------------------------------------------------------
// CLine.h							(c)1997 Jesse Hall. All rights reserved.
// ------------------------------------------------------------------------

#pragma once

#include <View.h>

class CLine : public BView {
public:
	enum { horiz, vert };

	// ====================================================================
	//	CONSTRUCTORS AND DESTRUCTORS
	// ====================================================================
			CLine( BRect frame, const char *name, uint32 resizingMode,
					uint32 flags, uint8 dir );
	virtual	~CLine( void );
	
	// ====================================================================
	// 	INHERITED OVERRIDES
	// ====================================================================
	virtual	void	Draw( BRect );

protected:
	uint8 mDir;
};