# Project 4

## fstream()
- Creates memory mapped file stream object without a file name. Initializes a character pointer with a nullptr.

## fstream(string)
- Creates memory mapped file stream object with a file name. Opens the file with the permission of ios::in | ios::out.

## fstream(string, ios::openmode)
- Creates memory mapped file stream object with a file name and a mode. Opens the file with the permission of the mode.

## open(string)
- Opens the file with the given name and the permission of ios::in | ios::out.

## open(string, ios::openmode)
- Opens the file with the given name and the permission of the mode.

## close()
- Closes the file.

## is_open()
- Returns true if the file is open.

## size()
- Returns the size of the file.

## get()
- Returns the next character in the file.

## put(char)
- Puts the given character in the file.

## mode_conversion(ios::openmode)
- Converts the mode to the flag of the open() function.

## prot_conversion(ios::openmode)
- Converts the mode to the flag of the mmap() function.

