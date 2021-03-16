#include "hw-1.h"

int main(int argc, const char** argv) {
    if (argc == 1) {
        if (parse_comments_from_console())
            return -1;
    } else {
        if (parse_comments_from_file(argv[1]))
            return -1;
    }
    return 0;
}
