/*
* Copyright (c) 2002-2006 Samit Basu
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*/

#include "Operators.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include "MemPtr.hpp"
#if HAVE_PCRE
#ifdef WIN32
#define PCRE_STATIC
#endif
#include <pcre.h>
#endif
#include <QtCore>
#include <QChar>
#include <QString>
#include "Printf.hpp"
#include "Algorithms.hpp"
#include "Utils.hpp"
#include "IEEEFP.hpp"

static int convspec(char c) {
    return ((c == 'd') || (c == 'i') || (c == 'o') || 
	(c == 'u') || (c == 'x') || (c == 'X') ||
	(c == 'e') || (c == 'E') || (c == 'f') || 
	(c == 'F') || (c == 'g') || (c == 'G') ||
	(c == 'a') || (c == 'A') || (c == 'c') || 
	(c == 's'));
}


//@@Signature
//function strcmp StrCmpFunction
//inputs string1 string2
//outputs flag
//DOCBLOCK string_strcmp
struct OpStrCmp {
    static inline bool func(const ArrayVector& arg) {
	Array x(arg[0]);
	Array y(arg[1]);
	if (!x.isString() || !y.isString()) return false;
	if (x.dimensions() != y.dimensions()) return false;
	return (x.asString() == y.asString());
    }
};

ArrayVector StrCmpFunction(int nargout, const ArrayVector& arg) {
    return ArrayVector(StringOp<bool,OpStrCmp>(arg));
}

//@@Signature
//function strcmpi StrCmpiFunction
//inputs string1 string2
//outputs flag
//DOCBLOCK string_strcmpi
struct OpStrCmpi {
    static inline bool func(const ArrayVector& arg) {
	Array x(arg[0]);
	Array y(arg[1]);
	if (!x.isString() || !y.isString()) return false;
	if (x.dimensions() != y.dimensions()) return false;
	return (x.asString().toUpper() == y.asString().toUpper());    
    }
};
ArrayVector StrCmpiFunction(int nargout, const ArrayVector& arg) {
    return ArrayVector(StringOp<bool,OpStrCmpi>(arg));
}

//@@Signature
//function strncmp StrnCmpFunction
//inputs string1 string2 len
//outputs flag
//DOCBLOCK string_strncmp
struct OpStrnCmp {
    static inline bool func(const ArrayVector& arg) {
	Array x(arg[0]);
	Array y(arg[1]);
	Array n(arg[2]);
	int m = n.asInteger();
	if (!x.isString() || !y.isString()) return false;
	if ((x.length() < m) || (y.length() < m)) return false;
	return (x.asString().left(m) == y.asString().left(m));
    }
};

ArrayVector StrnCmpFunction(int nargout, const ArrayVector& arg) {
    return ArrayVector(StringOp<bool,OpStrnCmp>(arg));
}
//@@Signature
//function strstr StrStrFunction
//inputs x y
//outputs flag
//DOCBLOCK string_strstr
ArrayVector StrStrFunction(int nargout, const ArrayVector& arg) {
    Array retval, arg1, arg2;
    if (arg.size() != 2)
	throw Exception("strstr function requires two string arguments");
    if (!(arg[0].isString()))
	throw Exception("strstr function requires two string arguments");
    if (!(arg[1].isString()))
	throw Exception("strstr function requires two string arguments");
    return ArrayVector(Array(double(arg[0].asString().indexOf(arg[1].asString())+1)));
}


//@@Signature
//function strrep StrRepFunction
//inputs source pattern replace
//outputs y
//DOCBLOCK string_strrep
static Array StrRepFunc(const Array &source, const Array &pattern, const Array &replace) {
    if (!source.isString() || !pattern.isString() || !replace.isString()) 
	return source;
    return Array(source.asString().replace(pattern.asString(),replace.asString()));
}

ArrayVector StrRepFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 3)
	throw Exception("strrep function requires arguments");
    Array source(arg[0]);
    Array pattern(arg[1]);
    Array replace(arg[2]);
    // Force everything to be a cell array
    if (source.dataClass() != CellArray) source = CellArrayFromArray(source);
    if (pattern.dataClass() != CellArray) pattern = CellArrayFromArray(pattern);
    if (replace.dataClass() != CellArray) replace = CellArrayFromArray(replace);
    NTuple dims;
    if (source.isScalar() && pattern.isScalar()) {
	dims = replace.dimensions();
    } else if (source.isScalar() && replace.isScalar()) {
	dims = pattern.dimensions();
    } else if (pattern.isScalar() && replace.isScalar()) {
	dims = source.dimensions();
    } else if (source.isScalar()) {
	if (pattern.dimensions() != replace.dimensions())
	    throw Exception("All cell-array arguments must be the same size (or scalars)");
	dims = pattern.dimensions();
    } else if (pattern.isScalar()) {
	if (source.dimensions() != replace.dimensions()) 
	    throw Exception("All cell-array arguments must be the same size (or scalars)");
	dims = source.dimensions();
    } else if (replace.isScalar()) {
	if (source.dimensions() != pattern.dimensions())
	    throw Exception("All cell-array arguments must be the same size (or scalars)");
	dims = source.dimensions();
    } else {
	if ((source.dimensions() != pattern.dimensions()) || 
	    (source.dimensions() != replace.dimensions()))
	    throw Exception("All cell-array arguments must be the same size (or scalars)");
	dims = source.dimensions();
    }
    int source_incr = (source.isScalar() ? 0 : 1);
    int pattern_incr = (pattern.isScalar() ? 0 : 1);
    int replace_incr = (replace.isScalar() ? 0 : 1);
    const BasicArray<Array> &sp(source.constReal<Array>());
    const BasicArray<Array> &pp(pattern.constReal<Array>());
    const BasicArray<Array> &rp(replace.constReal<Array>());
    BasicArray<Array> retvec(dims);
    for (index_t i=1;i<=dims.count();i++) 
	retvec.set(i,StrRepFunc(sp[1+(i-1)*source_incr],pp[1+(i-1)*pattern_incr],rp[1+(i-1)*replace_incr]));
    if (retvec.isScalar())
	return ArrayVector(ArrayFromCellArray(retvec));
    else
	return ArrayVector(retvec);
}

//@@Signature
//function regexp RegExpFunction
//inputs string expr varargin
//outputs start stop tokenExtents match tokens names
//@@Signature
//function regexpi RegExpIFunction
//inputs string expr varargin
//outputs start stop tokenExtents match tokens names
//@@Signature
//function regexprepdriver RegExpRepDriverFunction
//inputs varargin
//outputs y
//DOCBLOCK string_regexp
#if HAVE_PCRE
static bool isSlotSpec(QString t) {
    return ((t == "start") ||
	(t == "end") ||
	(t == "tokenExtents") ||
	(t == "match") ||
	(t == "tokens") ||
	(t == "names"));
}
#endif

