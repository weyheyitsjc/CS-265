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

/* TODO
 *  Copy all the functions (but for main) from csvgetline2 here.  Not the prototypes.  You already have them in
 *  csv.h.  Functions that access global CSV data need to be modified, take a pointer to a CSV struct as the first
 *  argument, and any references need to be modified
 */

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
static void reset( void )
{
	free( line );	 /* free(NULL) permitted by ANSI C */
	free( sline ) ;
	free( field ) ;
	line = NULL ;
	sline = NULL ;
	field = NULL ;
	maxline = maxfield = nfield = 0 ;
}

	/* csvgetline:  get one line, grow as needed */
	/* sample input: "LU",86.25,"11/4/1998","2:19PM",+4.0625 */
char* csvgetline( FILE *fin, csv_t* )
{	
	int i, c ;
	char *newl, *news ;

	if( csv_t.line==NULL ) {			/* allocate on first call */
		csv_t.maxline = csv_t.maxfield = 1 ;
		csv_t.line = (char*) malloc( csv_t.maxline ) ;
		csv_t.sline = (char*) malloc( csv_t.maxline ) ;
		csv_t.field = (char**) malloc( csv_t.maxfield*sizeof( csv_t.field[0] )) ;
		if( csv_t.line==NULL || csv_t.sline==NULL || csv_t.field==NULL) {
			reset() ;
			return NULL ;		/* out of memory */
		}
	}

	for( i=0; (c=getc( fin ))!=EOF && ! endofline(fin,c); i++ ) {
		if( i>=csv_t.maxline-1 ) {	  /* grow line */
			csv_t.maxline *= 2;		    /* double current size */
			newl = (char*) realloc( csv_t.line, csv_t.maxline ) ;
			if( newl==NULL ) {
				reset() ;
				return NULL ;
			}
			csv_t.line = newl ;
			news = (char*) realloc( csv_t.sline, csv_t.maxline ) ;
			if( news==NULL ) {
				reset() ;
				return NULL ;
			}
			csv_t.sline = news ;
		}
		line[i] = c ;
	}  /* for i */

	line[i] = '\0' ;
	if( split()==NOMEM ) {
		reset() ;
		return NULL;			/* out of memory */
	}
	return (c==EOF && i==0) ? NULL : line ;
}

/* split: split line into fields */
static int split( csv_t* )
{
	char *p, **newf ;
	char *sepp; /* pointer to temporary separator character */
	int sepc;   /* temporary separator character */

	csv_t.nfield = 0 ;
	if( csv_t.line[0]=='\0' )
		return 0 ;
	strcpy( csv_t.sline, csv_t.line ) ;
	p = csv_t.sline ;

	do {
		if( csv_t.nfield>=maxfield ) {
			csv_t.maxfield *= 2;			/* double current size */
			newf = (char**) realloc( csv_t.field, 
							csv_t.maxfield * sizeof( csv_t.field[0] )) ;
			if( newf==NULL )
				return NOMEM ;
			csv_t.field = newf ;
		}
		if( *p=='"' )
			sepp = advquoted( ++p ) ;	/* skip initial quote */
		else
			sepp = p + strcspn( p, fieldsep ) ;
		sepc = sepp[0] ;
		sepp[0] = '\0' ;			/* terminate field */
		csv_t.field[csv_t.nfield++] = p ;
		p = sepp + 1 ;
	} while( sepc==',' ) ;

	return csv_t.nfield ;
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
char* csvfield( int n, csv_t* )
{
	if( n<0 || n>=nfield )
		return NULL ;
	return csv_t.field[n] ;
}

/* csvnfield:  return number of fields */ 
int csvnfield( csv_t* )
{
	return csv_t.nfield ;
}

/* csvtest main: test CSV library */
int main( void )
{
	int i ;
	char *line ;

	while( (line=csvgetline(stdin)) != NULL ) {
		printf( "line = %s\n", line ) ;
		for( i=0; i<csvnfield(); i++ )
			printf( "field[%d] = %s\n", i, csvfield( i )) ;
		printf( "\n" ) ;
	}
	return 0 ;
}

