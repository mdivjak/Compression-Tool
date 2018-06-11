#include "menuHeader.h"

int isReadable(char *path) {
	FILE *in = fopen(path, "rb");
	if (in == NULL) {
		return 0;
	}
	fclose(in);
	return 1;
}

int isArchive(char *arch) {
	if (strlen(arch) < strlen(extension)) return 0;
	int ind = strlen(arch) - strlen(extension);
	if (strcmp(arch + ind, extension) == 0) return 1;
	return 0;
}

void listArchives(struct Listing *list, char *path) {
	DIR *dir = opendir(path);
	struct dirent *arch;

	if (dir != NULL) {
		while (arch = readdir(dir)) {
			if (isArchive(arch->d_name) || arch->d_type == DT_DIR) {
				addListing(list, arch);
			}
		}
		list->ls = (struct Choice **) realloc(list->ls, list->size * sizeof(struct Choice *));
		list->capacity = list->size;
		closedir(dir);
	}
}

void listFiles(struct Listing *list, char *path) {
	DIR *dir = opendir(path);
	struct dirent *file;

	if (dir != NULL) {
		while (file = readdir(dir)) {
			addListing(list, file);
		}
		list->ls = (struct Choice **) realloc(list->ls, list->size * sizeof(struct Choice *));
		list->capacity = list->size;
		closedir(dir);
	}
}

void addListing(struct Listing *list, struct dirent *file) {
	//if list is empty
	if (list->capacity == 0) {
		list->capacity += LISTING_INCREMENT;
		list->ls = (struct Choice **) calloc(list->capacity, sizeof(struct Choice *));
		assert(!(!list->ls && list->size));
	}	//if list is full
	else if (list->capacity == list->size) {
		list->capacity += LISTING_INCREMENT;
		list->ls = (struct Choice **) realloc(list->ls, list->capacity * sizeof(struct Choice *));
		assert(!(!list->ls && list->size));
	}

	//create new choice for display
	int ind = list->size;

	list->ls[ind] = (struct Choice *) malloc(sizeof(struct Choice));
	assert(!(!list->ls[ind] && list->size));

	list->ls[ind]->name = (char*)calloc(file->d_namlen + 1, sizeof(char));
	assert(!(!list->ls[ind]->name && list->size));

	strcpy(list->ls[ind]->name, file->d_name);

	list->ls[ind]->checked = 0;
	list->ls[ind]->type = (file->d_type == DT_DIR) ? T_DIRECTORY : ((file->d_type == DT_REG) ? T_FILE : T_UNDEF);

	list->size++;
}

void addFilePath(struct FilePaths *fp, char *path) {
	//if filepath struct is full or empty expand it
	if (fp->size == fp->capacity) {
		fp->capacity += FILEPATH_INCREMENT;
		fp->paths = (char**)realloc(fp->paths, fp->capacity * sizeof(char *));
		assert(!(!fp->paths && fp->capacity));
	}

	fp->paths[fp->size] = (char*)calloc(strlen(path) + 1, sizeof(char));
	assert(!(!fp->paths[fp->size] && strlen(path)));
	strcpy(fp->paths[fp->size], path);
	fp->size++;
}

int notIn(struct FilePaths fp, char *name) {
	for (int i = 0; i < fp.size; i++) {
		if (strcmp(fp.paths[i], name) == 0)
			return 0;
	}
	return 1;
}

void addChecked(struct Listing list, struct FilePaths *fp) {
	char currPath[_MAX_DIR + 1], tmpName[_MAX_DIR + 1];

	getcwd(currPath);
	strcat(currPath, "\\");

	for (int i = 0; i < list.size; i++) {
		strcpy(tmpName, "");
		strcpy(tmpName, currPath);
		strcat(tmpName, list.ls[i]->name);

		//if checked file is not in pathfile struct, then add it
		if (list.ls[i]->checked && notIn(*fp, tmpName)) {
			addFilePath(fp, tmpName);
		}
	}

	fp->paths = (char**)realloc(fp->paths, fp->size * sizeof(char*));
	fp->capacity = fp->size;
	assert(!(!fp->paths && fp->capacity));
}

