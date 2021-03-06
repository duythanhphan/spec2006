/*
 * This file was generated automatically by xsubpp version 1.9508 from the
 * contents of Parser.xs. Do not edit this file, edit Parser.xs instead.
 *
 *	ANY CHANGES MADE HERE WILL BE LOST!
 *
 */

/* #line 1 "Parser.xs" */
/* $Id: Parser.xs,v 2.131 2005/01/06 09:02:28 gisle Exp $
 *
 * Copyright 1999-2005, Gisle Aas.
 * Copyright 1999-2000, Michael A. Chase.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the same terms as Perl itself.
 */


/*
 * Standard XS greeting.
 */
#ifdef __cplusplus
extern "C" {
#endif
#define PERL_NO_GET_CONTEXT     /* we want efficiency */
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#ifdef __cplusplus
}
#endif

/* Added for SPEC CPU */
#define MARKED_SECTION

/*
 * Some perl version compatibility gruff.
 */
#include "patchlevel.h"
#if PATCHLEVEL <= 4 /* perl5.004_XX */

#ifndef PL_sv_undef
   #define PL_sv_undef sv_undef
   #define PL_sv_yes   sv_yes
#endif

#ifndef PL_hexdigit
   #define PL_hexdigit hexdigit
#endif

#ifndef ERRSV
   #define ERRSV GvSV(errgv)
#endif

#if (PATCHLEVEL == 4 && SUBVERSION <= 4)
/* The newSVpvn function was introduced in perl5.004_05 */
static SV *
newSVpvn(char *s, STRLEN len)
{
    register SV *sv = newSV(0);
    sv_setpvn(sv,s,len);
    return sv;
}
#endif /* not perl5.004_05 */
#endif /* perl5.004_XX */

#ifndef dNOOP
   #define dNOOP extern int errno
#endif
#ifndef dTHX
   #define dTHX dNOOP
   #define pTHX_
   #define aTHX_
#endif

#ifndef MEMBER_TO_FPTR
   #define MEMBER_TO_FPTR(x) (x)
#endif

#ifndef INT2PTR
   #define INT2PTR(any,d)  (any)(d)
   #define PTR2IV(p)       (IV)(p)
#endif


#if PATCHLEVEL > 6 || (PATCHLEVEL == 6 && SUBVERSION > 0)
   #define RETHROW	   croak(Nullch)
#else
   #define RETHROW    { STRLEN my_na; croak("%s", SvPV(ERRSV, my_na)); }
#endif

#if PATCHLEVEL < 8
   /* No useable Unicode support */
   /* Make these harmless if present */
   #undef SvUTF8
   #undef SvUTF8_on
   #undef SvUTF8_off
   #define SvUTF8(sv)      0
   #define SvUTF8_on(sv)   0
   #define SvUTF8_off(sv)  0
#else
   #define UNICODE_HTML_PARSER
#endif

#ifdef G_WARN_ON
   #define DOWARN (PL_dowarn & G_WARN_ON)
#else
   #define DOWARN PL_dowarn
#endif

/*
 * Include stuff.  We include .c files instead of linking them,
 * so that they don't have to pollute the external dll name space.
 */

#ifdef EXTERN
  #undef EXTERN
#endif

#define EXTERN static /* Don't pollute */

#include "hparser.h"
#include "parser-util.c"
#include "hparser.c"


/*
 * Support functions for the XS glue
 */

static SV*
check_handler(pTHX_ SV* h)
{
    if (SvROK(h)) {
	SV* myref = SvRV(h);
	if (SvTYPE(myref) == SVt_PVCV)
	    return newSVsv(h);
	if (SvTYPE(myref) == SVt_PVAV)
	    return SvREFCNT_inc(myref);
	croak("Only code or array references allowed as handler");
    }
    return SvOK(h) ? newSVsv(h) : 0;
}


static PSTATE*
get_pstate_iv(pTHX_ SV* sv)
{
    PSTATE* p = INT2PTR(PSTATE*, SvIV(sv));
    if (p->signature != P_SIGNATURE)
	croak("Bad signature in parser state object at %p", p);
    return p;
}