static ArrayVector RegExpCoreFunction(StringVector stringed_args, bool defaultMatchCase) {
#if HAVE_PCRE
    // These are the default options
    bool globalMatch = true;
    bool matchCase = defaultMatchCase;
    bool greedyDot = true;
    bool stringAnchors = true;
    bool literalSpacing = true;

    int start_slot = 0;
    int end_slot = 1;
    int tokenExtents_slot = 2;
    int match_slot = 3;
    int tokens_slot = 4;
    int names_slot = 5;
    int count_slots = 6;

    // This is a cut-paste-hack of the pcredemo program that comes with pcre
    QList<uint32> startList;
    QList<uint32> stopList;
    QList<uint32> tokenStartList;
    QList<uint32> tokenStopList;
    QList<QList<uint32> > tokenExtentsList;
    QList<StringVector> tokenList;
    StringVector matchList;
    StringVector namedTokenNames;
    QList<StringVector> namedTokenValues;

    pcre *re;
    const char *error;
    const char *pattern;
    const char *subject;
    unsigned char *name_table;
    int erroffset;
    int namecount;
    int name_entry_size;
    int subject_length;
    int rc, i;
    ArrayVector retvec;

    if (stringed_args.size() < 2)
	throw Exception("regexpcore requires at least two arguments - both strings, the data string and pattern string");

    // Check for output specifiers
    bool slot_specs_present = false;
    for (int i3=2;i3<stringed_args.size();i3++) 
	if (isSlotSpec(stringed_args[i3])) {
	    slot_specs_present = true;
	    break;
	}
	if (slot_specs_present) {
	    // output specifiers are active... eliminate all outputs by default
	    start_slot = end_slot = tokenExtents_slot = match_slot = tokens_slot = names_slot = -1;
	    count_slots = 0;
	    // Process inputs
	    for (int i2=2;i2<stringed_args.size();i2++) {
		QString t = stringed_args[i2];
		if (t == "start")  start_slot = count_slots++;
		if (t == "end") end_slot = count_slots++;
		if (t == "tokenExtents") tokenExtents_slot = count_slots++;
		if (t == "match") match_slot = count_slots++;
		if (t == "tokens") tokens_slot = count_slots++;
		if (t == "names") names_slot = count_slots++;
	    }
	}

	for (int i1=0;i1<count_slots;i1++)
	    retvec << EmptyConstructor();

	for (int j=2;j<stringed_args.size();j++) {
	    if (stringed_args[j]=="once")
		globalMatch = false;
	    else if (stringed_args[j]=="matchcase")
		matchCase = true;
	    else if (stringed_args[j]=="ignorecase")
		matchCase = false;
	    else if (stringed_args[j]=="dotall")
		greedyDot = true;
	    else if (stringed_args[j]=="dotexceptnewline")
		greedyDot = false;
	    else if (stringed_args[j]=="stringanchors")
		stringAnchors = true;
	    else if (stringed_args[j]=="lineanchors")
		stringAnchors = false;
	    else if (stringed_args[j]=="literalspacing")
		literalSpacing = true;
	    else if (stringed_args[j]=="freespacing")
		literalSpacing = false;
	}

	QByteArray pattern_ba(stringed_args[1].toAscii());
	QByteArray subject_ba(stringed_args[0].toAscii());
	pattern = pattern_ba.constData();
	subject = subject_ba.constData();
	subject_length = (int)strlen(subject);
	QString qsubject(subject);

	int options = 0;

	if (!matchCase) options |= PCRE_CASELESS;
	if (greedyDot) options |= PCRE_DOTALL;
	if (!stringAnchors) options |= PCRE_MULTILINE;
	if (!literalSpacing) options |= PCRE_EXTENDED;

	/*************************************************************************
	* Now we are going to compile the regular expression pattern, and handle *
	* and errors that are detected.                                          *
	*************************************************************************/

	re = pcre_compile(
	    pattern,              /* the pattern */
	    options,              /* default options */
	    &error,               /* for error message */
	    &erroffset,           /* for error offset */
	    NULL);                /* use default character tables */

	/* Compilation failed: print the error message and exit */

	if (re == NULL) 
	    throw Exception(QString("regular expression compilation failed at offset ") + 
	    QString("%1").arg(erroffset) + ": " + error);

	/* Determine how many capture expressions there are */
	int captureCount;
	rc = pcre_fullinfo(re,NULL,PCRE_INFO_CAPTURECOUNT,&captureCount);

	/* Allocate the output vector */
	int ovectorSize = (captureCount+1)*3;
	MemBlock<int> ovectorData(ovectorSize);
	int *ovector(&ovectorData);

	/*************************************************************************
	* If the compilation succeeded, we call PCRE again, in order to do a     *
	* pattern match against the subject string. This does just ONE match. If *
	* further matching is needed, it will be done below.                     *
	*************************************************************************/

	rc = pcre_exec(
	    re,                   /* the compiled pattern */
	    NULL,                 /* no extra data - we didn't study the pattern */
	    subject,              /* the subject string */
	    subject_length,       /* the length of the subject */
	    0,                    /* start at offset 0 in the subject */
	    0,                    /* default options */
	    ovector,              /* output vector for substring information */
	    ovectorSize);         /* number of elements in the output vector */

	/* Matching failed: handle error cases */

	if (rc < 0) {
	    pcre_free(re);     /* Release memory used for the compiled pattern */
	    return retvec;
	}

	/* Match succeded */

	startList << ovector[0]+1;
	stopList << ovector[1];
	matchList << qsubject.mid(ovector[0],ovector[1]-ovector[0]);

	/*************************************************************************
	* We have found the first match within the subject string. If the output *
	* vector wasn't big enough, set its size to the maximum. Then output any *
	* substrings that were captured.                                         *
	*************************************************************************/

	/* The output vector wasn't big enough */

	if (rc == 0) 
	    throw Exception("internal error - more outputs than expected?");

	/* Show substrings stored in the output vector by number. Obviously, in a real
	application you might want to do things other than print them. */

	QList<uint32> tEList;
	StringVector   tList;
	for (i = 1; i < rc; i++)
	{
	    tEList << ovector[2*i]+1;
	    tEList << ovector[2*i+1];
	    tList << qsubject.mid(ovector[2*i],ovector[2*i+1]-ovector[2*i]);
	}

	tokenExtentsList << tEList;
	tokenList << tList;


	/* See if there are any named substrings, and if so, show them by name. First
	we have to extract the count of named parentheses from the pattern. */

	(void)pcre_fullinfo(
	    re,                   /* the compiled pattern */
	    NULL,                 /* no extra data - we didn't study the pattern */
	    PCRE_INFO_NAMECOUNT,  /* number of named substrings */
	    &namecount);          /* where to put the answer */


	if (namecount > 0)  {
	    unsigned char *tabptr;

	    /* Before we can access the substrings, we must extract the table for
	    translating names to numbers, and the size of each entry in the table. */

	    (void)pcre_fullinfo(
		re,                       /* the compiled pattern */
		NULL,                     /* no extra data - we didn't study the pattern */
		PCRE_INFO_NAMETABLE,      /* address of the table */
		&name_table);             /* where to put the answer */

	    (void)pcre_fullinfo(
		re,                       /* the compiled pattern */
		NULL,                     /* no extra data - we didn't study the pattern */
		PCRE_INFO_NAMEENTRYSIZE,  /* size of each entry in the table */
		&name_entry_size);        /* where to put the answer */

	    /* Now we can scan the table and, for each entry, print the number, the name,
	    and the substring itself. */

	    tabptr = name_table;
	    for (i = 0; i < namecount; i++)
	    {
		namedTokenValues << StringVector();
		int n = (tabptr[0] << 8) | tabptr[1];
		namedTokenNames << QString((char*)(tabptr+2)).left(name_entry_size-3);
		namedTokenValues[i] << qsubject.mid(ovector[2*n],ovector[2*n+1]-ovector[2*n]);
		tabptr += name_entry_size;
	    }
	}


	/*************************************************************************
	* If the "-g" option was given on the command line, we want to continue  *
	* to search for additional matches in the subject string, in a similar   *
	* way to the /g option in Perl. This turns out to be trickier than you   *
	* might think because of the possibility of matching an empty string.    *
	* What happens is as follows:                                            *
	*                                                                        *
	* If the previous match was NOT for an empty string, we can just start   *
	* the next match at the end of the previous one.                         *
	*                                                                        *
	* If the previous match WAS for an empty string, we can't do that, as it *
	* would lead to an infinite loop. Instead, a special call of pcre_exec() *
	* is made with the PCRE_NOTEMPTY and PCRE_ANCHORED flags set. The first  *
	* of these tells PCRE that an empty string is not a valid match; other   *
	* possibilities must be tried. The second flag restricts PCRE to one     *
	* match attempt at the initial string position. If this match succeeds,  *
	* an alternative to the empty string match has been found, and we can    *
	* proceed round the loop.                                                *
	*************************************************************************/


	/* Loop for second and subsequent matches */

	if (globalMatch && (ovector[1] < subject_length)) {
	    for (;;)
	    {
		int options = 0;                 /* Normally no options */
		int start_offset = ovector[1];   /* Start at end of previous match */

		/* If the previous match was for an empty string, we are finished if we are
		at the end of the subject. Otherwise, arrange to run another match at the
		same point to see if a non-empty match can be found. */

		if (ovector[0] == ovector[1])
		{
		    if (ovector[0] == subject_length) break;
		    options = PCRE_NOTEMPTY | PCRE_ANCHORED;
		}

		/* Run the next matching operation */

		rc = pcre_exec(
		    re,                   /* the compiled pattern */
		    NULL,                 /* no extra data - we didn't study the pattern */
		    subject,              /* the subject string */
		    subject_length,       /* the length of the subject */
		    start_offset,         /* starting offset in the subject */
		    options,              /* options */
		    ovector,              /* output vector for substring information */
		    ovectorSize);           /* number of elements in the output vector */

		/* This time, a result of NOMATCH isn't an error. If the value in "options"
		is zero, it just means we have found all possible matches, so the loop ends.
		Otherwise, it means we have failed to find a non-empty-string match at a
		point where there was a previous empty-string match. In this case, we do what
		Perl does: advance the matching position by one, and continue. We do this by
		setting the "end of previous match" offset, because that is picked up at the
		top of the loop as the point at which to start again. */

		if (rc == PCRE_ERROR_NOMATCH)
		{
		    if (options == 0) break;
		    ovector[1] = start_offset + 1;
		    continue;    /* Go round the loop again */
		}

		/* Other matching errors are not recoverable. */

		if (rc < 0)
		{
		    break;
		}

		/* Match succeded */

		/* The match succeeded, but the output vector wasn't big enough. */

		if (rc == 0)
		    throw Exception("internal error - more outputs than expected?");

		startList << ovector[0]+1;
		stopList << ovector[1];
		matchList << qsubject.mid(ovector[0],ovector[1]-ovector[0]);

		QList<uint32> tEList;
		StringVector   tList;
		for (i = 1; i < rc; i++)
		{
		    tEList << ovector[2*i]+1;
		    tEList << ovector[2*i+1];
		    tList << qsubject.mid(ovector[2*i],ovector[2*i+1]-ovector[2*i]);
		}

		tokenExtentsList << tEList;
		tokenList << tList;

		if (namecount > 0) {
		    unsigned char *tabptr = name_table;
		    tabptr = name_table;
		    for (i = 0; i < namecount; i++)
		    {
			int n = (tabptr[0] << 8) | tabptr[1];
			namedTokenValues[i] << qsubject.mid(ovector[2*n],ovector[2*n+1]-ovector[2*n]);
			tabptr += name_entry_size;
		    }
		}
	    }      /* End of loop to find second and subsequent matches */
	}

	// Return this data to the user
	Array start(DoubleVectorFromQList(startList));
	Array end(DoubleVectorFromQList(stopList));
	Array matches(CellArrayFromStringVector(matchList));
	// Now build the tokens array
	ArrayVector tokensArrayContents;
	for (int i=0;i<tokenList.size();i++) 
	    tokensArrayContents << CellArrayFromStringVector(tokenList[i]);
	Array tokens(CellConstructor(tokensArrayContents));
	// Finally the token extents array
	ArrayVector tokensExtentsContents;
	for (int i=0;i<tokenExtentsList.size();i++)
	    tokensExtentsContents << DoubleVectorFromQList(tokenExtentsList[i]);
	Array tokenExtents(CellConstructor(tokensExtentsContents));
	// The named token data has to be resliced
	ArrayVector namedTokenValueContents;
	for (int i=0;i<namedTokenValues.size();i++)
	    namedTokenValueContents << CellArrayFromStringVector(namedTokenValues[i]);
	Array namedTokens(StructConstructor(namedTokenNames,namedTokenValueContents));
	// Stuff it all into a return vector
	pcre_free(re);
	if (start_slot >= 0)
	    retvec[start_slot] = start;
	if (end_slot >= 0)
	    retvec[end_slot] = end;
	if (tokenExtents_slot >= 0)
	    retvec[tokenExtents_slot] = tokenExtents;
	if (match_slot >= 0)
	    retvec[match_slot] = matches;
	if (tokens_slot >= 0)
	    retvec[tokens_slot] = tokens;
	if (names_slot >= 0)
	    retvec[names_slot] = namedTokens;
	return retvec;
#else
    throw Exception("regexp support not compiled in (requires PCRE library)");
#endif  

}

