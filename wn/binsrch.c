/*

  binsearch.c - general binary search functions

*/

#include <stdio.h>
#include <string.h>
#include <tchar.h>

/* Binary search - looks for the key passed at the start of a line
   in the file associated with open file descriptor fp, and returns
   a buffer containing the line in the file. */

#define KEY_LEN		(1024)
#ifdef UNIX
#define LINE_LEN	(1024*25)
#endif
#ifdef MAC
#define LINE_LEN	(1024*8)
#endif
#ifdef PC
#define LINE_LEN	(1024*25)
#endif

static _TCHAR line[LINE_LEN]; 
long last_bin_search_offset = 0;

/* General purpose binary search function to search for key as first
   item on line in open file.  Item is delimited by space. */

#undef getc

_TCHAR *read_index(long offset, FILE *fp) {
    _TCHAR *linep;

    linep = line;
    line[0] = '0';

    fseek( fp, offset, SEEK_SET );
    _fgetts(linep, LINE_LEN, fp);
    return(line);
}

_TCHAR *bin_search(_TCHAR *searchkey, FILE *fp)
{
    int c;
    long top, mid, bot, diff;
    _TCHAR *linep, key[KEY_LEN];
    int length;

    diff=666;
    linep = line;
    line[0] = '\0';

    fseek(fp, 0L, 2);
    top = 0;
    bot = ftell(fp);
    mid = (bot - top) / 2;

    do {
	fseek(fp, mid - 1, 0);
	if(mid != 1)
	    while((c = _gettc(fp)) != '\n' && c != _TEOF);
        last_bin_search_offset = ftell( fp );
	_fgetts(linep, LINE_LEN, fp);
	length = (int)(_tcschr(linep, ' ') - linep);
	_tcsnccpy(key, linep, length);
	key[length] = '\0';
	if(_tcscmp(key, searchkey) < 0) {
	    top = mid;
	    diff = (bot - top) / 2;
	    mid = top + diff;
	}
	if(_tcscmp(key, searchkey) > 0) {
	    bot = mid;
	    diff = (bot - top) / 2;
	    mid = top + diff;
	}
    } while((_tcscmp(key, searchkey)) && (diff != 0));
    
    if(!_tcscmp(key, searchkey))
	return(line);
    else
	return(NULL);
}

static long offset;

static int bin_search_key(_TCHAR *searchkey, FILE *fp)
{
    int c;
    long top, mid, bot, diff;
    _TCHAR *linep, key[KEY_LEN];
    int length, offset1, offset2;

    /* do binary search to find correct place in file to insert line */

    diff=666;
    linep = line;
    line[0] = '\0';

    fseek(fp, 0L, 2);
    top = 0;
    bot = ftell(fp);
    if (bot == 0) {
	offset = 0;
	return(0);		/* empty file */
    }
    mid = (bot - top) / 2;

    /* If only one line in file, don't work through loop */

    length = 0;
    rewind(fp);
    while((c = _gettc(fp)) != '\n' && c != _TEOF)
	line[length++] =  c;
    if (_gettc(fp) == _TEOF) {	/* only 1 line in file */
	length = (int)(_tcschr(linep, ' ') - linep);
	_tcsnccpy(key, linep, length);
	key[length] = '\0';
	if(_tcscmp(key, searchkey) > 0) {
	    offset = 0;
	    return(0);		/* line with key is not found */
	} else if (_tcscmp(key, searchkey) < 0) {
	    offset = ftell(fp);
	    return(0);		/* line with key is not found */
	} else {
	    offset = 0;
	    return(1);		/* line with key is found */
	}
    }

    do {
	fseek(fp, mid - 1, 0);
	if(mid != 1)
	    while((c = _gettc(fp)) != '\n' && c != _TEOF);
	offset1 = ftell(fp);	/* offset at start of line */
	if (_fgetts(linep, LINE_LEN, fp) != NULL) {
  	    offset2 = ftell(fp); /* offset at start of next line */
	    length = (int)(_tcschr(linep, ' ') - linep);
	    _tcsnccpy(key, linep, length);
	    key[length] = '\0';
	    if(_tcscmp(key, searchkey) < 0) {	/* further in file */
		top = mid;
		diff = (bot - top) / 2;
		mid = top + diff;
		offset = offset2;
	    }
	    if(_tcscmp(key, searchkey) > 0) {	/* earlier in file */
		bot = mid;
		diff = (bot - top) / 2;
		mid = top + diff;
		offset = offset1;
	    }
	} else {
	    bot = mid;
	    diff = (bot - top) / 2;
	    mid = top + diff;
	}
    } while((_tcscmp(key, searchkey)) && (diff != 0));

    if(!_tcscmp(key, searchkey)) {
	offset = offset1;	/* get to start of current line */
	return(1);		/* line with key is found */
    } else
	return(0);		/* line with key is not found */
}

/* Copy contents from one file to another. */

void copyfile(FILE *fromfp, FILE *tofp)
{
    int c;

    while ((c = _gettc(fromfp)) != _TEOF)
	_puttc(c, tofp);
}

/* Function to replace a line in a file.  Returns the original line,
   or NULL in case of error. */

_TCHAR *replace_line(_TCHAR *new_line, _TCHAR *searchkey, FILE *fp)
{
    FILE *tfp;			/* temporary file pointer */

    if (!bin_search_key(searchkey, fp))
	return(NULL);		/* line with key not found */

    if ((tfp = tmpfile()) == NULL)
	return(NULL);		/* could not create temp file */
    fseek(fp, offset, 0);
    _fgetts(line, LINE_LEN, fp);	/* read original */
    copyfile(fp, tfp);
    if (fseek(fp, offset, 0) == -1)
	return(NULL);		/* could not seek to offset */
    _ftprintf(fp, new_line);	/* write line */
    rewind(tfp);
    copyfile(tfp, fp);

    fclose(tfp);
    fflush(fp);

    return(line);
}

/* Find location to insert line at in file.  If line with this
   key is already in file, return NULL. */

_TCHAR *insert_line(_TCHAR *new_line, _TCHAR *searchkey, FILE *fp)
{
    FILE *tfp;

    if (bin_search_key(searchkey, fp))
	return(NULL);
    
    if ((tfp = tmpfile()) == NULL)
	return(NULL);		/* could not create temp file */
    if (fseek(fp, offset, 0) == -1)
	return(NULL);		/* could not seek to offset */
    copyfile(fp, tfp);
    if (fseek(fp, offset, 0) == -1)
	return(NULL);		/* could not seek to offset */
    _ftprintf(fp, new_line);	/* write line */
    rewind(tfp);
    copyfile(tfp, fp);

    fclose(tfp);
    fflush(fp);

    return(new_line);
}
