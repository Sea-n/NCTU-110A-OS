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
	while (!cin.eof()) {
		int stdout_fd;
		char **cmds[1024];
		string line, word;
		stringstream ss;
		int pipe_fd[2];
		FILE *fpin[1024], *fpout[1024];
		int p, q;

		/* Prompt */
		cout << "> ";
		getline(cin, line);
		ss << line;


		/* Parse command */
		p = 0;
		do {
			fpin[p] = fpout[p] = nullptr;
			cmds[p] = new char*[1024];
			q = 0;
			while (ss >> word) {
				if (word == "|") break;
				if (word == "&") break;

				if (word == ">") {
					ss >> word;
					fpout[p] = fopen(word.c_str(), "w");
					break;
				}

				if (word == ">>") {
					ss >> word;
					fpout[p] = fopen(word.c_str(), "a");
					break;
				}

				if (word == "<") {
					ss >> word;
					fpin[p] = fopen(word.c_str(), "r");
					break;
				}

				cmds[p][q] = new char[1024];
				strcpy(cmds[p][q++], word.c_str());
			}
			cmds[p++][q] = NULL;
		} while (word == "|");
		cmds[p] = NULL;


		/* Execute command */
		pid_t pid = fork();

		if (pid < 0) { /* error occurred */
			fprintf(stderr, "Fork Failed");
			exit(-1);
		} else if (pid == 0) { /* child process */
			stdout_fd = dup(STDOUT_FILENO);
			int stdin_pipe = 0;
			p = 0;
			do {
				pipe(pipe_fd);
				pid_t pid2 = fork();

				if (pid2 < 0) {
					fprintf(stderr, "Fork Failed");
					exit(-1);
				} else if (pid2 == 0) {
					if (fpin[p] != nullptr)
						dup2(fileno(fpin[p]), STDIN_FILENO);
					else
						dup2(stdin_pipe, STDIN_FILENO);

					if (fpout[p] != nullptr)
						dup2(fileno(fpout[p]), STDOUT_FILENO);
					else if (cmds[p+1] != NULL)
						dup2(pipe_fd[1], STDOUT_FILENO);
					else
						dup2(stdout_fd, STDOUT_FILENO);

					execvp(cmds[p][0], cmds[p]);
					close(pipe_fd[1]);
					if (fpin[p] != nullptr) fclose(fpin[p]);
					if (fpout[p] != nullptr) fclose(fpout[p]);
					exit(0);
				} else {
					wait(NULL);
					close(pipe_fd[1]);
					stdin_pipe = pipe_fd[0];
				}
			} while (cmds[++p] != NULL);
			dup2(stdout_fd, STDOUT_FILENO);
		} else {
			if (word != "&") {
				wait(NULL);
			}
		}
	}

	wait(NULL);
}