// res is organized like this:
//  <o1 o2 o3 o4>
//  <o1 o2 o3 o4>
// ...
// We want to perform a transpose
static ArrayVector CellifyRegexpResults(QList<ArrayVector> res, const NTuple &dims) {
    ArrayVector retVec;
    if (res.size() == 0) return ArrayVector();
    for (int i=0;i<res[0].size();i++) {
	ArrayVector slice;
	for (int j=0;j<res.size();j++) 
	    slice << res[j][i];
	int slice_len = slice.size();
	Array sliceArray(CellArrayFromArrayVector(slice,slice_len));
	sliceArray.reshape(dims);
	retVec << sliceArray;
    }
    return retVec;
}

static ArrayVector RegExpWrapperFunction(int nargout, const ArrayVector& arg, 
					 bool caseMatch) {
					     if (arg.size() < 2) throw Exception("regexp requires at least two arguments to function");
					     for (int i=2;i<arg.size();i++) 
						 if (!arg[i].isString())
						     throw Exception("all arguments to regexp must be strings");
					     if (arg[0].isString() && arg[1].isString()) {
						 // If both arguments are scalar strings...
						 StringVector stringed_args;
						 // Convert the argument array to strings
						 for (int i=0;i<arg.size();i++) 
						     stringed_args << arg[i].asString();
						 return RegExpCoreFunction(stringed_args,caseMatch);
					     } else if (IsCellStringArray(arg[0]) && arg[1].isString()) {
						 StringVector arg0(StringVectorFromArray(arg[0]));
						 QList<ArrayVector> results;
						 for (int j=0;j<arg0.size();j++) {
						     StringVector stringed_args;
						     stringed_args << arg0[j];
						     for (int i=1;i<arg.size();i++) 
							 stringed_args << arg[i].asString();
						     results << RegExpCoreFunction(stringed_args,caseMatch);
						 }
						 return CellifyRegexpResults(results,arg[0].dimensions());
					     } else if (arg[0].isString() && IsCellStringArray(arg[1])) {
						 StringVector arg1(StringVectorFromArray(arg[1]));
						 QList<ArrayVector> results;
						 for (int j=0;j<arg1.size();j++) {
						     StringVector stringed_args;
						     stringed_args << arg[0].asString();
						     stringed_args << arg1[j];
						     for (int i=2;i<arg.size();i++) 
							 stringed_args << arg[i].asString();
						     results << RegExpCoreFunction(stringed_args,caseMatch);
						 }
						 return CellifyRegexpResults(results,arg[1].dimensions());
					     } else if (IsCellStringArray(arg[0]) && IsCellStringArray(arg[1])) {
						 if (arg[0].length() != arg[1].length())
						     throw Exception("cell-arrays of strings as the first two arguments to regexp must be the same size");
						 StringVector dp(StringVectorFromArray(arg[0]));
						 StringVector sp(StringVectorFromArray(arg[1]));
						 QList<ArrayVector> results;
						 for (int j=0;j<arg[0].length();j++) {
						     StringVector stringed_args;
						     stringed_args << dp[j];
						     stringed_args << sp[j];
						     for (int i=2;i<arg.size();i++) 
							 stringed_args << arg[i].asString();
						     results << RegExpCoreFunction(stringed_args,caseMatch);
						 }
						 return CellifyRegexpResults(results,arg[0].dimensions());
					     } else {
						 throw Exception("first two arguments to regexp must be strings of cell-arrays of strings");
					     }
					     return ArrayVector();
}

