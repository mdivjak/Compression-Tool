#include "menuHeader.h"

void infoMessage(WINDOW *info, char *messg) {

	keypad(info, TRUE);
	wbkgd(info, COLOR_PAIR(1));

	wattrset(info, COLOR_PAIR(1));
	wclear(info);
	box(info, 0, 0);

	mvwprintw(info, INFO_HEIGHT / 2, (INFO_WIDTH - strlen(messg)) / 2, "%s", messg);
	wrefresh(info);
}

void promptAlgorithms(WINDOW *algo, int highlight) {
	char *opt[] = { "Dynamic Huffman", "Static Huffman", "LZW", "Deflate Algorithm", "Quit" };

	for (int i = 0; i < 5; i++) {
		wattrset(algo, COLOR_PAIR(1));
		if (highlight == i) wattrset(algo, COLOR_PAIR(3));
		mvwprintw(algo, 1 + i, 2, "%s", opt[i]);
	}

	wrefresh(algo);
}

int selectAlgorithm() {
	WINDOW *promptAlgo = newwin(SELECT_HEIGHT, SELECT_WIDTH, (LINES - SELECT_HEIGHT) / 2, (COLS - SELECT_WIDTH) / 2);
	keypad(promptAlgo, TRUE);
	wbkgd(promptAlgo, COLOR_PAIR(1));

	wattrset(promptAlgo, COLOR_PAIR(1));
	wclear(promptAlgo);
	box(promptAlgo, 0, 0);

	int ch, highlight = 0;
	promptAlgorithms(promptAlgo, highlight);

	while (1) {
		ch = wgetch(promptAlgo);
		switch (ch) {
		case KEY_UP:
			if (--highlight < 0) highlight = 4;
			promptAlgorithms(promptAlgo, highlight);
			break;
		case KEY_DOWN:
			if (++highlight > 4) highlight = 0;
			promptAlgorithms(promptAlgo, highlight);
			break;
		case KEY_ENTER: case KEY_RENTER:
			destroyWin(promptAlgo, COLOR_PAIR(2));
			// if Quit was chosen return -1
			if (highlight == 4) return -1;
			else return highlight;
		}
	}
}

void writeHeaderMenu(WINDOW *header, int highlight, char *title) {
	int n = sizeof(choices) / sizeof(char*);
	if (highlight >= n) highlight = 0;

	wclear(header);
	wattrset(header, COLOR_PAIR(4));	// title color
	mvwprintw(header, 0, 0, "\t%s", title);

	wattrset(header, COLOR_PAIR(1));	// header color
	wmove(header, 1, 0);
	for (int i = 0; i < n; i++) {
		if (i == highlight) {
			wattrset(header, COLOR_PAIR(3));
			wprintw(header, " %s ", choices[i]);
			wattrset(header, COLOR_PAIR(1));
		}
		else wprintw(header, " %s ", choices[i]);
	}

	wrefresh(header);
}

void writeFooterMenu(WINDOW *footer, char *message) {
	char cpy[] = " Copyright 2018 - Marko Divjak, Uros Isakovic, Dusan Vojinovic";
	
	wattrset(footer, COLOR_PAIR(1));	//footer color
	wclear(footer);

	mvwprintw(footer, 0, 0, "\t%s", message);
	//169 - code for copyright symbol
	mvwprintw(footer, FOOTER_HEIGHT - 1, COLS - strlen(cpy) - 1, " Copyright 2018 - Marko Divjak, Uros Isakovic, Dusan Vojinovic");

	wrefresh(footer);
}