static PSTATE*
get_pstate_hv(pTHX_ SV* sv)                               /* used by XS typemap */
{
    HV* hv;
    SV** svp;

    sv = SvRV(sv);
    if (!sv || SvTYPE(sv) != SVt_PVHV)
	croak("Not a reference to a hash");
    hv = (HV*)sv;
    svp = hv_fetch(hv, "_hparser_xs_state", 17, 0);
    if (svp) {
	if (SvROK(*svp))
	    return get_pstate_iv(aTHX_ SvRV(*svp));
	else
	    croak("_hparser_xs_state element is not a reference");
    }
    croak("Can't find '_hparser_xs_state' element in HTML::Parser hash");
    return 0;
}


static void
free_pstate(pTHX_ PSTATE* pstate)
{
    int i;
    SvREFCNT_dec(pstate->buf);
    SvREFCNT_dec(pstate->pend_text);
    SvREFCNT_dec(pstate->skipped_text);
#ifdef MARKED_SECTION
    SvREFCNT_dec(pstate->ms_stack);
#endif
    SvREFCNT_dec(pstate->bool_attr_val);
    for (i = 0; i < EVENT_COUNT; i++) {
	SvREFCNT_dec(pstate->handlers[i].cb);
	SvREFCNT_dec(pstate->handlers[i].argspec);
    }

    SvREFCNT_dec(pstate->report_tags);
    SvREFCNT_dec(pstate->ignore_tags);
    SvREFCNT_dec(pstate->ignore_elements);
    SvREFCNT_dec(pstate->ignoring_element);

    SvREFCNT_dec(pstate->tmp);

    pstate->signature = 0;
    Safefree(pstate);
}


static int
magic_free_pstate(pTHX_ SV *sv, MAGIC *mg)
{
    free_pstate(aTHX_ get_pstate_iv(aTHX_ sv));
    return 0;
}


MGVTBL vtbl_free_pstate = {0, 0, 0, 0, MEMBER_TO_FPTR(magic_free_pstate)};



/*
 *  XS interface definition.
 */

/* #line 223 "Parser.c" */
XS(XS_HTML__Parser__alloc_pstate); /* prototype to pass -Wmissing-prototypes */
XS(XS_HTML__Parser__alloc_pstate)
{
    dXSARGS;
    if (items != 1)
	Perl_croak(aTHX_ "Usage: HTML::Parser::_alloc_pstate(self)");
    {
	SV*	self = ST(0);
/* #line 221 "Parser.xs" */
	PSTATE* pstate;
	SV* sv;
	HV* hv;
        MAGIC* mg;

/* #line 238 "Parser.c" */
/* #line 227 "Parser.xs" */
	sv = SvRV(self);
        if (!sv || SvTYPE(sv) != SVt_PVHV)
            croak("Not a reference to a hash");
	hv = (HV*)sv;

	Newz(56, pstate, 1, PSTATE);
	pstate->signature = P_SIGNATURE;
	pstate->entity2char = get_hv("HTML::Entities::entity2char", TRUE);
	pstate->tmp = NEWSV(0, 20);

	sv = newSViv(PTR2IV(pstate));
	sv_magic(sv, 0, '~', 0, 0);
	mg = mg_find(sv, '~');
        assert(mg);
        mg->mg_virtual = &vtbl_free_pstate;
	SvREADONLY_on(sv);

	hv_store(hv, "_hparser_xs_state", 17, newRV_noinc(sv), 0);
/* #line 258 "Parser.c" */
    }
    XSRETURN_EMPTY;
}