ArrayVector RegExpFunction(int nargout, const ArrayVector&arg) {
    return RegExpWrapperFunction(nargout, arg, true);
}

ArrayVector RegExpIFunction(int nargout, const ArrayVector& arg) {
    return RegExpWrapperFunction(nargout, arg, false);
}

// Perform a replace 
QString RegExpRepCoreFunction(QString subject,
			      QString pattern,
			      StringVector modes,
			      StringVector replacements) {
#if HAVE_PCRE
				  // These are the default options
				  bool globalMatch = true;
				  bool matchCase = true;
				  bool greedyDot = true;
				  bool stringAnchors = true;
				  bool literalSpacing = true;

				  pcre *re;
				  const char *error;
				  int erroffset;
				  int rc;

				  for (int j=0;j<modes.size();j++) {
				      if (modes[j]=="once")
					  globalMatch = false;
				      else if (modes[j]=="matchcase")
					  matchCase = true;
				      else if (modes[j]=="ignorecase")
					  matchCase = false;
				      else if (modes[j]=="dotall")
					  greedyDot = true;
				      else if (modes[j]=="dotexceptnewline")
					  greedyDot = false;
				      else if (modes[j]=="stringanchors")
					  stringAnchors = true;
				      else if (modes[j]=="lineanchors")
					  stringAnchors = false;
				      else if (modes[j]=="literalspacing")
					  literalSpacing = true;
				      else if (modes[j]=="freespacing")
					  literalSpacing = false;
				  }

				  int options = 0;

				  if (!matchCase) options |= PCRE_CASELESS;
				  if (greedyDot) options |= PCRE_DOTALL;
				  if (!stringAnchors) options |= PCRE_MULTILINE;
				  if (!literalSpacing) options |= PCRE_EXTENDED;

				  /*************************************************************************
				  * Now we are going to compile the regular expression pattern, and handle *
				  * and errors that are detected.                                          *
				  *************************************************************************/

				  QByteArray pattern_ba(pattern.toAscii());
				  re = pcre_compile(
				      pattern_ba.constData(),      /* the pattern */
				      options,              /* default options */
				      &error,               /* for error message */
				      &erroffset,           /* for error offset */
				      NULL);                /* use default character tables */

				  /* Compilation failed: print the error message and exit */

				  if (re == NULL) 
				      throw Exception(QString("regular expression compilation failed at offset ") + 
				      erroffset + ": " + error);

				  /* Determine how many capture expressions there are */
				  int captureCount;
				  rc = pcre_fullinfo(re,NULL,PCRE_INFO_CAPTURECOUNT,&captureCount);

				  /* Allocate the output vector */
				  int ovectorSize = (captureCount+1)*3;
				  MemBlock<int> ovectorData(ovectorSize);
				  int *ovector(&ovectorData);

				  /*************************************************************************
				  * If the compilation succeeded, we call PCRE again, in order to do a     *
				  * pattern match against the subject string. This does just ONE match. If *
				  * further matching is needed, it will be done below.                     *
				  *************************************************************************/

				  QByteArray subject_ba(subject.toAscii());
				  rc = pcre_exec(
				      re,                   /* the compiled pattern */
				      NULL,                 /* no extra data - we didn't study the pattern */
				      subject_ba.constData(),      /* the subject string */
				      subject_ba.size(),       /* the length of the subject */
				      0,                    /* start at offset 0 in the subject */
				      0,                    /* default options */
				      ovector,              /* output vector for substring information */
				      ovectorSize);         /* number of elements in the output vector */

				  /* Matching failed: handle error cases */

				  if (rc < 0) {
				      pcre_free(re);     /* Release memory used for the compiled pattern */
				      return subject;
				  }

				  QString outputString;
				  QString tokenSelect;
				  int nextReplacement = 0;
				  int inputPointer = 0;
				  int outputPtr = 0;
				  int digitFinder = 0;
				  int replacementLength = replacements[nextReplacement].size();
				  int tokenNumber;
				  /* Match succeeded... start building up the output string */
				  while (inputPointer < ovector[0]) outputString += subject[inputPointer++];
				  /* Now insert the replacement string */
				  while (outputPtr < replacementLength) {
				      if (replacements[nextReplacement][outputPtr] != '$')
					  outputString += replacements[nextReplacement][outputPtr++];
				      else
					  if ((outputPtr < (replacementLength-1)) &&
					      replacements[nextReplacement][outputPtr+1].isDigit()) {
						  // Try to collect a token name
						  digitFinder = outputPtr+1;
						  while ((digitFinder < replacementLength) && 
						      replacements[nextReplacement][digitFinder].isDigit()) {
							  // Add this digit to the token name
							  tokenSelect += replacements[nextReplacement][digitFinder];
							  digitFinder++;
						  }
						  // try to map this to a token number
						  tokenNumber = tokenSelect.toInt();
						  // Is this a valid tokenNumber?
						  if (tokenNumber <= captureCount) {
						      // Yes - skip
						      outputPtr = digitFinder;
						      // Push the capturecount...
						      for (int p=ovector[2*tokenNumber];p<ovector[2*tokenNumber+1];p++)
							  outputString += subject[p];
						  } else {
						      // No - just push the '$' to the output, and bump outputPtr
						      outputString += '$';
						      outputPtr++;
						  }
					  }
				  }
				  /* advance the input pointer */
				  inputPointer = ovector[1];

				  if (replacements.size() > 1)
				      nextReplacement++;

				  if (globalMatch && (ovector[1] < (int)subject.size())) {
				      for (;;)
				      {
					  int options = 0;                 /* Normally no options */
					  int start_offset = ovector[1];   /* Start at end of previous match */

					  /* If the previous match was for an empty string, we are finished if we are
					  at the end of the subject. Otherwise, arrange to run another match at the
					  same point to see if a non-empty match can be found. */

					  if (ovector[0] == ovector[1])
					  {
					      if (ovector[0] == (int) subject.size()) break;
					      options = PCRE_NOTEMPTY | PCRE_ANCHORED;
					  }

					  /* Run the next matching operation */

					  QByteArray subject_ba(subject.toAscii());
					  rc = pcre_exec(
					      re,                   /* the compiled pattern */
					      NULL,                 /* no extra data - we didn't study the pattern */
					      subject_ba.constData(),      /* the subject string */
					      subject_ba.size(),       /* the length of the subject */
					      start_offset,         /* starting offset in the subject */
					      options,              /* options */
					      ovector,              /* output vector for substring information */
					      ovectorSize);           /* number of elements in the output vector */

					  /* This time, a result of NOMATCH isn't an error. If the value in "options"
					  is zero, it just means we have found all possible matches, so the loop ends.
					  Otherwise, it means we have failed to find a non-empty-string match at a
					  point where there was a previous empty-string match. In this case, we do what
					  Perl does: advance the matching position by one, and continue. We do this by
					  setting the "end of previous match" offset, because that is picked up at the
					  top of the loop as the point at which to start again. */

					  if (rc == PCRE_ERROR_NOMATCH)
					  {
					      if (options == 0) break;
					      ovector[1] = start_offset + 1;
					      continue;    /* Go round the loop again */
					  }

					  /* Other matching errors are not recoverable. */

					  if (rc < 0)
					  {
					      break;
					  }

					  /* Match succeded */

					  /* The match succeeded, but the output vector wasn't big enough. */

					  if (rc == 0)
					      throw Exception("internal error - more outputs than expected?");

					  /* output characters from the subject string until we reach the next match */
					  while (inputPointer < ovector[0]) outputString += subject[inputPointer++];
					  inputPointer = ovector[1];

					  /* output the replacement string */
					  replacementLength = replacements[nextReplacement].size();
					  outputPtr = 0;
					  tokenSelect = "";
					  /* Now insert the replacement string */
					  while (outputPtr < replacementLength) {
					      if (replacements[nextReplacement][outputPtr] != '$')
						  outputString += replacements[nextReplacement][outputPtr++];
					      else
						  if ((outputPtr < (replacementLength-1)) &&
						      replacements[nextReplacement][outputPtr+1].isDigit()) {
							  // Try to collect a token name
							  digitFinder = outputPtr+1;
							  while ((digitFinder < replacementLength) && 
							      replacements[nextReplacement][digitFinder].isDigit()) {
								  // Add this digit to the token name
								  tokenSelect += replacements[nextReplacement][digitFinder];
								  digitFinder++;
							  }
							  // try to map this to a token number
							  tokenNumber = tokenSelect.toInt();
							  // Is this a valid tokenNumber?
							  if (tokenNumber <= captureCount) {
							      // Yes - skip
							      outputPtr = digitFinder;
							      // Push the capturecount...
							      for (int p=ovector[2*tokenNumber];p<ovector[2*tokenNumber+1];p++)
								  outputString += subject[p];
							  } else {
							      // No - just push the '$' to the output, and bump outputPtr
							      outputString += '$';
							      outputPtr++;
							  }
						  }
					  }
					  if (replacements.size() > 1) {
					      nextReplacement++;
					      if (nextReplacement >= replacements.size())
						  break; // No more replacments to make
					  }
				      }      /* End of loop to find second and subsequent matches */
				  }

				  while (inputPointer < (int) subject.size()) 
				      outputString += subject[inputPointer++];

				  pcre_free(re);
				  return outputString;
#else
				  throw Exception("regexprep support not compiled in (requires PCRE library)");
#endif
}