void initColorScheme() {

	if (has_colors() == FALSE) {
		endwin();
		printf("Your terminal does not support color\n");
		getchar();
		exit(NO_COLOR);
	}

	start_color();
	//CYAN - prljava zelena/plava
	init_color(COLOR_BLACK, 0, 0, 0);	// make BLACK darker
	init_color(COLOR_WHITE, 1000, 1000, 1000);	// make WHITE lighter
	init_color(COLOR_GREEN, 0, 1000, 0);	// make GREEN more visible
	init_color(COLOR_RED, 1000, 0, 0);	// make RED more visible
	init_color(COLOR_YELLOW, 1000, 1000, 0);	// make YELLOW lighter
	init_pair(1, COLOR_WHITE, COLOR_CYAN);	// for header and footer and prompt
	init_pair(2, COLOR_WHITE, COLOR_BLUE);	// for body
	init_pair(3, COLOR_WHITE, COLOR_BLACK);	// for highlighted option
	init_pair(4, COLOR_BLACK, COLOR_CYAN);	// for programs title
	init_pair(5, COLOR_YELLOW, COLOR_BLUE);	// for directories
	init_pair(6, COLOR_GREEN, COLOR_BLUE);	// for files
	init_pair(7, COLOR_RED, COLOR_BLUE);	// for selected listing

}

void promptText(WINDOW *prompt, int highlight, char *mesg) {

	wattrset(prompt, COLOR_PAIR(1));
	mvwprintw(prompt, 2, (prompt->_maxx - strlen(mesg)) / 2, "%s", mesg);
	//mvwprintw(prompt, 2, 1, "   file during the process?");
	if (highlight == 0) wattrset(prompt, COLOR_PAIR(3));	//if option is selected change color
	mvwprintw(prompt, PROMPT_HEIGHT - 2, 7, "[YES]");
	wattrset(prompt, COLOR_PAIR(1));
	if (highlight == 1) wattrset(prompt, COLOR_PAIR(3));
	mvwprintw(prompt, PROMPT_HEIGHT - 2, PROMPT_WIDTH - 12, "[NO]");
	wattrset(prompt, COLOR_PAIR(1));

	wrefresh(prompt);
}

void destroyWin(WINDOW *win, chtype scheme) {
	wbkgd(win, scheme);
	wclear(win);
	wrefresh(win);
	delwin(win);
}

int prompt(char *mesg) {
	WINDOW *promptLog = newwin(PROMPT_HEIGHT, PROMPT_WIDTH, (LINES - PROMPT_HEIGHT) / 2, (COLS - PROMPT_WIDTH) / 2);
	keypad(promptLog, TRUE);
	wbkgd(promptLog, COLOR_PAIR(1));
	
	wattrset(promptLog, COLOR_PAIR(1));	
	wclear(promptLog);
	box(promptLog, 0, 0);

	//default prompt value is NO => highlight = 1
	int ch, highlight = 1;
	promptText(promptLog, highlight, mesg);

	while (1) {

		ch = wgetch(promptLog);
		switch (ch) {

		case KEY_LEFT:
			if (--highlight < 0) highlight = 1;
			promptText(promptLog, highlight, mesg);
			break;

		case KEY_RIGHT:
			if (++highlight > 1) highlight = 0;
			promptText(promptLog, highlight, mesg);
			break;

		case KEY_ENTER:case KEY_RENTER:
			// paint the prompt window to have BODY color
			destroyWin(promptLog, COLOR_PAIR(2));
			// if highlight is true NO was selected so return 0
			return (highlight) ? 0 : 1;
		}
	}
}

void archiveName(struct FilePaths *fp, WINDOW *footer) {
	char cpy[] = " Copyright 2018 - Marko Divjak, Uros Isakovic, Dusan Vojinovic";

	wattrset(footer, COLOR_PAIR(1));	//footer color
	wclear(footer);
	mvwprintw(footer, FOOTER_HEIGHT - 1, COLS - strlen(cpy) - 1, "%s", cpy);
	mvwprintw(footer, 0, 0, "\tInput archive name: ");
	wrefresh(footer);
	nocbreak();
	echo();

	wscanw(footer, "%s", fp->archive);
	cbreak();
	noecho();
}
