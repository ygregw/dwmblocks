//Modify this file to change what commands output to your statusbar, and recompile using the make command.
static const Block upperblocks[] = {
   /*Icon*/   /*Command*/     /*Update-interval*/   /*Update-signal*/   /*Fixed-length*/
	{"",       "sb-battery",   1,                    1,						-16},
	{"",       "sb-volume",		1800,						 2,						-14},
   {"",       "sb-clock",     30,						 3,                   16},
};

static const Block lowerblocks[] = {
   {"",   "sb-internet",      3,   4,   -13},
   {"",   "sb-nettraf",       1,   5,   -21},
   {"",   "sb-cpu",				2,   6,   -26},
   {"",   "sb-memory",			2,   7,   -22},
   {"",   "sb-disk",				5,   8,   -19},
	{"",   "sb-wttr",			3600,	  9,   -30},
};

//sets delimiter between status commands. NULL character ('\0') means no delimiter.
static char delim[] = " | ";