ArrayVector RegExpRepDriverFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 3) throw Exception("regexprep requires at least three arguments to function");
    for (int i=3;i<arg.size();i++) 
	if (!arg[i].isString())
	    throw Exception("all arguments to regexprep must be strings");
    StringVector modes;
    for (int i=3;i<arg.size();i++)
	modes << arg[i].asString();
    QString subject = arg[0].asString();
    QString pattern = arg[1].asString();
    StringVector replist;
    if (arg[2].isString())
	replist << arg[2].asString();
    else if (IsCellStringArray(arg[2])) {
	StringVector dp(StringVectorFromArray(arg[2]));
	for (int i=0;i<dp.size();i++)
	    replist << dp[i];
    }
    return ArrayVector(Array(RegExpRepCoreFunction(subject,pattern,modes,replist)));
}				  



//@@Signature
//function deblank DeblankFunction
//inputs x
//outputs y
//DOCBLOCK string_deblank
struct OpDeblank {
    static inline Array func(const Array& arg) {
	QString txt(arg.asString());
	while ((txt.length() >= 1) && (txt.right(1).at(0).isSpace() ||
	    txt.right(1).at(0).isNull()))
	    txt.chop(1);
	Array z(txt);
	if (z.isEmpty() && z.is2D())
	    z = EmptyConstructor();
	return z;
    }
};

template <typename T>
static BasicArray<T> DeBlankDouble(const BasicArray<T> &rp, 
				   index_t trim_count) {
				       if (trim_count == 0) return rp;
				       index_t p_len = rp.dimensions()[1];
				       BasicArray<T> sp(rp.dimensions().replace(1,p_len-trim_count));
				       ConstBasicIterator<T> iter2(&rp,1);
				       BasicIterator<T> dest(&sp,1);
				       while (iter2.isValid()) {
					   for (index_t i=1;i<=(p_len-trim_count);i++) {
					       dest.set(iter2.get());
					       iter2.next();
					       dest.next();
					   }
					   iter2.nextSlice();
					   dest.nextSlice();
				       }
				       return sp;
}

template <typename T>
static index_t GetTrimCount(const BasicArray<T> &rp) {
    index_t p_len = rp.dimensions()[1];
    ConstBasicIterator<T> iter(&rp,1);
    index_t trim_count = p_len;
    while (iter.isValid()) {
	index_t trim_this_slice = 0;
	for (index_t i=1;i<p_len;i++)
	    iter.next();
	for (index_t i=iter.size();i>=1;i--) {
	    if ((iter.get() == 0) && (iter.size()-i+1 > trim_this_slice))
		trim_this_slice = iter.size() - i + 1;
	    else if (iter.get() != 0)	break;
	    iter.prev();
	}
	iter.nextSlice();
	trim_count = qMin(trim_count,trim_this_slice);
    }
    return trim_count;
}

template <typename T>
static Array DeblankDouble(Array arg) {
    arg.ensureNotScalarEncoded();
    index_t real_trim = GetTrimCount(arg.constReal<T>());
    Array z;
    if (arg.allReal())
	z = Array(DeBlankDouble(arg.constReal<T>(),real_trim));
    else {
	index_t imag_trim = GetTrimCount(arg.constImag<T>());
	real_trim = qMin(real_trim,imag_trim);
	z = Array(DeBlankDouble(arg.constReal<T>(),real_trim),
	    DeBlankDouble(arg.constImag<T>(),real_trim));
    }
    if (z.isEmpty() && z.is2D())
	z = EmptyConstructor();
    return z;
}

#define MacroDeblank(ctype,cls) \
  case cls: return DeblankDouble<ctype>(x);

