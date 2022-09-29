/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	EANA.H
* FILE:	lite\eana.h
* CR:		10/12/99 AM.
* SUBJ:	Executable analyzer object.
* NOTE:	For now, will hold flags and major analyzer configurations.
*
*******************************************************************************/

#ifndef EANA_
#define EANA_
//#include <iostream.h>
#include <assert.h>
#include "lite/lite.h"

// Convenient organization of common analyzer execution configurations.
enum Eanaconf
	{
	conf_ZILCH,			// All flags false.
	conf_RUG,			// Minimal flags on, for automated rule generation.
	conf_RFA,			// Minimal flags on, for RFA read of RFB.	// 12/20/01 AM.
	conf_INTERN,		// For RFA or RFB to intern the analyzer.	// 12/20/01 AM.
	conf_REG,			// Regular configuration (only final log file).
	conf_DEV,			// Developer's configuration (all log files).
	conf_DEBUG,			// Full debugging configuration.
	conf_ALL				// All flags true.
	};

/********************************************
* CLASS:	EANA
* CR:		10/12/99 AM.
* SUBJ:	Executable analyzer configuration.
* NOTE:	"f" is for "flag".
********************************************/

class LITE_API Eana
{
public:
	Eana(enum Eanaconf = conf_REG);		// Default constructor.
	~Eana();

	// Access functions
	bool	getFlogfiles();
	bool	getFsilent();														// 06/16/02 AM.
	bool	getFlogfinal();
	bool	getFoutfiles();
	bool	getFdumpfiles();
	bool	getFcompile();														// 05/10/00 AM.

	bool	getFtimesum();
	bool	getFtimepass();

	bool	getFverbose();
	bool	getFdebug();
	bool	getFerrout();
	bool	getFreadrfb();														// 12/20/01 AM.
	bool	getFintern();														// 05/20/01 AM.

	// Modify functions
	void	setFlogfiles(bool);
	void	setFsilent(bool);													// 06/16/02 AM.
	void	setFlogfinal(bool);
	void	setFoutfiles(bool);
	void	setFdumpfiles(bool);
	void	setFcompile(bool);												// 05/10/00 AM.

	void	setFtimesum(bool);
	void	setFtimepass(bool);

	void	setFverbose(bool);
	void	setFdebug(bool);
	void	setFerrout(bool);
	void	setFreadrfb(bool);												// 12/20/01 AM.
	void	setFintern(bool);													// 05/20/01 AM.

	// General functions
	void configure(enum Eanaconf);


	// Class functions

private:
	bool flogfiles_;				// If creating intermediate log files.
	bool fsilent_;					// If not debugging/logging.		// 06/16/02 AM.
	bool flogfinal_;				// If creating log file for final tree.
	bool foutfiles_;				// If creating main output files.
	bool fdumpfiles_;				// If creating data dump files.
	bool fcompile_;				// If compiling the analyzer.		// 05/10/00 AM.
										// Used only by RFB.					// 05/10/00 AM.

	bool ftimesum_;				// If reporting overall timing.
	bool ftimepass_;				// If reporting time per pass.

	bool fverbose_;				// If verbose output.
	bool fdebug_;					// If debug output.
	bool ferrout_;					// If error output.

	bool freadrfb_;				// If interning the RFB.			// 12/20/01 AM.
	bool fintern_;					// If we're IN RFA,RFB, intern ana.// 05/20/01 AM.

#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;					// Count objects currently allocated.
#endif
};


#endif