XS(XS_HTML__Parser_parse); /* prototype to pass -Wmissing-prototypes */
XS(XS_HTML__Parser_parse)
{
    dXSARGS;
    if (items != 2)
	Perl_croak(aTHX_ "Usage: HTML::Parser::parse(self, chunk)");
    SP -= items;
    {
	SV*	self = ST(0);
	SV*	chunk = ST(1);
/* #line 251 "Parser.xs" */
	PSTATE* p_state = get_pstate_hv(aTHX_ self);
/* #line 275 "Parser.c" */
/* #line 253 "Parser.xs" */
	if (p_state->parsing)
    	    croak("Parse loop not allowed");
        p_state->parsing = 1;
	if (SvROK(chunk) && SvTYPE(SvRV(chunk)) == SVt_PVCV) {
	    SV* generator = chunk;
	    STRLEN len;
	    do {
                int count;
		PUSHMARK(SP);
	        count = call_sv(generator, G_SCALAR|G_EVAL);
		SPAGAIN;
		chunk = count ? POPs : 0;
	        PUTBACK;

	        if (SvTRUE(ERRSV)) {
		    p_state->parsing = 0;
		    p_state->eof = 0;
		    RETHROW;
                }

		if (chunk && SvOK(chunk)) {
		    (void)SvPV(chunk, len);  /* get length */
		}
		else {
		    len = 0;
                }
		parse(aTHX_ p_state, len ? chunk : 0, self);
	        SPAGAIN;

            } while (len && !p_state->eof);
        }
	else {
	    parse(aTHX_ p_state, chunk, self);
            SPAGAIN;
        }
        p_state->parsing = 0;
	if (p_state->eof) {
	    p_state->eof = 0;
            PUSHs(sv_newmortal());
        }
	else {
	    PUSHs(self);
	}
/* #line 320 "Parser.c" */
	PUTBACK;
	return;
    }
}

XS(XS_HTML__Parser_eof); /* prototype to pass -Wmissing-prototypes */
XS(XS_HTML__Parser_eof)
{
    dXSARGS;
    if (items != 1)
	Perl_croak(aTHX_ "Usage: HTML::Parser::eof(self)");
    SP -= items;
    {
	SV*	self = ST(0);
/* #line 301 "Parser.xs" */
	PSTATE* p_state = get_pstate_hv(aTHX_ self);
/* #line 337 "Parser.c" */
/* #line 303 "Parser.xs" */
        if (p_state->parsing)
            p_state->eof = 1;
        else {
	    p_state->parsing = 1;
	    parse(aTHX_ p_state, 0, self); /* flush */
	    p_state->parsing = 0;
	}
	PUSHs(self);
/* #line 347 "Parser.c" */
	PUTBACK;
	return;
    }
}

