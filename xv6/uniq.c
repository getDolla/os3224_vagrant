/*
	Yikai Wang
	CS3224 2019 Spring
  uniq.c
*/
#include "types.h"
#include "user.h"

void free_lines(char **lines, int size) {
  int i;
  for(i = 0; i < size; ++i) {
    free(lines[i]);
  }
  free(lines);
}

char** lines_copy(char **lines, int size) {
  char** new_arr = malloc(sizeof(char *) * size * 2);
  int i;
  for(i = 0; i < size; ++i) {
    new_arr[i] = malloc(strlen(lines[i]) + 1);
    strcpy(new_arr[i], lines[i]);
  }
  free_lines(lines, size);
  return new_arr;
}

int* count_copy(int *count, int size) {
  int *new_count = malloc(sizeof(int) * size * 2);
  int i;
  for(i = 0; i < size; ++i) {
    new_count[i] = count[i];
  }
  free(count);
  return new_count;
}

int compare_to_prev_line(char **lines, int ind, char *line, int *count, char flag_i) {
  if (!ind) {
    return 1;
  }

  // printf(1, "line: %s\n", line);
  // printf(1, "line[ind]: %s\n", lines[ind - 1]);
  // printf(1, "%d\n", strcmp(line, lines[ind - 1]));
  if ((!flag_i) && (!strcmp(line, lines[ind - 1]))) {
      count[ind - 1] += 1;
      return 0;
  }
  else {
    int size_1 = strlen(line);
    int size_2 = strlen(lines[ind - 1]);

    if (size_1 != size_2) {
      return 1;
    }

    int i;
    for(i = 0; i < size_1; ++i) {
      char c1 = (line[i] >= 'A' && line[i] <= 'Z') ? line[i] + 32 : line[i];
      char c2 = (lines[ind - 1][i] >= 'A' && lines[ind - 1][i] <= 'Z') ? lines[ind - 1][i] + 32 : lines[ind - 1][i];
      if (c1 != c2) {
        return 1;
      }
    }
    count[ind - 1] += 1;
    return 0;
  }
  return 1;
}

void uniq(int fd, char flag_c, char flag_d, char flag_i) {
  int ind, char_ind = 0;
  int capacity = 10;
  int buffer_size = 512;
  char **lines = malloc(sizeof(char *) * capacity);
  char *buffer = malloc(sizeof(char) * buffer_size);
  lines[0] = malloc(sizeof(char) * buffer_size);
  int *count = malloc(sizeof(int) * capacity);
  count[0] = 1;

  int num_bytes;
  while ((num_bytes = read(fd, buffer, sizeof(buffer))) > 0) {
    int i = 0;
    for(; i < num_bytes; ++i) {
      if (char_ind >= buffer_size - 1) {
        buffer_size *= 2;
        char *new_buf = malloc(sizeof(char) * buffer_size);
        strcpy(new_buf, lines[ind]);
        free(lines[ind]);
        lines[ind] = new_buf;
      }

        if (!strchr("\n\r", buffer[i])) {
          lines[ind][char_ind] = buffer[i];
          // printf(1, "%c", lines[ind][char_ind]);
          char_ind += 1;
        }
        else {
        if (buffer[i] == '\n') {
          lines[ind][char_ind] = '\0';
          // printf(1, "\n```````````\n%s\n```````````\n", lines[ind]);
          char_ind = 0;
          int is_new = compare_to_prev_line(lines, ind, lines[ind], count, flag_i);

          if (is_new) {
            ind += 1;

            if (ind >= capacity) {
              lines = lines_copy(lines, capacity);
              count = count_copy(count, capacity);
              capacity *= 2;
            }

            lines[ind] = malloc(sizeof(char) * buffer_size);
            count[ind] = 1;
          }
        }
      }
    }
  }
  if(num_bytes < 0){
    printf(1, "uniq: read error\n");
    free_lines(lines, capacity);
    free(buffer);
    free(count);
    close(fd);
    exit();
  }

  lines[ind][char_ind] = '\0';
  if (!compare_to_prev_line(lines, ind, lines[ind], count, flag_i)) {
    free(lines[ind]);
    ind -= 1;
  }

  int i;
  for(i = 0; i <= ind; ++i) {
    if (flag_c) {
      printf(1, "%d ", count[i]);
    }
    if ((!flag_d) || (flag_d && count[i] > 1)) {
      printf(1, "%s\n", lines[i]);
    }
  }
  free_lines(lines, ind + 1);
  free(buffer);
  free(count);
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
