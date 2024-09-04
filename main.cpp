#include <iostream>
#include <cstring>

const char* kDelimiterLongArgument = "--delimiter=";
const char* kDelimiterShortArgument = "-d";
const char* kLinesLongArgument = "--lines=";
const char* kLinesShortArgument = "-l";
const char* kTailLongArgument = "--tail";
const char* kTailShortArgument = "-t";
const char *TypesErrorStr[] = {"", "[ERROR] Wasn't given argument for -d\n", "[ERROR] Wasn't given argument for -l\n",
	"[ERROR] Syntax: PrintFile [OPTION] filename\n"};

enum class TypesError {ErrorNone, ErrorArgumentDelimiter, ErrorArgumentLines, ErrorSyntax};

struct ArgumentsForPrintFile {
	bool is_tail = false;
	long long lines_output = 0;
	char delimiter = '\n';
	char* filename = NULL;
	TypesError error = TypesError::ErrorNone;
};

ArgumentsForPrintFile arguments;

void GetArgumentDelimiter(char* delimiter) {
	if (delimiter[0] == '\\' && delimiter[1] != '\0' && delimiter[2] == '\0') {
		switch (delimiter[1]) {
			case '0':
				arguments.delimiter = '\0';
				break;
			case 'a':
				arguments.delimiter = '\a';
				break;
			case 'r':
				arguments.delimiter = '\r';
				break;
			case 'b':
				arguments.delimiter = '\b';
				break;
			case 't':
				arguments.delimiter = '\t';
				break;
			case 'f':
				arguments.delimiter = '\f';
				break;
		}
	} else if (delimiter[1] == '\0') {
		arguments.delimiter = delimiter[0];
	} else {
		arguments.error = TypesError::ErrorArgumentDelimiter;
	}
}

size_t GetLinesCount(char* filename, char delimiter) {
	long long lenght = 0;
	char c;
	FILE* file = fopen(filename, "r");
	if (!file) {
		std::cout << "[ERROR] File not found " << arguments.filename << '\n';
		return lenght;
	}
	while ((c = getc(file)) != EOF) {
		if (c == delimiter) {
			++lenght;
		}
	}
	fclose(file);
	return lenght;
}

bool IsNumberLineValid(bool is_tail, long long lines_count, long long lines_total, long long lines_output) {
	return (!is_tail && lines_count + 1 <= lines_output) || (is_tail && lines_count >= lines_total - lines_output);
}

void PrintFile() {
	FILE* file = fopen(arguments.filename, "r");
	if (!file) {
		std::cout << "[ERROR] File not found " << arguments.filename << '\n';
		return;
	}
	char c;
	long long lines_count = 0;
	long long lines_total = GetLinesCount(arguments.filename, arguments.delimiter);
	if (arguments.lines_output == 0) {
		arguments.lines_output = lines_total;
	}
	while ((c = getc(file)) != EOF) {
		if (IsNumberLineValid(arguments.is_tail, lines_count, lines_total, arguments.lines_output)) {
			putchar(c);
		}
		if (c == arguments.delimiter) {
			lines_count++;
		}
	}
	if (ferror(file)) {
		std::cerr << "[ERROR] while reading the file\n";
	}
	fclose(file);
 }

void ParsArgument(int argc, char** argv) {
	if (argc < 2) {
		arguments.error = TypesError::ErrorSyntax;
	}
	for (int i = 1; i < argc; i++) {
		if (!std::strcmp(kLinesShortArgument, argv[i])) {
			if (i + 1 < argc) {
				arguments.lines_output = std::atoi(argv[i + 1]);
			}
			if (i + 1 >= argc || (arguments.lines_output == 0 && std::strcmp(argv[i], "0"))) {
				arguments.error = TypesError::ErrorArgumentLines;
			}
		} else if (!std::strcmp(kTailShortArgument, argv[i]) || !std::strcmp(kTailLongArgument, argv[i])) {
			arguments.is_tail = true;
		} else if (!std::strcmp(kDelimiterShortArgument, argv[i])) {
			if (i + 1 < argc) {
				GetArgumentDelimiter(argv[i + 1]);
			} else {
				arguments.error = TypesError::ErrorArgumentDelimiter;
			}
		} else if (!std::strncmp(kLinesLongArgument, argv[i], std::strlen(kLinesLongArgument))) {
			arguments.lines_output = std::atoi(argv[i] + std::strlen(kLinesLongArgument));
			if (arguments.lines_output == 0 && std::strcmp(argv[i], "0")) {
				arguments.error = TypesError::ErrorArgumentLines;
			}
		} else if (!std::strncmp(kDelimiterLongArgument, argv[i], std::strlen(kDelimiterLongArgument))) {
			GetArgumentDelimiter(argv[i] + std::strlen(kDelimiterLongArgument));
		} else {
			arguments.filename = argv[i];
		}
	}
}

int main(int argc, char** argv) {
	ParsArgument(argc, argv);
	if (arguments.error != TypesError::ErrorNone) {
		std::cerr << TypesErrorStr[static_cast<int>(arguments.error)];
	} else {
		PrintFile();
	}
	return EXIT_SUCCESS;
}
