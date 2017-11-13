#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <string>
#include <cstring>
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

void read_directory(const string& name, vector<string>& v){
  DIR* dirp = opendir(name.c_str());
  struct dirent * dp;

  while ((dp = readdir(dirp)) != NULL)
    if(strcmp(dp->d_name, "..") && strcmp(dp->d_name, "."))
      v.push_back(dp->d_name);

  closedir(dirp);
}
