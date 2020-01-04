#
# squash.awk
#		given 2 columns, puts all 2nd cols on the same line, if first column is the same, and records are adjacent
#
# EDITOR:  tabstop=2, cols=120
#

$1 != prev {
	prev = $1
	if( NR > 1 )
		printf "\n"
}

{ printf "%s ", $2 }

END {
	printf "\n"
}

