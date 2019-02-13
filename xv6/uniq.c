/*
	Yikai Wang
	CS3224 2019 Spring
  uniq.c
*/
#include "types.h"
#include "user.h"

void print_line(char *s, int count, char flag_c, char flag_d) {
  if (strlen(s) > 0) {
    if (flag_c) {
      printf(1, "%4d ", count);
    }
    if (flag_d && count > 1) {
      printf(1,"%s", s);
    }
    else if (!flag_d) {
      printf(1, "%s", s);
    }
  }
}

int compare_to_prev_line(char *prev, char *curr, char flag_i) {
  if (prev[0] == '\0') {
    return 1;
  }
  if (!flag_i) {
      if (strcmp(prev, curr) != 0) {
        return 1;
      }
      return 0;
  }
  else {
    int size_1 = strlen(prev);
    int size_2 = strlen(curr);

    if (size_1 != size_2) {
      return 1;
    }

    int i;
    for(i = 0; i < size_1; ++i) {
      char c1 = (prev[i] >= 'A' && prev[i] <= 'Z') ? prev[i] + 32 : prev[i];
      char c2 = (curr[i] >= 'A' && curr[i] <= 'Z') ? curr[i] + 32 : curr[i];
      if (c1 != c2) {
        return 1;
      }
    }
    return 0;
  }
  return 1;
}

void uniq(int fd, char flag_c, char flag_d, char flag_i) {
  int char_ind = 0;
  int buffer_size = 512;
  char *prev = malloc(sizeof(char) * buffer_size);
  prev[0] = '\0';
  char *curr = malloc(sizeof(char) * buffer_size);
  char buffer[buffer_size];
  int count = 0;

  int num_bytes;
  while ((num_bytes = read(fd, buffer, sizeof(buffer))) > 0) {
    int i = 0;
    for(; i < num_bytes; ++i) {
      if (char_ind >= buffer_size - 1) {
        buffer_size *= 2;
        char *new_buf = malloc(sizeof(char) * buffer_size);
        strcpy(new_buf, curr);
        free(curr);
        curr = new_buf;
      }

      if (buffer[i] != '\r') {
        curr[char_ind] = buffer[i];
        char_ind += 1;
      if (buffer[i] == '\n') {
        curr[char_ind] = '\0';
        // printf(1, "\n```````````\n%s\n```````````\n", lines[ind]);
        char_ind = 0;
        int is_new = compare_to_prev_line(prev, curr, flag_i);

        if (is_new) {
            print_line(prev, count, flag_c, flag_d);
            if (sizeof(curr) > sizeof(prev)) {
              free(prev);
              prev = malloc(sizeof(char) * strlen(curr) + 1);
            }
            strcpy(prev, curr);
            // printf(1, "```````````\nprev\n``````````````\n%s\n", prev);
            // printf(1, "```````````\ncurr\n``````````````\n%s\n", curr);
            count = 1;
        }
        else {
          count += 1;
        }
      }
    }
    }
  }
  if(num_bytes < 0){
    printf(1, "uniq: read error\n");
    free(prev);
    free(curr);
    close(fd);
    exit();
  }

  if (char_ind > 0) {
    curr[char_ind] = '\0';
    int is_new = compare_to_prev_line(prev, curr, flag_i);
    if (is_new) {
      print_line(prev, count, flag_c, flag_d);
      print_line(curr, 1, flag_c, flag_d);
    }
    else {
      print_line(prev, count + 1, flag_c, flag_d);
    }
  }
  else {
    print_line(prev, count, flag_c, flag_d);
  }

  free(prev);
  free(curr);
}

int main(int argc, char *argv[]) {
  if(argc <= 1){
    uniq(0, 0, 0, 0);
    exit();
  }

  int fd, i;
  char *file_name = "";
  char flag_c, flag_d, flag_i = 0;

  for(i = 1; i < argc; i++) {
      if(!strcmp(argv[i], "-c")) {
        flag_c = 1;
      }
      else if(!strcmp(argv[i], "-d")) {
        flag_d = 1;
      }
      else if(!strcmp(argv[i], "-i")) {
        flag_i = 1;
      }
      else {
        file_name = argv[i];
      }
  }

  if(flag_c && flag_d) {
    printf(2, "-c and -d cannot appear in the same line\n");
    exit();
  }

  if(!strlen(file_name)) {
    uniq(0, flag_c, flag_d, flag_i);
    exit();
  }

  if((fd = open(file_name, 0)) < 0){
    printf(1, "uniq: cannot open %s\n", file_name);
    exit();
  }

  uniq(fd, flag_c, flag_d, flag_i);
  close(fd);
  // printf(1, "here\n");
  exit();
}
