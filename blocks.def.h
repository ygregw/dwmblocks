//Modify this file to change what commands output to your statusbar, and recompile using the make command.
static const Block upperblocks[] = {
   /*Icon*/   /*Command*/     /*Update-interval*/   /*Update-signal*/   /*Fixed-length*/
	{"",       "sb-battery",   1,                    1,						-14},
   {"",       "sb-clock",     1,                    2,                  18},
};

static const Block lowerblocks[] = {
   {"",   "sb-cpu",        1,   3,   -24},
   {"",   "sb-memory",     1,   4,   -22},
   {"",   "sb-disk",       1,   5,   -20},
};

//sets delimiter between status commands. NULL character ('\0') means no delimiter.
static char delim[] = " | ";