void deleteUnchecked(struct Listing list, struct FilePaths *fp) {
	char currPath[_MAX_DIR + 1], tmpName[_MAX_DIR + 1];
	int del;

	getcwd(currPath);
	strcat(currPath, "\\");

	fp->capacity = 0;
	for (int i = 0; i < fp->size; i++) {
		del = 0;
		for (int j = 0; j < list.size; j++) {
			strcpy(tmpName, "");
			strcpy(tmpName, currPath);
			strcat(tmpName, list.ls[j]->name);

			//if current file is unchecked and is in filepaths struct, then delete it
			if (!list.ls[j]->checked && (strcmp(tmpName, fp->paths[i]) == 0)) {
				free(fp->paths[i]);
				fp->paths[i] = NULL;
				del = 1;
				break;
			}
		}

		if (!del) {
			fp->paths[fp->capacity++] = fp->paths[i];
		}
	}

	fp->size = fp->capacity;
	if (fp->size > 0)
		fp->paths = (char**)realloc(fp->paths, fp->size * sizeof(char*));
}

void markChecked(struct Listing *list, struct FilePaths fp) {
	char currPath[_MAX_DIR + 1], tmpName[_MAX_DIR + 1];

	getcwd(currPath);
	strcat(currPath, "\\");

	for (int i = 0; i < list->size; i++) {
		assert(!(strlen(currPath) + strlen(list->ls[i]->name) + 1 > _MAX_DIR));

		//create full path name for file [i] in listing
		strcpy(tmpName, ""); //just to be sure
		strcpy(tmpName, currPath);
		strcat(tmpName, list->ls[i]->name);

		if (!notIn(fp, tmpName)) {
			list->ls[i]->checked = 1;
		}
	}
}

int writeListing(WINDOW *body, struct Listing list, int highlight) {
	char name[_MAX_DIR];
	int i = 0;
	getcwd(name);	//where is this defined ???
	wclear(body);
	if (strlen(name) > 114) {
		name[113] = '\0';
		strcat(name, "...");
	}

	//for current directory
	wattrset(body, COLOR_PAIR(2));
	mvwprintw(body, 0, 0, "%s", name);

	for (i = list.start; i < list.size && body->_cury < body->_maxy-1; i++) {
		char fileName[_MAX_DIR + 1];
		strcpy(fileName, list.ls[i]->name);
		if (strlen(fileName) > 114) {
			fileName[113] = '\0';
			strcat(fileName, "...");
		}
		//selected listing
		if (i == highlight) wattrset(body, COLOR_PAIR(7));
		//directory color
		else if (list.ls[i]->type == T_DIRECTORY) wattrset(body, COLOR_PAIR(5));
		//files color
		else if (list.ls[i]->type == T_FILE) wattrset(body, COLOR_PAIR(6));
		//anything else
		else wattrset(body, COLOR_PAIR(2));

		mvwprintw(body, body->_cury + 1, 0, "%c %s", (list.ls[i]->checked) ? '+' : ' ', fileName);
	}

	wrefresh(body);
	return i-1;
}

void deleteListing(struct Listing *list) {
	list->capacity = 0;
	if (list->ls) {
		for (int i = 0; i < list->size; i++) {
			if (list->ls[i]) free(list->ls[i]->name);
			list->ls[i]->name = NULL;
			free(list->ls[i]);
			list->ls[i] = NULL;
		}
		free(list->ls);
	}
	list->ls = NULL;
	list->size = 0;
	list->start = 0;
	list->end = 0;
}

void printCheckedFiles(WINDOW *body, struct FilePaths fp) {
	wclear(body);
	wattrset(body, COLOR_PAIR(2));
	mvwprintw(body, 0, 0, "  You have selected the following files\n");

	for (int i = 0; i < fp.size && body->_cury < body->_maxy; i++) {
		char fileName[_MAX_DIR + 1];
		strcpy(fileName, fp.paths[i]);
		// if name is too long then add "..." at the end
		if (strlen(fileName) > 110) {
			fileName[109] = '\0';
			strcat(fileName, "...");
		}
		mvwprintw(body, body->_cury + 1, 0, "%3d) %s", i + 1, fileName);
	}

	wrefresh(body);
}

