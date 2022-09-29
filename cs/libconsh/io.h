void
lctab(int num, std::_t_ostream *out);		// 04/20/99 AM.
void
rule_pret(
	CON *atom,			/* Atomic concept for phrase.	*/
	//FILE *out			// 04/20/99 AM.
	std::_t_ostream *out		// 04/20/99 AM.
	);


void
phrase_pret(
	CON *phr,
	//FILE *out		// 04/20/99 AM.
	std::_t_ostream *out,	// 04/20/99 AM.
	CG *cg
	);
//LIBKBM_API
void
con_pret(
	CON *con,
	//FILE *out		// 04/20/99 AM.
	std::_t_ostream *out,	// 04/20/99 AM.
	CG *cg
	);

//LIBKBM_API
extern void
rule_pret(
	CON *atom,			/* Atomic concept for phrase.	*/
	//FILE *out			// 04/20/99 AM.
	std::_t_ostream *out		// 04/20/99 AM.
	);

bool f_exists(_TCHAR *fil);
