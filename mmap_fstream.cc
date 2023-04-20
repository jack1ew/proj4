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
  int flag = mode_conversion(std::ios_base::in | std::ios_base::out);
  int prot = prot_conversion(std::ios_base::in | std::ios_base::out);
  if (access(fname.c_str(), F_OK) != -1) {
    flag |= O_CREAT;
  }
  fd = ::open(fname.c_str(), flag, 0666);
  if (fd == -1) {
    std::cerr << "Error opening file" << std::endl;
  }
  struct stat fileInfo;
  if (::fstat(fd, &fileInfo) == -1)
    std::cerr << "Error getting file size" << std::endl;
  size_ = fileInfo.st_size;
  file_ = static_cast<char*>(mmap(nullptr,
                                  size_,
                                  prot,
                                  MAP_SHARED,
                                  fd,
                                  0));
  if (file_ == MAP_FAILED) {
    std::cerr << "Error mapping file" << std::endl;
  }
}

// use open open
// use access for checking if file exists
// fstat size
// mmap for put and get
// O_CREATE do not include if the file already exists.
void mem_map::fstream::open(const std::string& fname, std::ios_base::openmode mode) {
  int flag = mode_conversion(mode);
  int prot = prot_conversion(mode);
  if (access(fname.c_str(), F_OK) != -1) {
    flag |= O_CREAT;
  }
  fd = ::open(fname.c_str(), flag, 0666);
  if (fd == -1) {
    std::cerr << "Error opening file" << std::endl;
  }
  struct stat fileInfo;
  if (::fstat(fd, &fileInfo) == -1)
    std::cerr << "Error getting file size" << std::endl;
  size_ = fileInfo.st_size;
  file_ = static_cast<char*>(mmap(nullptr,
                                  size_,
                                  prot,
                                  MAP_SHARED,
                                  fd,
                                  0));
  if (file_ == MAP_FAILED) {
    std::cerr << "Error mapping file" << std::endl;
  }
}

void mem_map::fstream::close() {
  if (is_open()) {
    ftruncate(fd, size_);
    msync(file_, size_, MS_SYNC);
    munmap(file_, size_);
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
  if (cursor == size_) {
    cursor = 0;
  }
  char c = file_[cursor];
  cursor++;
  return c;
}

mem_map::fstream& mem_map::fstream::put(char c) {
  if (cursor == size_) {
    size_++;
    ftruncate(fd, size_);
  }
  file_ = static_cast<char*>(mremap(file_, size_ - 1, size_, MREMAP_MAYMOVE));
  file_[cursor] = c;
  cursor++;
  return *this;
}

int mem_map::fstream::mode_conversion(std::ios_base::openmode mode) {
  int mask = 0;
  if (mode & std::ios_base::in && mode & std::ios_base::out) {
    mask |= O_RDWR;
  } else if (mode & std::ios_base::in) {
    mask |= O_RDONLY;
  } else if (mode & std::ios_base::out) {
    mask |= O_WRONLY;
  }
  if (mode & std::ios_base::ate) {
    mask |= O_APPEND;
    cursor = size();
  }
  return mask;
}

int mem_map::fstream::prot_conversion(std::ios_base::openmode mode) {
  int mask = 0;
  if (mode & std::ios_base::in && mode & std::ios_base::out) {
    mask |= PROT_READ | PROT_WRITE;
  } else if (mode & std::ios_base::in) {
    mask |= PROT_READ;
  } else if (mode & std::ios_base::out) {
    mask |= PROT_WRITE;
  }
  return mask;
}









