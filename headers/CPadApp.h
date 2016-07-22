// ------------------------------------------------------------------------
// CPadApp.h						(c)1997 Jesse Hall. All rights reserved.
// ------------------------------------------------------------------------

#pragma once

#include "CPad.h"
#include <Application.h>

class CPadApp : public BApplication {
public:
	// ====================================================================
	//	CONSTRUCTORS AND DESTRUCTORS
	// ====================================================================
			CPadApp		( void );
	virtual	~CPadApp	( void );
	
	// ====================================================================
	//	INHERITED OVERRIDES
	// ====================================================================
	virtual	void	AboutRequested	( void );
	virtual void	MessageReceived	( BMessage *msg );
	virtual	void	ReadyToRun		( void );
	virtual	void	RefsReceived	( BMessage *msg );

private:
	CPad *mPad;
	bool mIgnorePadClosed;
};