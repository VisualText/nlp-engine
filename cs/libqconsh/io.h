void
lctab(int num, _t_ostream *out);		// 04/20/99 AM.
void
rule_pret(
	CON_ID atom,			/* Atomic concept for phrase.	*/
	//FILE *out			// 04/20/99 AM.
	_t_ostream *out		// 04/20/99 AM.
	);


void
phrase_pret(
	CON_ID phr,
	//FILE *out		// 04/20/99 AM.
	_t_ostream *out,	// 04/20/99 AM.
	CG *cg
	);
//LIBKBM_API
void
con_pret(
	CON_ID con,
	//FILE *out		// 04/20/99 AM.
	_t_ostream *out,	// 04/20/99 AM.
	CG *cg
	);

//LIBKBM_API
extern void
rule_pret(
	CON_ID atom,			/* Atomic concept for phrase.	*/
	//FILE *out			// 04/20/99 AM.
	_t_ostream *out		// 04/20/99 AM.
	);

bool f_exists(_TCHAR *fil);
