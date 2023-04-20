// Jackie Wang Copyright 2023
#include <mmap_fstream.h>
#include <unistd.h>

// Creates Memory-mapped file stream obj without file name; file name must be
// specified in call to Open.
mem_map::fstream::fstream() {
  file_ = nullptr;
}

mem_map::fstream::fstream(const std::string& fname) {
  open(fname);
}

mem_map::fstream::fstream(const std::string& fname, std::ios_base::openmode mode) {
  open(fname, mode);
}

void mem_map::fstream::open(const std::string& fname) {
  fd = ::open(fname.c_str(), O_RDWR | O_CREAT, 0666);
  struct stat fileInfo;
  if (::fstat(fd, &fileInfo) == -1)
    std::cerr << "Error getting file size" << std::endl;
  size_ = fileInfo.st_size;
  if (fd == -1) {
    std::cerr << "Error opening file" << std::endl;
  }
}

// use open open
// use access for checking if file exists
// fstat size
// mmap for put and get
// O_CREATE do not include if the file already exists.
void mem_map::fstream::open(const std::string& fname, std::ios_base::openmode mode) {
  
  int m = mode_conversion(mode);
  fd = ::open(fname.c_str(), m, 0664);
  struct stat fileInfo;
  if (::fstat(fd, &fileInfo) == -1)
    std::cerr << "Error getting file size" << std::endl;
  size_ = fileInfo.st_size;
  if (fd == -1)
    std::cerr << "Error opening file" << std::endl;
}

void mem_map::fstream::close() {
  if (is_open()) {
    int result = ::close(fd);
    if (result == -1) {
      std::cerr << "Error closing file" << std::endl;
    }
  }
}

bool mem_map::fstream::is_open() const {
  int flags = fcntl(fd, F_GETFL);
  if (flags == -1) {
    return false;
  } else {
    return true;
  }
}

std::size_t mem_map::fstream::size() const {
  return size_;
}

char mem_map::fstream::get() {
  char* fileData = static_cast<char*>(mmap(nullptr,
                                      size_,
                                      PROT_READ,
                                      MAP_PRIVATE,
                                      fd,
                                      0));
  if (fileData == MAP_FAILED) {
    std::cerr << "Error mapping file" << std::endl;
  }
  if (cursor == size_) {
    cursor = 0;
  }
  char c = fileData[cursor];
  cursor++;
  msync(fileData, size_, MS_SYNC);
  munmap(fileData, size_);
  
  return c;
}

mem_map::fstream& mem_map::fstream::put(char c) {
  char* fileData = static_cast<char*>(mmap(nullptr,
                                      size_,
                                      PROT_READ | PROT_WRITE,
                                      MAP_SHARED,
                                      fd,
                                      0));
  std::cout << fd << std::endl;
  if (fileData == MAP_FAILED) {
    std::cerr << "Error mapping file" << std::endl;
  }
  if (cursor == size_) {
    size_++;
    ftruncate(fd, size_);
  }
  fileData[cursor] = c;
  cursor++;
}

int mem_map::fstream::mode_conversion(std::ios_base::openmode mode) {
  int mask = 0;
  if (mode & std::ios_base::in) {
    mask |= O_RDONLY;
  }
  if (mode & std::ios_base::out) {
    mask |= O_WRONLY | O_CREAT;
  }
  if (mode & std::ios_base::ate) {
    mask |= O_APPEND;
    cursor = lseek(fd, 0, SEEK_END);
  }
  return mask;
}








