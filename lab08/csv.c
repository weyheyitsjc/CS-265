/*
 * csv.c - Implementation for a csv library
 *
 * Modified from code in Kernighan & Pike, _The Practice of Programming_
 *   Copyright (C) 1999 Lucent Technologies 
 *   Excerpted from 'The Practice of Programming' 
 *   by Brian W. Kernighan and Rob Pike 
 *
 * Kurt Schmidt
 * 3/2018
 *
 * gcc 5.4.0 20160609 on
 * Linux 4.13.0-32-generic
 *
 * EDITOR:  tabstop=2 cols=120
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "csv.h"


static char fieldsep[] = "," ; /* field separator chars */

	/***** Prototypes for local helper functions ******/
static char *advquoted( char* ) ;
static int split( csv_t* ) ;

/* TODO
 *  Copy all the functions (but for main) from csvgetline2 here.  Not the prototypes.  You already have them in
 *  csv.h.  Functions that access global CSV data need to be modified, take a pointer to a CSV struct as the first
 *  argument, and any references need to be modified
 */

/* endofline: check for and consume \r, \n, \r\n, or EOF */
static int endofline( FILE *fin, int c )
{
	int eol ;

	eol = ( c=='\r' || c=='\n' ) ;
	if( c=='\r' ) {
		c = getc( fin ) ;
		if( c!='\n' && c != EOF )
			ungetc( c, fin ) ;	/* read too far; put c back */
	}
	return eol ;
}

	/* reset: set variables back to starting values */
static void reset(csv_t* s )
{
	free( s->line );	 /* free(NULL) permitted by ANSI C */
	free( s->sline ) ;
	free( s->field ) ;
	s->line = NULL ;
	s->sline = NULL ;
	s->field = NULL ;
	s->maxline = s->maxfield = s->nfield = 0 ;
}

	/* csvgetline:  get one line, grow as needed */
	/* sample input: "LU",86.25,"11/4/1998","2:19PM",+4.0625 */
char* csvgetline( csv_t* s )
{	
	int i, c ;
	char *newl, *news ;

	if( s->line==NULL ) {			/* allocate on first call */
		s->maxline = s->maxfield = 1 ;
		s->line = (char*) malloc( s->maxline ) ;
		s->sline = (char*) malloc( s->maxline ) ;
		s->field = (char**) malloc( s->maxfield*sizeof( s->field[0] )) ;
		if( s->line==NULL || s->sline==NULL || s->field==NULL) {
			reset(s) ;
			return NULL ;		/* out of memory */
		}
	}

	for( i=0; (c=getc( s->fin ))!=EOF && ! endofline(s->fin,c); i++ ) {
		if( i>=s->maxline-1 ) {	  /* grow line */
			s->maxline *= 2;		    /* double current size */
			newl = (char*) realloc( s->line, s->maxline ) ;
			if( newl==NULL ) {
				reset(s) ;
				return NULL ;
			}
			s->line = newl ;
			news = (char*) realloc( s->sline, s->maxline ) ;
			if( news==NULL ) {
				reset(s) ;
				return NULL ;
			}
			s->sline = news ;
		}
		s->line[i] = c ;
	}  /* for i */

	s->line[i] = '\0' ;
	if( split(s)==NOMEM ) {
		reset(s) ;
		return NULL;			/* out of memory */
	}
	return (c==EOF && i==0) ? NULL : s->line ;
}

/* split: split line into fields */
static int split( csv_t* s )
{
	char *p, **newf ;
	char *sepp; /* pointer to temporary separator character */
	int sepc;   /* temporary separator character */

	s->nfield = 0 ;
	if( s->line[0]=='\0' )
		return 0 ;
	strcpy( s->sline, s->line ) ;
	p = s->sline ;

	do {
		if( s->nfield>=s->maxfield ) {
			s->maxfield *= 2;			/* double current size */
			newf = (char**) realloc( s->field, 
							s->maxfield * sizeof( s->field[0] )) ;
			if( newf==NULL )
				return NOMEM ;
			s->field = newf ;
		}
		if( *p=='"' )
			sepp = advquoted( ++p ) ;	/* skip initial quote */
		else
			sepp = p + strcspn( p, fieldsep ) ;
		sepc = sepp[0] ;
		sepp[0] = '\0' ;			/* terminate field */
		s->field[s->nfield++] = p ;
		p = sepp + 1 ;
	} while( sepc==',' ) ;

	return s->nfield ;
}

/* advquoted: quoted field; return pointer to next separator */
static char *advquoted( char* p )
{
	int i, j ;

	for( i=j=0; p[j]!='\0'; ++i, ++j ) {
		if( p[j]=='"' && p[++j]!='"' ) {
				/* copy up to next separator or \0 */
			int k = strcspn( p+j, fieldsep ) ;
			memmove( p+i, p+j, k ) ;
			i += k ;
			j += k ;
			break ;
		}
		p[i] = p[j] ;
	}
	p[i] = '\0' ;
	return p + j ;
}

/* csvfield:  return pointer to n-th field */
char* csvfield( csv_t* s, int n )
{
	if( n<0 || n>=s->nfield )
		return NULL ;
	return s->field[n] ;
}

/* csvnfield:  return number of fields */ 
int csvnfield( csv_t* s ) 
{
	return s->nfield ;
}

csv_t* csv_init( FILE *f )
{
	csv_t *rv = (csv_t*) malloc( sizeof( csv_t )) ;
	rv->fin = f ;
	reset( rv ) ;

	return( rv ) ;
}

void kill( csv_t* s )
{
	reset( s ) ;
	free( s ) ;
}