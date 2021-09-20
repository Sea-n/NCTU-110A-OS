#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>

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
	string line, word;
	stringstream ss;

	for (;!cin.eof();) {
		/* fork another process */
		pid_t pid = fork();

		if (pid < 0) { /* error occurred */
			fprintf(stderr, "Fork Failed");
			exit(-1);
		} else if (pid == 0) { /* child process */
			cout << "> ";
			getline(cin, line);
			ss << line;
			char *argv[1024];
			int p = 0;
			while (ss >> word) {
				argv[p] = new char[1024];
				strcpy(argv[p], word.c_str());
				p++;
			}
			execvp(argv[0], argv);
		} else { /* parent process */
			/* parent will wait for the child to complete */
			wait(NULL);
		}
	}


}
