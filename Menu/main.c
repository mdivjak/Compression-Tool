#include "menuHeader.h"

void startTUI() {
	char beginPath[_MAX_DIR + 1];
	char **paths = NULL;
	struct FilePaths fp;

	fp.capacity = fp.size = 0;
	strcpy(fp.archive, "");
	fp.paths = NULL;
	FILE *out;
	getcwd(beginPath);

	initscr();
	noecho();	//do not print user input
	cbreak();	//no enter needed to read
	curs_set(0); //turn off the cursor
	keypad(stdscr, TRUE); //enable function keys
	initColorScheme();	//init colors for header, footer and body
	bkgd(COLOR_PAIR(2));
	resize_term(30, 120);
	clear();
	refresh();

	WINDOW *header, *footer, *body;
	header = newwin(HEADER_HEIGHT, COLS, 0, 0);
	footer = newwin(FOOTER_HEIGHT, COLS, LINES - FOOTER_HEIGHT, 0);
	body = newwin(LINES - HEADER_HEIGHT - FOOTER_HEIGHT, COLS, HEADER_HEIGHT, 0);
	keypad(header, TRUE);
	keypad(body, TRUE);
	keypad(footer, TRUE);
	// init default backgrounds
	wbkgd(header, COLOR_PAIR(1));
	wbkgd(footer, COLOR_PAIR(1));
	wbkgd(body, COLOR_PAIR(2));
	
	writeHeaderMenu(header, 0, title);
	writeFooterMenu(footer, "Use arrow keys to navigate through options");
	clear(body);

	int ch, highlight = 0;
	while (1) {

		ch = wgetch(header);
		switch (ch) {

		case KEY_LEFT:
			if (--highlight < 0) highlight = MENU_OPTIONS - 1;
			writeHeaderMenu(header, highlight, title);
			break;

		case KEY_RIGHT:
			if (++highlight == MENU_OPTIONS) highlight = 0;
			writeHeaderMenu(header, highlight, title);
			break;

		case KEY_ENTER:case KEY_RENTER:
			if (highlight == MENU_OPTIONS - 1) exit(0);
			else {

				int algo = -1;
				if (highlight == 0) {
					algo = selectAlgorithm();
					if (algo == -1) break;
				}

				int log = prompt("Maintain a log file?");
				clear(body);
				wrefresh(body);

				writeFooterMenu(footer, "Press Enter to mark / unmark files, Press Q to finish selection");
				fp = fileSelection(body, highlight, footer);
				//return to program's folder path
				chdir(beginPath);
				if (fp.size) {

					//CALL FUNCTIONS FOR COMPRESSION / DECOMPRESSION
					WINDOW *info = newwin(INFO_HEIGHT, INFO_WIDTH, (LINES - INFO_HEIGHT) / 2, (COLS - INFO_WIDTH) / 2);
					if (highlight == 0) {
						infoMessage(info, "Compressing...");
						callCompression(fp, log, algo);
					}
					else if (highlight == 1) {
						infoMessage(info, "Decompressing...");
						callDecompression(fp, log);
					}
					destroyWin(info, COLOR_PAIR(2));
					wclear(body);
					wrefresh(body);

					// delete paths
					deleteFilePaths(&fp);
				}
				else {
					wclear(body);
					wrefresh(body);
				}
				if(highlight) writeFooterMenu(footer,"Decompression Successful!");
				else writeFooterMenu(footer, "Compression Successful!");
			}
			break;
		default:
			writeFooterMenu(footer, "Use arrow keys to navigate through options");
			break;
		}
	}
}

void startCMD(int argc, char *argv[]) {
	// if there are 2 arguments -help or -about could have been called
	if (argc == 2) {
		if (strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "-h") == 0)
			writeHelp(argv[0]);
		else if (strcmp(argv[1], "-a") == 0)
			writeAbout();
		else {
			writeError();
		}
	}
	else {
		// if program was called for compression / decompression
		int log = 0, i = 2;
		struct FilePaths fp;
		
		fp.capacity = 0;
		fp.size = 0;
		fp.paths = NULL;

		// program was called for compression
		if (strcmp(argv[1], "-c") == 0) {
			int algorithm = -1;
			if (strcmp(argv[2], "-lzw") == 0) {
				algorithm = LZW;
			}
			else if (strcmp(argv[2], "-dhuff") == 0) {
				algorithm = DHUFF;
			}
			else if (strcmp(argv[2], "-shuff") == 0) {
				algorithm = SHUFF;
			}
			else if (strcmp(argv[2], "-def") == 0) {
				algorithm = DEFLATE;
			}
			else { 
				writeError();
				return;
			}

			// if only arguments are -c and algorithm type
			if (argc == 3) {
				writeError();
				return;
			}

			if (strcmp(argv[3], "-l") == 0) log = 1;

			// if only arguments are -c, algorithm and -l
			if (log && argc == 4) {
				writeError();
				return;
			}

			// i is index of first path
			if (log) i = 4;
			else i = 3;

			// last argument for compression is the name of the archive
			// so we pass argc - 1 instead of argc
			addPathsCMD(argc-1, argv, &fp, i, 0);
			strcpy(fp.archive, argv[argc - 1]);

			if (fp.size > 0) {
				callCompression(fp, log, algorithm);
			}
		}
		else if (strcmp(argv[1], "-d") == 0) {

			if (strcmp(argv[2], "-l") == 0) log = 1;

			// if only commands are -d -l
			if (log && argc == 3) {
				writeError();
				return;
			}

			// i is index of first path
			if (log) i = 3;
			else i = 2;

			addPathsCMD(argc, argv, &fp, i, 1);

			if (fp.size > 0) {
				callDecompression(fp, log);
			}
		}
		else {
			writeError();
			return;
		}
	}
}

int main(int argc, char *argv[]) {
	if (argc == 1) {
		startTUI();
	}
	else {
		startCMD(argc, argv);
	}
	return 0;
}
