// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <color.h>
#include <commands.h>
#include <iolib.h>
#include <shell.h>
#include <stddef.h>
#include <stdint.h>
#include <stringutils.h>
#include <time.h>

#define WELCOME_MESSAGE "Welcome to Clifford OS. Today is %d/%d/%d.\n"

#define CURSOR_COLOR CYAN
#define SCREEN_COLOR BLACK

#define EOF -1

#define USER_COLOR 0x0000AFFF
#define USER_SEPARATOR_COLOR 0x005FD700

#define TAB_SIZE 4
#define ESC 27

#define DEFAULT_SCALE 1

static void printUser();
static void getInputAndPrint(char *input);
static void printCursor(uint32_t hexColor, size_t offsetX);

static char *username;

void shell() {
	clearScreen();
	setFontScale(DEFAULT_SCALE);
	printf(WELCOME_MESSAGE, getDay(), getMonth(), getYear());
	char input[MAX_COMMAND_SIZE];
	char copy[MAX_COMMAND_SIZE];
	int returned = OK;

	if (username == NULL) {
		setUsername("user");
	}

	while (returned != EXIT) {
		if (noScreenSpace()) {
			parseCommand("clear");
		}
		printUser();
		getInputAndPrint(input);
		if (input[0] != 0) {
			putchar('\n');
			strcpy(copy, input);
			returned = parseCommand(copy);
			if (noScreenSpace()) {
				parseCommand("clear");
				// Execute the command again
				parseCommand(input);
			}
		}
		else {
			if (noScreenSpace())
				parseCommand("clear");
			else
				putchar('\n');
		}
	}
}

static void getInputAndPrint(char *input) {
	char c;
	int i = 0;
	int count = 0;
	printCursor(CURSOR_COLOR, 0);
	while ((c = getchar()) != '\n') {
		if (c == EOF) {
			continue;
		}
		if (c == '\t') {
			for (int j = 0; j < TAB_SIZE; j++) {
				putchar(' ');
				count++;
				if (i < (MAX_COMMAND_SIZE - 1))
					input[i++] = ' ';
			}
			printCursor(CURSOR_COLOR, 0);
		}
		else if (c == '\b') {
			if (count > 0) {
				if (count < MAX_COMMAND_SIZE)
					i--;
				putchar(c);
				count--;
				// we need to erase the cursor
				printCursor(SCREEN_COLOR, getFontWidth());
				printCursor(CURSOR_COLOR, 0);
			}
		}
		else if (c != ESC) {
			printCursor(CURSOR_COLOR, getFontWidth());
			if (i < (MAX_COMMAND_SIZE - 1))
				input[i++] = c;
			putchar(c);
			count++;
		}
	}
	printCursor(SCREEN_COLOR, 0);
	input[i] = 0;
}

void setUsername(char *user) {
	username = user;
}

static void printUser() {
	printStringColor(username, USER_COLOR);
	printStringColor("@os:", USER_COLOR);
	printStringColor("$ ", USER_SEPARATOR_COLOR);
}

static void printCursor(uint32_t hexColor, size_t offsetX) {
	uint32_t x = getCursorX() + offsetX;
	uint32_t y = getCursorY();
	if (x + getFontWidth() > getScreenWidth()) {
		x = 0;
		y += getFontHeight();
	}
	drawRectangle(hexColor, x, y, getFontWidth(), getFontHeight());
}

int noScreenSpace() {
	return (getCursorY() + getFontHeight()) > getScreenHeight();
}