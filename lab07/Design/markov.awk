#!/usr/bin/awk -f
# markov.awk: markov chain algorithm for 2-word prefixes
#
# From Kernighan & Pike, _Practice of Programming_.
#
# EDITOR:  tabstop=2, cols=120
#

BEGIN { MAXGEN = 10000 ; NONWORD = "\n" ; wl = w2 = NONWORD }

{	 # read all words
	for( i=1; i<=NF; ++i )
	{
		statetab[ wl, w2, ++nsuffix[wl, w2] ] = $i
		wl = w2
		w2 = $i
	}
}

END {  # generate
	statetab[ wl, w2 , ++suffix[wl, w2] ] = NONWORD   # add tai1
	wl = w2 = NONWORD
	for( i=0; i<MAXGEN; ++i )
	{
		r = int( rand() * nsuffix[wl, w2] ) + 1   # nsuffix >= 1
		p = statetab[ wl, w2, r ]
		if( p == NONWORD ) exit
		print p

		wl = w2   # advance chain
		w2 = p
	}
}