static Array DeBlank(const Array &x) {
    if (x.isEmpty())
	return EmptyConstructor().toClass(StringArray);
    if (x.isString())
	return StringOp<OpDeblank>(x);
    else if (x.dataClass() == CellArray) {
	BasicArray<Array> qp(x.dimensions());
	const BasicArray<Array> &sp(x.constReal<Array>());
	for (index_t i=1;i<=sp.length();i++)
	    qp[i] = DeBlank(sp[i]);
	return Array(qp);
    } else {
	switch (x.dataClass()) {
  default:
      throw Exception("Unhandled case for deblank");
      MacroExpandCases(MacroDeblank);
	}
    }
}

ArrayVector DeblankFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() == 0)
	throw Exception("deblank requires at least one argument");
    if (arg[0].isEmpty())
	return ArrayVector(arg[0]);
    return ArrayVector(DeBlank(arg[0]));
}

//@@Signature
//function strtrim StrTrimFunction
//inputs x
//outputs y
//DOCBLOCK string_strtrim

struct OpStrTrim {
    static inline Array func(const Array& arg) {
	QString txt(arg.asString());
	while ((txt.length() >= 1) && (txt.right(1).at(0).isSpace() ||
	    txt.right(1).at(0).isNull()))
	    txt.chop(1);
	while ((txt.length() >= 1) && (txt.left(1).at(0).isSpace() ||
	    txt.left(1).at(0).isNull()))
	    txt.remove(0,1);
	Array z(txt);
	if (z.isEmpty() && z.is2D())
	    z = EmptyConstructor().toClass(StringArray);
	return z;
    }
};

ArrayVector StrTrimFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() == 0)
	throw Exception("strtrim requires at least one argument");
    return ArrayVector(StringOp<OpStrTrim>(arg[0]));
}

//@@Signature
//function strfind StrFindFunction
//inputs x pattern
//outputs y
//DOCBLOCK string_strfind
static Array StrFindFunc(const Array &r, const QString &pattern) {
    if (!r.isString()) return EmptyConstructor();
    QString x = r.asString();
    QVector<double> v;
    int from = 0;
    while (x.indexOf(pattern,from) >= 0) {
	from = x.indexOf(pattern,from) + 1;
	v.push_back(from);
    }
    if (v.size() == 0) return EmptyConstructor();
    BasicArray<double> vr(ToBasicArray(v));
    vr.reshape(NTuple(1,vr.length()));
    return Array(vr);
}

ArrayVector StrFindFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 2)
	throw Exception("strfind requires at least two arguments");
    Array y(arg[0]);
    if (!arg[1].isString()) throw Exception("second argument to strfind must be a string");
    QString pattern(arg[1].asString());
    if (y.dataClass() != CellArray)
	y = CellArrayFromArray(y);
    const BasicArray<Array> &rp(y.constReal<Array>());
    BasicArray<Array> sp(y.dimensions());
    for (index_t i=1;i<=rp.length();i++)
	sp[i] = StrFindFunc(rp[i],pattern);
    Array ret(sp);
    if (ret.isScalar()) return ArrayVector(ArrayFromCellArray(ret));
    return ArrayVector(ret);
}


//@@Signature
//function num2str Num2StrFunction
//inputs x format
//outputs string
//DOCBLOCK array_num2str

static void StripLeadingSpaces(StringVector& all_rows) {
    // Trim out leading spaces
    int leadspaces = 1000000;
    for (int i=0;i<all_rows.size();i++) {
	QString t = all_rows[i];
	int whitelead = 0;
	while (whitelead < t.size() && t[whitelead] == QChar(' ')) whitelead++;
	leadspaces = qMin(leadspaces,whitelead);
    }
    for (int i=0;i<all_rows.size();i++)
	all_rows[i].remove(0,leadspaces);
}

template <class T>
static Array Num2StrHelperReal(const BasicArray<T> &dp, const char *formatspec) {
    // First measure the length of the values
    ConstBasicIterator<T> iter1(&dp,1);
    int maxlen = 0;
    bool allint = true;
    while (iter1.isValid()) {
	for (index_t i=1;i<=iter1.size();i++) {
	    allint = allint && (iter1.get() == round(iter1.get()));
	    maxlen = qMax(maxlen,QString().sprintf(formatspec,fabs(static_cast<double>(iter1.get()))).length());
	    iter1.next();
	}
	iter1.nextSlice();
    }
    if (!allint) maxlen = qMax(maxlen,10);
    ConstBasicIterator<T> iter(&dp,1);
    StringVector all_rows;
    while (iter.isValid()) {
	QString row_string;
	for (index_t i=1;i<=iter.size();i++) {
	    QString d(QString().sprintf(formatspec,iter.get()));
	    d.replace(QString("inf"),QString("Inf"));
	    d.replace(QString("nan"),QString("NaN"));
	    QString pad(maxlen-d.length()+2,QChar(' '));
	    row_string += pad;
	    row_string += d;
	    iter.next();
	}
	all_rows << row_string;
	iter.nextSlice();
    }
    StripLeadingSpaces(all_rows);
    return StringArrayFromStringVector(all_rows,QChar(' '));
}

template <class T>
Array Num2StrHelperComplex(const BasicArray<T> &rp, const BasicArray<T> &ip, const char *formatspec) {
    ConstBasicIterator<T> iter_real1(&rp,1);
    ConstBasicIterator<T> iter_imag1(&ip,1);
    int maxlen = 0;
    bool allint = true;
    while (iter_real1.isValid() && iter_imag1.isValid()) {
	for (index_t i=1;i<=iter_real1.size();i++) {
	    allint = allint && (iter_real1.get() == round(iter_real1.get()));
	    allint = allint && (iter_imag1.get() == round(iter_imag1.get()));
	    QString dr = QString().sprintf(formatspec,iter_real1.get());
	    QString di = QString().sprintf(formatspec,iter_imag1.get());
	    maxlen = qMax(maxlen,dr.length()+di.length());
	    iter_real1.next();
	    iter_imag1.next();
	}
	iter_real1.nextSlice();
	iter_imag1.nextSlice();
    }
    if (!allint) maxlen = qMax(maxlen,20);
    ConstBasicIterator<T> iter_real(&rp,1);
    ConstBasicIterator<T> iter_imag(&ip,1);
    StringVector all_rows;
    while (iter_real.isValid() && iter_imag.isValid()) {
	QString row_string;
	for (index_t i=1;i<=iter_real.size();i++) {
	    QString dr(QString().sprintf(formatspec,iter_real.get()));
	    dr.replace(QString("inf"),QString("Inf"));
	    dr.replace(QString("nan"),QString("NaN"));
	    QString di(QString().sprintf(formatspec,iter_imag.get()));
	    di.replace(QString("inf"),QString("Inf"));
	    di.replace(QString("nan"),QString("NaN"));
	    if (iter_imag.get() >= 0) di = "+" + di;
	    QString d = dr+di;
	    QString pad(maxlen-d.length()+4,QChar(' '));
	    row_string += pad + d + "i";
	    iter_real.next();
	    iter_imag.next();
	}
	all_rows << row_string;
	iter_real.nextSlice();
	iter_imag.nextSlice();
    }
    StripLeadingSpaces(all_rows);
    return StringArrayFromStringVector(all_rows,QChar(' '));
}

template <typename T>
static ArrayVector Num2Str(const Array &X, const char *formatspec) {
    if (X.allReal())
	return ArrayVector(Num2StrHelperReal(X.constReal<T>(),formatspec));
    else
	return ArrayVector(Num2StrHelperComplex(X.constReal<T>(),X.constImag<T>(),formatspec));
}

