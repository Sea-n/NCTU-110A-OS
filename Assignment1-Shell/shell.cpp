#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <cstring>

using namespace std;

/*
 * Student ID: 0816146
 * Student Name: Sean Wei
 * Email: nctu-os@sean.taipei
 * SE tag: xnxcxtxuxoxsx
 * Statement: I am fully aware that this program is not supposed to be posted
 * to a public server, such as a public GitHub repository or a public web page.
 */

int main() {
	for (;!cin.eof();) {
		string line, word;
		stringstream ss;
		char** cmds[64];
		int pipe_fd[2];
		FILE* fout;

		cout << "> ";
		getline(cin, line);
		ss << line;
		int p = 0;

		do {
			cmds[p] = new char*[64];
			int q = 0;
			while (ss >> word) {
				if (word == "&" || word == ">" || word == ">>" || word == "|") {
					break;
				}
				cmds[p][q] = new char[256];
				strcpy(cmds[p][q++], word.c_str());
			}
			cmds[p][q] = NULL;
			p++;
		} while (word == "|");
		cmds[p] = NULL;

		const char* mode = (word == ">") ? "w" : (word == ">>") ? "a" : nullptr;
		if (word == ">") {
			mode = (word == ">") ? "w" : "a";
		}
		if (mode != nullptr && !ss.eof()) {
			ss >> word;
			fout = fopen(word.c_str(), mode);
		}

		pid_t pid = fork();

		if (pid < 0) { /* error occurred */
			fprintf(stderr, "Fork Failed");
			exit(-1);
		} else if (pid == 0) { /* child process */
			p = 0;
			int stdin_pipe = 0;
			do {
				pipe(pipe_fd);
				pid_t pid2 = fork();

				if (pid2 < 0) {
					fprintf(stderr, "Fork Failed");
					exit(-1);
				} else if (pid2 == 0) {
					dup2(stdin_pipe, STDIN_FILENO);
					close(pipe_fd[0]);

					/* piping, to file, stdout */
					if (cmds[p+1] != NULL) {
						dup2(pipe_fd[1], STDOUT_FILENO);
						execvp(cmds[p][0], cmds[p]);
					} else if (fout != nullptr && fout != NULL) {
						dup2(fileno(fout), STDOUT_FILENO);
						execvp(cmds[0][0], cmds[0]);
					} else {
						execvp(cmds[p][0], cmds[p]);
					}
					cout << flush;
				} else {
					wait(NULL);
					close(pipe_fd[1]);
					stdin_pipe = pipe_fd[0];
					p++;
				}
			} while (cmds[p] != NULL);
		} else {
			if (word != "&") {
				wait(NULL);
			}
		}
	}

	wait(NULL);
}