XS(XS_HTML__Parser_strict_comment); /* prototype to pass -Wmissing-prototypes */
XS(XS_HTML__Parser_strict_comment)
{
    dXSARGS;
    dXSI32;
    if (items < 1)
       Perl_croak(aTHX_ "Usage: %s(pstate, ...)", GvNAME(CvGV(cv)));
    {
	PSTATE*	pstate = get_pstate_hv(aTHX_ ST(0));
/* #line 327 "Parser.xs" */
	bool *attr;
/* #line 364 "Parser.c" */
	SV *	RETVAL;
/* #line 329 "Parser.xs" */
        switch (ix) {
	case  1: attr = &pstate->strict_comment;       break;
	case  2: attr = &pstate->strict_names;         break;
	case  3: attr = &pstate->xml_mode;             break;
	case  4: attr = &pstate->unbroken_text;        break;
        case  5:
#ifdef MARKED_SECTION
		 attr = &pstate->marked_sections;      break;
#else
	         croak("marked sections not supported"); break;
#endif
	case  6: attr = &pstate->attr_encoded;         break;
	case  7: attr = &pstate->case_sensitive;       break;
	case  8: attr = &pstate->strict_end;           break;
	case  9: attr = &pstate->closing_plaintext;    break;
#ifdef UNICODE_HTML_PARSER
        case 10: attr = &pstate->utf8_mode;            break;
#else
	case 10: croak("The utf8_mode does not work with this perl; perl-5.8 or better required");
#endif
	default:
	    croak("Unknown boolean attribute (%d)", ix);
        }
	RETVAL = boolSV(*attr);
	if (items > 1)
	    *attr = SvTRUE(ST(1));
/* #line 393 "Parser.c" */
	ST(0) = RETVAL;
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}

XS(XS_HTML__Parser_boolean_attribute_value); /* prototype to pass -Wmissing-prototypes */
XS(XS_HTML__Parser_boolean_attribute_value)
{
    dXSARGS;
    if (items < 1)
	Perl_croak(aTHX_ "Usage: HTML::Parser::boolean_attribute_value(pstate, ...)");
    {
	PSTATE*	pstate = get_pstate_hv(aTHX_ ST(0));
	SV *	RETVAL;
/* #line 362 "Parser.xs" */
	RETVAL = pstate->bool_attr_val ? newSVsv(pstate->bool_attr_val)
				       : &PL_sv_undef;
	if (items > 1) {
	    SvREFCNT_dec(pstate->bool_attr_val);
	    pstate->bool_attr_val = newSVsv(ST(1));
        }
/* #line 416 "Parser.c" */
	ST(0) = RETVAL;
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}

XS(XS_HTML__Parser_ignore_tags); /* prototype to pass -Wmissing-prototypes */
XS(XS_HTML__Parser_ignore_tags)
{
    dXSARGS;
    dXSI32;
    if (items < 1)
       Perl_croak(aTHX_ "Usage: %s(pstate, ...)", GvNAME(CvGV(cv)));
    {
	PSTATE*	pstate = get_pstate_hv(aTHX_ ST(0));
/* #line 379 "Parser.xs" */
	HV** attr;
	int i;
/* #line 435 "Parser.c" */
/* #line 382 "Parser.xs" */
	switch (ix) {
	case  1: attr = &pstate->report_tags;     break;
	case  2: attr = &pstate->ignore_tags;     break;
	case  3: attr = &pstate->ignore_elements; break;
	default:
	    croak("Unknown tag-list attribute (%d)", ix);
	}
	if (GIMME_V != G_VOID)
	    croak("Can't report tag lists yet");

	items--;  /* pstate */
	if (items) {
	    if (*attr)
		hv_clear(*attr);
	    else
		*attr = newHV();

	    for (i = 0; i < items; i++) {
		SV* sv = ST(i+1);
		if (SvROK(sv)) {
		    sv = SvRV(sv);
		    if (SvTYPE(sv) == SVt_PVAV) {
			AV* av = (AV*)sv;
			STRLEN j;
			STRLEN len = av_len(av) + 1;
			for (j = 0; j < len; j++) {
			    SV**svp = av_fetch(av, j, 0);
			    if (svp) {
				hv_store_ent(*attr, *svp, newSViv(0), 0);
			    }
			}
		    }
		    else
			croak("Tag list must be plain scalars and arrays");
		}
		else {
		    hv_store_ent(*attr, sv, newSViv(0), 0);
		}
	    }
	}
	else if (*attr) {
	    SvREFCNT_dec(*attr);
            *attr = 0;
	}
/* #line 481 "Parser.c" */
    }
    XSRETURN_EMPTY;
}

XS(XS_HTML__Parser_handler); /* prototype to pass -Wmissing-prototypes */
XS(XS_HTML__Parser_handler)
{
    dXSARGS;
    if (items < 2)
	Perl_croak(aTHX_ "Usage: HTML::Parser::handler(pstate, eventname, ...)");
    SP -= items;
    {
	PSTATE*	pstate = get_pstate_hv(aTHX_ ST(0));
	SV*	eventname = ST(1);
/* #line 432 "Parser.xs" */
	STRLEN name_len;
	char *name = SvPV(eventname, name_len);
        int event = -1;
        int i;
        struct p_handler *h;
/* #line 502 "Parser.c" */
/* #line 438 "Parser.xs" */
	/* map event name string to event_id */
	for (i = 0; i < EVENT_COUNT; i++) {
	    if (strEQ(name, event_id_str[i])) {
	        event = i;
	        break;
	    }
	}
        if (event < 0)
	    croak("No handler for %s events", name);

	h = &pstate->handlers[event];

	/* set up return value */
	if (h->cb) {
	    PUSHs((SvTYPE(h->cb) == SVt_PVAV)
	                 ? sv_2mortal(newRV_inc(h->cb))
	                 : sv_2mortal(newSVsv(h->cb)));
	}
        else {
	    PUSHs(&PL_sv_undef);
        }

        /* update */
        if (items > 3) {
	    SvREFCNT_dec(h->argspec);
	    h->argspec = 0;
	    h->argspec = argspec_compile(ST(3), pstate);
	}
        if (items > 2) {
	    SvREFCNT_dec(h->cb);
            h->cb = 0;
	    h->cb = check_handler(aTHX_ ST(2));
	}
/* #line 537 "Parser.c" */
	PUTBACK;
	return;
    }
}

XS(XS_HTML__Entities_decode_entities); /* prototype to pass -Wmissing-prototypes */
XS(XS_HTML__Entities_decode_entities)
{
    dXSARGS;
   PERL_UNUSED_VAR(ax); /* -Wall */
    SP -= items;
    {
/* #line 478 "Parser.xs" */
        int i;
	HV *entity2char = get_hv("HTML::Entities::entity2char", FALSE);
/* #line 553 "Parser.c" */
/* #line 481 "Parser.xs" */
	if (GIMME_V == G_SCALAR && items > 1)
            items = 1;
	for (i = 0; i < items; i++) {
	    if (GIMME_V != G_VOID)
	        ST(i) = sv_2mortal(newSVsv(ST(i)));
	    else if (SvREADONLY(ST(i)))
		croak("Can't inline decode readonly string");
	    decode_entities(aTHX_ ST(i), entity2char, 0);
	}
	SP += items;
/* #line 565 "Parser.c" */
	PUTBACK;
	return;
    }
}

XS(XS_HTML__Entities__decode_entities); /* prototype to pass -Wmissing-prototypes */
XS(XS_HTML__Entities__decode_entities)
{
    dXSARGS;
    if (items < 2)
	Perl_croak(aTHX_ "Usage: HTML::Entities::_decode_entities(string, entities, ...)");
    {
	SV*	string = ST(0);
	SV*	entities = ST(1);
/* #line 497 "Parser.xs" */
	HV* entities_hv;
        bool allow_unterminated = (items > 2) ? SvTRUE(ST(2)) : 0;
/* #line 583 "Parser.c" */
/* #line 500 "Parser.xs" */
        if (SvOK(entities)) {
	    if (SvROK(entities) && SvTYPE(SvRV(entities)) == SVt_PVHV) {
		entities_hv = (HV*)SvRV(entities);
	    }
            else {
		croak("2nd argument must be hash reference");
            }
        }
        else {
            entities_hv = 0;
        }
	if (SvREADONLY(string))
	    croak("Can't inline decode readonly string");
	decode_entities(aTHX_ string, entities_hv, allow_unterminated);
/* #line 599 "Parser.c" */
    }
    XSRETURN_EMPTY;
}

XS(XS_HTML__Entities__probably_utf8_chunk); /* prototype to pass -Wmissing-prototypes */
XS(XS_HTML__Entities__probably_utf8_chunk)
{
    dXSARGS;
    if (items != 1)
	Perl_croak(aTHX_ "Usage: HTML::Entities::_probably_utf8_chunk(string)");
    {
	SV*	string = ST(0);
/* #line 519 "Parser.xs" */
        STRLEN len;
        char *s;
/* #line 615 "Parser.c" */
	bool	RETVAL;
/* #line 522 "Parser.xs" */
#ifdef UNICODE_HTML_PARSER
        sv_utf8_downgrade(string, 0);
	s = SvPV(string, len);
        RETVAL = probably_utf8_chunk(aTHX_ s, len);
#else
	croak("_probably_utf8_chunk() only works for Unicode enabled perls");
#endif
/* #line 625 "Parser.c" */
	ST(0) = boolSV(RETVAL);
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}

XS(XS_HTML__Entities_UNICODE_SUPPORT); /* prototype to pass -Wmissing-prototypes */
XS(XS_HTML__Entities_UNICODE_SUPPORT)
{
    dXSARGS;
    if (items != 0)
	Perl_croak(aTHX_ "Usage: HTML::Entities::UNICODE_SUPPORT()");
    {
	int	RETVAL;
	dXSTARG;
/* #line 536 "Parser.xs" */
#ifdef UNICODE_HTML_PARSER
       RETVAL = 1;
#else
       RETVAL = 0;
#endif
/* #line 647 "Parser.c" */
	XSprePUSH; PUSHi((IV)RETVAL);
    }
    XSRETURN(1);
}

#ifdef __cplusplus
extern "C"
#endif
XS(boot_HTML__Parser); /* prototype to pass -Wmissing-prototypes */
XS(boot_HTML__Parser)
{
    dXSARGS;
    char* file = __FILE__;

    XS_VERSION_BOOTCHECK ;

    {
        CV * cv ;

        newXS("HTML::Parser::_alloc_pstate", XS_HTML__Parser__alloc_pstate, file);
        newXS("HTML::Parser::parse", XS_HTML__Parser_parse, file);
        newXS("HTML::Parser::eof", XS_HTML__Parser_eof, file);
        cv = newXS("HTML::Parser::closing_plaintext", XS_HTML__Parser_strict_comment, file);
        XSANY.any_i32 = 9 ;
        cv = newXS("HTML::Parser::strict_end", XS_HTML__Parser_strict_comment, file);
        XSANY.any_i32 = 8 ;
        cv = newXS("HTML::Parser::marked_sections", XS_HTML__Parser_strict_comment, file);
        XSANY.any_i32 = 5 ;
        cv = newXS("HTML::Parser::case_sensitive", XS_HTML__Parser_strict_comment, file);
        XSANY.any_i32 = 7 ;
        cv = newXS("HTML::Parser::unbroken_text", XS_HTML__Parser_strict_comment, file);
        XSANY.any_i32 = 4 ;
        cv = newXS("HTML::Parser::strict_comment", XS_HTML__Parser_strict_comment, file);
        XSANY.any_i32 = 1 ;
        cv = newXS("HTML::Parser::xml_mode", XS_HTML__Parser_strict_comment, file);
        XSANY.any_i32 = 3 ;
        cv = newXS("HTML::Parser::attr_encoded", XS_HTML__Parser_strict_comment, file);
        XSANY.any_i32 = 6 ;
        cv = newXS("HTML::Parser::strict_names", XS_HTML__Parser_strict_comment, file);
        XSANY.any_i32 = 2 ;
        cv = newXS("HTML::Parser::utf8_mode", XS_HTML__Parser_strict_comment, file);
        XSANY.any_i32 = 10 ;
        newXS("HTML::Parser::boolean_attribute_value", XS_HTML__Parser_boolean_attribute_value, file);
        cv = newXS("HTML::Parser::ignore_tags", XS_HTML__Parser_ignore_tags, file);
        XSANY.any_i32 = 2 ;
        cv = newXS("HTML::Parser::ignore_elements", XS_HTML__Parser_ignore_tags, file);
        XSANY.any_i32 = 3 ;
        cv = newXS("HTML::Parser::report_tags", XS_HTML__Parser_ignore_tags, file);
        XSANY.any_i32 = 1 ;
        newXS("HTML::Parser::handler", XS_HTML__Parser_handler, file);
        newXS("HTML::Entities::decode_entities", XS_HTML__Entities_decode_entities, file);
        newXS("HTML::Entities::_decode_entities", XS_HTML__Entities__decode_entities, file);
        newXS("HTML::Entities::_probably_utf8_chunk", XS_HTML__Entities__probably_utf8_chunk, file);
        newXSproto("HTML::Entities::UNICODE_SUPPORT", XS_HTML__Entities_UNICODE_SUPPORT, file, "");
    }
    XSRETURN_YES;
}

