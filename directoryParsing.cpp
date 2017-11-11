#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

int is_regular_file(const char *path){
  struct stat path_stat;
  stat(path, &path_stat);
  return S_ISREG(path_stat.st_mode);
}

void read_directory(const std::string& name, vector<string>& v){
  DIR* dirp = opendir(name.c_str());
  struct dirent * dp;
  while ((dp = readdir(dirp)) != NULL) {
      v.push_back(dp->d_name);
  }
  closedir(dirp);
}

int main(){
  vector<string> list_files;
  string dir = "/home/mahsa/Downloads/assignment4";
  read_directory(dir, list_files);
  for (int i = 0; i < list_files.size(); ++i) {
      cout << is_regular_file((dir + '/' + list_files[i]).c_str())<<" " <<  list_files[i] << endl;
  }
}
