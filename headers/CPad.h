// ------------------------------------------------------------------------
// CPad.h							(c)1997 Jesse Hall. All rights reserved.
// ------------------------------------------------------------------------

#pragma once

#include "CText.h"
//#include "TList.h"
#include <Entry.h>
#include <MenuBar.h>
#include <ScrollBar.h>
#include <Window.h>
#include <ObjectList.h>
#include <StringView.h>

class CPad : public BWindow {
public:
	// ====================================================================
	//	CONSTRUCTORS AND DESTRUCTORS
	// ====================================================================
			CPad	( BEntry *entry = NULL );
	virtual	~CPad	( void );
	
	// ====================================================================
	// 	INHERITED OVERRIDES
	// ====================================================================
	virtual void	MessageReceived( BMessage *msg );
	virtual	bool	QuitRequested	( void );
	virtual	void	SaveRequested	( void );
	
	// ====================================================================
	//	OPERATIONS
	// ====================================================================
	virtual	void	NextPage	( void );
	virtual	void	PrevPage	( void );
	virtual	void	NewPage		( void );
	virtual	void	DelPage		( void );
	virtual	void	GoToPage	( uint16 num );
	
protected:
	// ====================================================================
	//	UTILITIES
	// ====================================================================
	virtual	BEntry	*GetDefaultPad	( bool /*create*/ = true );
			bool	ReadPad100		( BFile &file );
	virtual	void	NewPad			( void );
	virtual	void	SetPageNum		( void );
	virtual void	SetupWindow		( void );

	// ====================================================================
	//	DATA MEMBERS
	// ====================================================================
	BEntry				*mEntry;	// The entry representing the pad file
	BScrollBar			*mScroll;	// The vertical scroll bar at side
	BStringView			*mFooter;	// Footer (contains page number)
	BMenuBar			*mMenu;		// Menu bar
	BObjectList<CText>		mPages;		// The list of pages
	uint16				mCurPage;	// Number of the current page
};
