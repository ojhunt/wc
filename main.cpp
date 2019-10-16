#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <tuple>
#include <unistd.h>

std::tuple<void *, size_t> load_file(const char *filename) {
  auto filedes = open(filename, O_RDONLY);
  if (filedes < 0) {
    fprintf(stderr, "failed to open %s\n", filename);
    exit(-1);
  }
  struct stat statbuf;
  if (fstat(filedes, &statbuf) != 0) {
    fprintf(stderr, "failed to stat %s with %d\n", filename, errno);
    exit(-1);
  }
  void *mapped_file =
      mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, filedes, 0);
  if (mapped_file == MAP_FAILED) {
    fprintf(stderr, "failed to map %s with %d\n", filename, errno);
    exit(-1);
  }

  return {mapped_file, statbuf.st_size};
}

std::tuple<size_t, size_t, size_t> count(std::tuple<void *, size_t> file) {
  auto bytes = (char *)std::get<0>(file);
  auto length = std::get<1>(file);
  size_t line_count = 0;
  size_t word_count = 0;
  size_t character_count = 0;
  bool in_word = false;
  for (size_t i = 0; i < length; i++) {
    character_count++;
    if (isspace(bytes[i])) {
      if (in_word) {
        in_word = false;
      }
      if (bytes[i] == '\n') {
        line_count++;
      }
    } else if (!in_word) {
      word_count++;
      in_word = true;
    }
  }
  return {line_count, word_count, character_count};
}

int main(int argc, char *argv[]) {
  bool print_lc = false;
  bool print_wc = false;
  bool print_cc = false;
  bool print_mbc = false;
  while (auto ch = getopt(argc, argv, "clmw")) {
    argv += optind;
    argc -= optind;
    switch (ch) {
    case 'l':
      print_lc = true;
      break;
    case 'w':
      print_wc = true;
      break;
    case 'c':
      print_cc = true;
      print_mbc = false;
      break;
    case 'm':
      print_cc = false;
      print_mbc = true;
      break;
    case -1:
      goto endopts;
    default:
      fprintf(stderr, "Invalid arguments\n");
    }
  }
endopts:
  if (!print_wc && !print_cc && !print_lc && !print_mbc) {
    print_lc = true;
    print_wc = true;
    print_cc = true;
  }
  size_t twc = 0;
  size_t tlc = 0;
  size_t tcc = 0;
  size_t file_count = 0;
  while (char *filename = *argv++) {
    file_count++;
    auto results = count(load_file(filename));
    auto lc = std::get<0>(results);
    auto wc = std::get<1>(results);
    auto cc = std::get<2>(results);
    if (print_lc) {
      printf(" %7ju", lc);
    }
    if (print_wc) {
      printf(" %7ju", wc);
    }
    if (print_cc) {
      printf(" %7ju", cc);
    }
    printf(" %s\n", filename);
  }
  if (file_count > 1) {
    if (print_lc) {
      printf(" %7ju", tlc);
    }
    if (print_wc) {
      printf(" %7ju", twc);
    }
    if (print_cc) {
      printf(" %7ju", tcc);
    }
    printf(" total\n");
  }
  return 0;
}
