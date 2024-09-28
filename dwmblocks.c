#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
/* #define NO_X */
#ifndef NO_X
#include <stdlib.h>
#include <X11/Xlib.h>
#endif
#define   SIGPLUS        SIGRTMIN
#define   SIGMINUS       SIGRTMIN
#define   LENGTH(X)      (sizeof(X)/sizeof(X[0]))
#define   CMDLENGTH      50 //max module output len
#define   MIN(a,b)       ((a<b)?a:b)
#define   ABS(c)			 ((unsigned)((c<0)?-c:c))
#define   STATUSLENGTH   ((LENGTH(upperblocks)+LENGTH(lowerblocks))*CMDLENGTH+1)

typedef struct {
	char* icon; //could be plaintext
	char* command;
	unsigned int interval;
	unsigned int signal;
	int fixedlen;
} Block;

void sighandler(int num);
void getcmds(int time);
void getsigcmds(unsigned int signal);
void setupsignals();
void sighandler(int signum);
int getstatus(char *str, char *last);
void statusloop();
void termhandler();
void pstdout();
void setroot();
#ifndef NO_X
static void (*writestatus) () = setroot;
static int setupX();
static Display *dpy;
static int screen;
static Window root;
#else
static void (*writestatus) () = pstdout;
#endif

#include "blocks.h"

static Block blocks[LENGTH(upperblocks)+LENGTH(lowerblocks)] = {0};
static char statusbar[LENGTH(upperblocks)+LENGTH(lowerblocks)][CMDLENGTH] = {0};
static char statusstr[2][STATUSLENGTH];
static unsigned int jump = LENGTH(upperblocks)-1;
static int statusContinue = 1;
static int fixedLength = 0;
static char jumpdelim[] = ";";

//opens process *cmd and stores output in *output
void getcmd(const Block *block, char *output, unsigned int pos)
{
	//make sure status is same until output is ready
	char tempstatus[CMDLENGTH] = {0};
	strcat(tempstatus, block->icon);
	FILE *cmdf = popen(block->command, "r");
	if (!cmdf)
		return;
	int i = strlen(block->icon);
	int delimlen = pos == jump ? 1 : strlen(delim);
	fgets(tempstatus+i, CMDLENGTH-i-delimlen, cmdf);
	i = strlen(tempstatus);
	//unless both icon and command ouput are empty
	if (i != 0) {
		//only chop off newline if one is present at the end
		i = tempstatus[i-1] == '\n' ? i-1 : i;
		if (delim[0] != '\0' && pos != jump) {
			strncpy(tempstatus+i, delim, delimlen);
		}
		else
			tempstatus[i++] = '\0';
	}
	strcpy(output, tempstatus);
	pclose(cmdf);
}

void getcmds(int time)
{
	const Block* current;
	for (unsigned int i = 0; i < LENGTH(blocks); i++) {
		current = blocks + i;
		if ((current->interval != 0 && time % current->interval == 0) || time == -1)
			getcmd(current,statusbar[i],i);
	}
}

void getsigcmds(unsigned int signal)
{
	const Block *current;
	for (unsigned int i = 0; i < LENGTH(blocks); i++) {
		current = blocks + i;
		if (current->signal == signal)
			getcmd(current,statusbar[i],i);
	}
}

void setupsignals()
{
	for (unsigned int i = 0; i < LENGTH(blocks); i++) {
		if (blocks[i].signal > 0)
			signal(SIGMINUS+blocks[i].signal, sighandler);
	}
}

int getstatus(char *str, char *last)
{
	strcpy(last, str);
	str[0] = ' ';
	str[1] = '\0';
	if (fixedLength) {
		const Block* current;
		for (unsigned int i = 0; i < LENGTH(blocks); i++) {
			current = blocks + i;
			unsigned int currentl = strlen(statusbar[i]);
			unsigned int fixedl = ABS(current->fixedlen);
			unsigned int dif = fixedl > currentl ? (fixedl-currentl) : (CMDLENGTH-currentl);
			if (current->fixedlen < 0) {
				strcat(str, "[");
				strcat(str, statusbar[i]);
				strcat(str, "]");
				for (unsigned int j = 0; j < dif; j++)
					strcat(str, " ");
			} else {
				for (unsigned int j = 0; j < dif; j++)
					strcat(str, " ");
				strcat(str, "[");
				strcat(str, statusbar[i]);
				strcat(str, "]");
			}
			if (i == jump) {
				strcat(str, jumpdelim);
			}
		}
	} else {
		for (unsigned int i = 0; i < LENGTH(blocks); i++) {
			strcat(str, statusbar[i]);
		}
	}
	str[strlen(str)-strlen(delim)] = '\0';
	return strcmp(str, last);//0 if they are the same
}

#ifndef NO_X
void setroot()
{
	if (!getstatus(statusstr[0], statusstr[1]))//only set root if text has changed
		return;
	XStoreName(dpy, root, statusstr[0]);
	XFlush(dpy);
}

int setupX()
{
	dpy = XOpenDisplay(NULL);
	if (!dpy) {
		fprintf(stderr, "dwmblocks: Failed to open display\n");
		return 0;
	}
	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);
	return 1;
}
#endif

void pstdout()
{
	if (!getstatus(statusstr[0], statusstr[1]))//only write when text has changed
		return;
	printf("%s\n",statusstr[0]);
	fflush(stdout);
}


void statusloop()
{
	setupsignals();
	int i = 0;
	getcmds(-1);
	while (1) {
		getcmds(i++);
		writestatus();
		if (!statusContinue)
			break;
		sleep(1.0);
	}
}

void sighandler(int signum)
{
	getsigcmds(signum-SIGPLUS);
	writestatus();
}

void termhandler()
{
	statusContinue = 0;
}

int main(int argc, char** argv)
{
	for (int i = 0; i < argc; i++) {//toggle fixed length blocks
		if (!strcmp("-f",argv[i])) {
			fixedLength = 1;
			delim[0] = '\0';
		}
	}

	for (unsigned int i = 0; i < LENGTH(upperblocks); i++)
		blocks[i] = upperblocks[i];
	for (unsigned int i = 0; i < LENGTH(lowerblocks); i++)
		blocks[LENGTH(upperblocks)+i] = lowerblocks[i];

#ifndef NO_X
	if (!setupX())
		return 1;
#endif
	signal(SIGTERM, termhandler);
	signal(SIGINT, termhandler);
	statusloop();
#ifndef NO_X
	XCloseDisplay(dpy);
#endif
	return 0;
}