void deleteFilePaths(struct FilePaths *fp) {
	for (int i = 0; i < fp->size; i++) {
		if (fp->paths) {
			free(fp->paths[i]);
			fp->paths[i] = NULL;
		}
	}
	if(fp->paths) free(fp->paths);
	fp->paths = NULL;
	strcpy(fp->archive, "");
	fp->size = fp->capacity = 0;
}

struct FilePaths fileSelection(WINDOW *body, int ind, WINDOW *footer) {
	int ch = 0, highlight = 0;
	char pathName[_MAX_DIR + 2];
	struct Listing list;
	struct FilePaths fp;
	void(*makeListing[2])(struct Listing *list, char *path) = { listFiles, listArchives };

	fp.capacity = 0;
	fp.size = 0;
	fp.paths = NULL;

	list.capacity = 0;
	list.size = 0;
	list.start = 0;
	list.end = 0;
	list.ls = NULL;

	makeListing[ind](&list, "./");
	list.end = writeListing(body, list, highlight);

	while (1) {
		ch = wgetch(body);

		switch (ch) {
		case KEY_UP:
			if (highlight == 0) {
				highlight = list.size - 1;
				if (list.size % (body->_maxy - 1)) {
					list.start = list.size - (list.size % (body->_maxy - 1));
				}
				else {
					list.start = list.size - (body->_maxy - 1);
				}
				if (list.start < 0) list.start = 0;
			}
			else if (highlight == list.start) {
				list.start -= body->_maxy - 1;
				if (list.start < 0) list.start = 0;
				highlight--;
			}
			else highlight--;

			list.end = writeListing(body, list, highlight);
			break;

		case KEY_DOWN:
			if (highlight == list.size - 1) {
				highlight = 0;
				list.start = 0;
			}
			else if (highlight == list.end) {
				list.start = highlight + 1;
				highlight++;
			}
			else highlight++;

			list.end = writeListing(body, list, highlight);
			break;

		case KEY_ENTER:case KEY_RENTER:
			// if file was checked
			if (list.ls[highlight]->type == T_FILE) {
				list.ls[highlight]->checked = (list.ls[highlight]->checked) ? 0 : 1;
			}
			// if user entered a directory
			else if (list.ls[highlight]->type == T_DIRECTORY) {
				// delete any unchecked files and add any new checked files
				deleteUnchecked(list, &fp);
				addChecked(list, &fp);

				// move into selected directory
				getcwd(pathName);
				strcat(pathName, "\\");
				strcat(pathName, list.ls[highlight]->name);
				chdir(pathName);

				// create a new listing of files
				deleteListing(&list);
				makeListing[ind](&list, "./");
				markChecked(&list, fp);

				// cursor set to first file
				highlight = 0;
			}

			list.end = writeListing(body, list, highlight);
			break;

		case 'q':case 'Q':
			// delete any unchecked files and add any new checked files
			deleteUnchecked(list, &fp);
			addChecked(list, &fp);

			deleteListing(&list);

			// do not include files that cannot be read
			fp.capacity = fp.size; fp.size = 0;
			for (int i = 0; i < fp.capacity; i++) {
				if (isReadable(fp.paths[i])) {
					fp.paths[fp.size++] = fp.paths[i];
				}
				else free(fp.paths[i]);
			}
			fp.paths = (char**)realloc(fp.paths, fp.size * sizeof(char*));
			fp.capacity = fp.size;

			if (fp.size == 0) {
				deleteFilePaths(&fp);
				return fp;
			}

			printCheckedFiles(body, fp);

			if (!ind) {
				strcpy(fp.archive, "");
				archiveName(&fp, footer);

				if (strlen(fp.archive) == 0) {
					deleteFilePaths(&fp);
					return fp;
				}
			}

			int answer = prompt("Are you sure?");

			// if user confirmed selected files and
			// number of selected files is > 0
			if (answer && fp.size > 0) {
				return fp;
			}
			else {
				deleteFilePaths(&fp);
				return fp;
			}
			break;
		}
	}
}
