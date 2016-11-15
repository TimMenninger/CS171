#ifndef FILE_UTIL
#define FILE_UTIL

inline int getFileLength(std::ifstream &file) {
    if (!file.good()) {
        return 0;
    }

    // unsigned long pos = file.tellg();
    file.seekg(0, std::ios::end);
    int len = file.tellg();
    file.seekg(std::ios::beg);

    return len;
}

inline int loadFile(const char *filename, char **source, int *len) {
    std::ifstream file;
    file.open(filename, std::ios::in); // opens as ASCII!
    if (!file) {
        return -1;
    }

    *len = getFileLength(file);

    if (*len == 0) {
        return -2;   // Error: Empty File
    }

    *source = new char[*len + 1]();
    if (*source == NULL) {
        return -3;   // can't reserve memory
    }

    // len isn't always strlen cause some characters are stripped in ascii read
    // it is important to 0-terminate the real length later, len is just max
    // possible value...
    (*source)[*len] = 0;

    unsigned int i = 0;
    while (file.good()) {
        file.get((*source)[i]);       // get character from file.
        if (!file.eof()) {
            i++;
        }
    }

    (*source)[i] = 0;  // 0-terminate it at the correct position

    file.close();

    return 0; // No Error
}


inline int unloadFile(GLubyte **source) {
    if (*source != 0) {
        delete[] *source;
    }
    *source = 0;

    return 0;
}

#endif