#define MacroNum2Str(ctype,cls) \
  case cls: return Num2Str<ctype>(X,formatspec);

ArrayVector Num2StrFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() == 0)
	throw Exception("num2str function requires at least one argument");
    Array X(arg[0]);
    if (X.isReferenceType())
	throw Exception("num2str function requires a numeric input");
    if (X.isSparse())
	throw Exception("num2str not defined for sparse inputs");
    if (X.dataClass() == StringArray)
	return ArrayVector(X);
    X = X.asDenseArray();
    if (!X.is2D())
	X.reshape(NTuple(X.rows(),X.length()/X.rows()));
    char formatspec[1024];
    if ((X.dataClass() != Float) && (X.dataClass() != Double))
	sprintf(formatspec,"%%d");
    else
	sprintf(formatspec,"%%.5g"); //without preceding space
    if (arg.size() > 1) {
	Array format(arg[1]);
	if (format.isString()) {
	    strcpy(formatspec,qPrintable(format.asString()));
	    if (formatspec[0] == '%')
		for (int i = 1; i<1024; i++)
		    if (formatspec[i] == 'd') {
			X = X.toClass(Int32); //convert to integer if want print as integer
			break;
		    }
		    else if ((formatspec[i] <= 'a' && formatspec[i] >= 'z') || (formatspec[i] <= 'A' && formatspec[i] >= 'Z'))
			break;
	}
    }
    switch (X.dataClass())  {
  default: throw Exception("illegal type argument to num2str");
      MacroExpandCases(MacroNum2Str);
    }
}

//
//  This should become STR2DOUBLE.
//
// ArrayVector Str2NumFunction(int nargout, const ArrayVector& arg) {
//   if (arg.size() != 1)
//     throw Exception("str2num takes a single argument, the string to convert into a number");
//   Array data(arg[0]);
//   if (!data.isString())
//     return ArrayVector(EmptyConstructor());
//   return ArrayVector(Array(data.asDouble()));
// }


// How does sscanf work...
// a whitespace matches any number/type of whitespace
// a non-whitespace must be matched exactly
// a %d, %ld, etc.  reads a number... 
// a %s matches a sequence of characters that does
// not contain whitespaces...
// a mismatch - 

