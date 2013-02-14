#include <iostream>
#include <sstream>
using namespace std;

class Bot {
  private:
    int write_pipe;
    int read_pipe;
  public:
    Bot (char* const args[]) {
      //Declare pipe ends:
      int stdin_pipe[2];
      int stdout_pipe[2];

      //Make pipes:
      pipe(stdin_pipe);
      pipe(stdout_pipe);

      //Fork ourselves:
      int pid = fork();
      if (pid < 0) {
        //If pid == -1, the fork failed
        cout << "Error in forking." << endl;
        return;
      }
      if (pid == 0) {
        //If we are the child, we close the unneeded stdin_write and stdout_read ends:
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);

        //We relink the read and writes
        if (dup2(stdin_pipe[0], STDIN_FILENO) == -1) {
          cout << "Error in relinking stdin" << endl;
          return;
        }
        if (dup2(stdout_pipe[1], STDOUT_FILENO) == -1) {
          cout << "Error in relinking stdout" << endl;
          return;
        }

        //Then we execute the bot program.
        execvp(args[0], args);

        //If we get here, execvp failed:
        cout << "execvp failed" << endl;
        return;
      }
      else {
        //If we are the parent, we close the unneeded stdin_read and stdout_write ends
        close(stdin_pipe[0]);
        close(stdout_pipe[1]);
        //The we save the ends we need:
        read_pipe = stdout_pipe[0];
        write_pipe = stdin_pipe[1];
      }
    }

    bool feed_c(char f) {
      write(write_pipe, &f, 1);
      return true;
    }

    char read_c() {
      char b;
      read(read_pipe, &b, 1);
      return b;
    }
};

string stringify (int n, int* a) {
  stringstream s;
  s << '[';
  for (int i = 0; i < n; i += 1) {
    s << a[i];
    if (i < n - 1) s << ',' << ' ';
  }
  s << ']';
  return s.str();
}

int getPlay(char w) {
  switch (w) {
    case 'R':
      return 0;
    case 'P':
      return 1;
    case 'S':
      return 2;
  }
}

int main(int n, char* args[]) {
  //Initiate the contestants:
  char* morple_args[] = {"./morple", NULL};
  char* zfeng_args[] = {"./zfeng.rb", NULL};
  Bot morple (morple_args);
  Bot zfeng (zfeng_args);

  int score[] = {0,0,0};

  int num_games = stoi(args[1]);

  for (int i = 0; i < num_games; i += 1) {
    char mplay = morple.read_c();
    char zplay = zfeng.read_c();
    cout << mplay << ' ' << zplay << ' ' << stringify(3, score) << endl;
    score[(getPlay(zplay) - getPlay(mplay) + 4) % 3] += 1;
    morple.feed_c(zplay);
    zfeng.feed_c(mplay);
  }

  return 0;
}