ArrayVector ScanfHelperFunction( QFile *fp, const ArrayVector& arg )
{
    Array format(arg[0]);
    Array errormsg = Array( QString("") );
    NTuple dims;
    index_t firstdim = -1, seconddim = 1;
    index_t nMaxRead = 1.e9; //good approximation of infinity
    index_t nRead = 0;

    if( arg.size() == 2 ){
	if( arg[1].isScalar() ){
	    if( IsFinite( arg[1].asDouble() ) ){
		nMaxRead = firstdim = arg[1].asIndexScalar();
	    }
	}
	else{
	    if( !IsFinite( arg[1].asDouble() ) )
		throw Exception("Illegal size.");
	    firstdim = arg[1].get(1).asIndexScalar();
	    if( IsFinite( arg[1].get(2).asDouble() ) )
		nMaxRead = arg[1].get(1).asIndexScalar()*arg[1].get(2).asIndexScalar();
	}
    }

    QString frmt = format.asString();
    QString::iterator frmt_iter = frmt.begin();

    bool stringarg = true;
    bool alldone = false;

    enum ReadStateType { ExpectNone, ExpectMatch, ExpectChar, ExpectInt, ExpectDouble, 
	ExpectIntBase, ExpectOctal, ExpectString, ExpectSignedInt, ExpectHex, ExpectScanlist, ExpectReadSkip };

    enum FormatStateType { ParsingMatch, ParsingFormat, ParsingFlag, ParsingWidth, ParsingType };
    enum ErrorStateType { ErrorNone, ErrorNoMatch, ErrorInvalidFormat };

    ReadStateType ReadState = ExpectMatch;
    FormatStateType FormatState = ParsingMatch;
    ErrorStateType ErrorState = ErrorNone;

    char MatchChar('\0');
    QChar FormatSpec('\0');
    QChar FormatWidthSpec('\0');
    bool FormatSkipFlag = false;
    int FormatFieldWidth = 0;

#define RESET_FORMAT { MatchChar = '\0'; FormatSpec = '\0'; FormatWidthSpec = '\0'; FormatSkipFlag = false; FormatFieldWidth = 0; }

    RESET_FORMAT;
    // Scan the string
    ArrayVector values;
    values.clear();

    try{
	while( !alldone && (nRead < nMaxRead) ){
	    switch( FormatState ){
		  case ParsingMatch:
		      if( *frmt_iter == '%' ){
			  FormatState = ParsingFormat;
			  ReadState = ExpectNone;
		      }
		      else{
			  MatchChar = (*frmt_iter).toAscii();
			  if( iscntrl(MatchChar) || isspace( MatchChar ) ){
			      ReadState = ExpectNone;
			      break;
			  }
			  ReadState = ExpectMatch;
		      }
		      break;
		  case ParsingFormat:
		      {
			  char curr_char = (*frmt_iter).toAscii();
			  if( convspec( curr_char ) ){
			      FormatSpec = curr_char;
			      FormatState = ParsingMatch;

			      ReadState = ExpectNone;

			      if( FormatSpec.toLower() == 'c' )
				  ReadState = ExpectChar;
			      if( FormatSpec.toLower() == 'd' )
				  ReadState = ExpectInt;
			      if( FormatSpec.toLower() == 'e' || FormatSpec.toLower() == 'f' || FormatSpec.toLower() == 'g' )
				  ReadState = ExpectDouble;
			      if( FormatSpec.toLower() == 'i' )
				  ReadState = ExpectIntBase;
			      if( FormatSpec.toLower() == 'o' )
				  ReadState = ExpectOctal;
			      if( FormatSpec.toLower() == 's' )
				  ReadState = ExpectString;
			      if( FormatSpec.toLower() == 'u' )
				  ReadState = ExpectSignedInt;
			      if( FormatSpec.toLower() == 'x' )
				  ReadState = ExpectHex;

			      if( ReadState == ExpectNone ){
				  ErrorState = ErrorInvalidFormat;
				  alldone = true;
				  continue;
			      }
			  }
			  else{
			      if( curr_char == 'h' || curr_char =='l' ){
				  if( !FormatSpec.isNull() || !FormatWidthSpec.isNull() ){
				      ErrorState = ErrorInvalidFormat;
				      alldone = true;
				      continue;
				  }
				  FormatWidthSpec = curr_char;
			      }
			      else{

				  if( isdigit( curr_char ) ){
				      if( !FormatSpec.isNull() ){
					  ErrorState = ErrorInvalidFormat;
					  alldone = true;
					  continue;
				      }
				      FormatFieldWidth = 10*FormatFieldWidth + digitvalue( curr_char );
				  }
				  else{
				      if( curr_char == '*' ){
					  if( FormatFieldWidth > 0 || !FormatSpec.isNull() ){
					      ErrorState = ErrorInvalidFormat;
					      alldone = true;
					      continue;
					  }
					  FormatSkipFlag = true;
				      }
				      else{
					  ErrorState = ErrorInvalidFormat;
					  alldone = true;
					  continue;
				      }
				  }
			      }
			  }
		      }
		      break;
		      //case ParsingFlag:
		      //	break;
		      //case ParsingWidth:
		      //	break;
		      //case ParsingType:
		      //	break;
		  default:
		      throw Exception("Internal error in sscanf helper function.");
		      break;
	    }
	    if( (++frmt_iter) == frmt.end() ){ //rewind the format string until run out of input
		frmt_iter = frmt.begin();
	    }

	    switch( ReadState ){
		  case ExpectNone:
		      break;
		  case ExpectMatch:
		      {
			  if( QFileReadChar( fp ) != MatchChar ){
			      ErrorState = ErrorNoMatch;
			      alldone = true;
			  }
		      }
		      break; 
		  case ExpectChar:
		      {
			  if( FormatFieldWidth == 0 ){
			      Array val( QString(QFileReadChar(fp)) );
			      if( !FormatSkipFlag )
				  values.push_back( val );
			  }
			  else{
			      Array val( QFileReadString( fp, FormatFieldWidth ) );
			      if( !FormatSkipFlag )
				  values.push_back( val );
			  }
			  ++nRead;
			  RESET_FORMAT;
		      }
		      break;
		  case ExpectInt:
		      {
			  try{
			      Array val( QFileReadInteger(fp, 10, FormatFieldWidth) );
			      if( !FormatSkipFlag )
				  values.push_back( val );
			      ++nRead;
			  }
			  catch( const Exception &e ){
			      ErrorState = ErrorNoMatch;
			      alldone = true;
			  }
			  stringarg = false;
			  RESET_FORMAT;
		      }
		      break; 
		  case ExpectDouble:
		      {
			  try{
			      Array val( QFileReadFloat(fp, FormatFieldWidth) );
			      if( !FormatSkipFlag )
				  values.push_back( val );
			      ++nRead;
			  }
			  catch( const Exception &e ){
			      ErrorState = ErrorNoMatch;
			      alldone = true;
			  }
			  stringarg = false;
			  RESET_FORMAT;
		      }
		      break;
		  case ExpectIntBase:
		      {
			  try{
			      Array val( QFileReadInteger(fp, 0, FormatFieldWidth) ); //autodetect base
			      if( !FormatSkipFlag )
				  values.push_back( val );
			      ++nRead;
			  }
			  catch( const Exception &e ){
			      ErrorState = ErrorNoMatch;
			      alldone = true;
			  }

			  stringarg = false;
			  RESET_FORMAT;
		      }
		      break;
		  case ExpectOctal:
		      {
			  try{ 
			      Array val( QFileReadInteger(fp, 8, FormatFieldWidth) );
			      if( !FormatSkipFlag )
				  values.push_back( val );
			      ++nRead;
			  }
			  catch( const Exception &e ){
			      ErrorState = ErrorNoMatch;
			      alldone = true;
			  }

			  stringarg = false;
			  RESET_FORMAT;
		      }
		      break;
		  case ExpectString:
		      {
			  try{
			      Array val( QFileReadString(fp, FormatFieldWidth) );
			      if( !FormatSkipFlag )
				  values.push_back( val );
			      ++nRead;
			  }
			  catch( const Exception &e ){
			      ErrorState = ErrorNoMatch;
			      alldone = true;
			  }

			  RESET_FORMAT;
		      }
		      break;
		  case ExpectSignedInt:
		      {
			  try{
			      Array val( QFileReadInteger(fp, 10, FormatFieldWidth) );
			      if( !FormatSkipFlag )
				  values.push_back( val );
			      ++nRead;
			  }
			  catch( const Exception &e ){
			      ErrorState = ErrorNoMatch;
			      alldone = true;
			  }

			  stringarg = false;
			  RESET_FORMAT;
		      }
		      break;
		  case ExpectHex:
		      {
			  try{
			      Array val( QFileReadInteger(fp, 16, FormatFieldWidth) );
			      if( !FormatSkipFlag )
				  values.push_back( val );
			      ++nRead;
			  }
			  catch( const Exception &e ){
			      ErrorState = ErrorNoMatch;
			      alldone = true;
			  }

			  stringarg = false;
			  RESET_FORMAT;
		      }
		      break;
		  case ExpectScanlist:
		      {
			  throw Exception("Scanlist format is not Implemented");
		      }
		      break;
		  case ExpectReadSkip:
		      {
			  QFileReadChar( fp ); //just skip over a character
		      }
		      break;
		  default:
		      throw Exception("Internal Error in ScanfHelperFunction");
		      break;
	    }
	    if( fp->atEnd() )
		alldone = true;
	}

    }
    catch( const Exception &e ){
	ErrorState = ErrorNoMatch;
    }

    ArrayVector ret; 
    ret.push_front( Array( static_cast<double>(fp->pos()-1) ) );
    //process error reporting
    switch(ErrorState){ 
      case ErrorNone:
	  ret.push_front( Array() );
	  break;
      case ErrorInvalidFormat:
	  ret.push_front( Array(QString("Invalid Format")) );
	  break;
      case ErrorNoMatch:
	  ret.push_front( Array(QString("Matching failure in format.")) );
	  break;
    }
    ret.push_front( Array( static_cast<double>(values.size()) ) );

    int nElem = 0; //number of elements in the output array
    
    //calculate the length of the array

    for( int i=0; i<values.size(); i++ ){
	nElem += values[i].length();
    }

    if( firstdim > 0 ){
	seconddim = (nElem / static_cast<int>(firstdim)) + 1;
	if( seconddim > 1 )
	    nElem = static_cast<int>(firstdim*seconddim);
	else
	    firstdim = nElem;
    }
    else{
	firstdim = nElem;
    }

    NTuple dim( firstdim, seconddim ); 
    if( !stringarg ){ //At least one of the read values was numerical. Convert strings to numbers.
	Array out( Double, NTuple(nElem,1) );

	//double *data = (double*) ret[0].getVoidPointer();
	int ind = 1;

	for( int i=0; i<values.size(); i++ ){
	    if( values[i].isString() ){
		QString str( values[i].asString() );
		for( int j=0; j < str.length(); j++ ){
		    out.set(ind++, Array(QChar(str[j])));
		}
	    }
	    else{
		out.set(ind++, values[i]);
	    }
	}
	ret.push_front( out );
    }
    else{ //all string input
	QString outstring;
	
	for( int i=0; i<values.size(); i++ ){
	    if( values[i].isString() ){
		outstring.append( values[i].asString() );
	    }
	    else{
		throw Exception("Internal Error in sscanf.");
	    }
	}
	for( int i=outstring.length(); i<nElem; ++i)
	    outstring.push_back(QChar('\0'));

	ret.push_front( Array( outstring ) );
	dim=NTuple(1,nElem);
    }

    ret[0].reshape( dim );
    return ret;
}


//@@Signature
//function sscanf SscanfFunction
//inputs varargin
//outputs varargout
//DOCBLOCK io_sscanf
ArrayVector SscanfFunction(int nargout, const ArrayVector& arg) {
    if ((arg.size() > 3) || (arg.size() < 2) || (!arg[0].isString()) || (!arg[1].isString()))
	throw Exception("incorrect number or types or the parameters");
    QTemporaryFile fp;
    if (!fp.open())
	throw Exception("sscanf was unable to open a temp file (and so it won't work)");
    QTextStream out(&fp);
    out << arg[0].asString();
    out.seek(0);
    fp.seek(0);

    ArrayVector helper_arg; 
    helper_arg << arg[1];
    if( arg.size() == 3 )
	helper_arg << arg[2];

    return ScanfHelperFunction(&fp,helper_arg);
}
