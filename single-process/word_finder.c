//
// Created by denactive on 20.03.2021.
//
#include "word_finder.h"

char* file_input(const char* filename, struct stat* stat_buf);
char* execute_line_comment(const char *str);
char* execute_multiline_comment(const char *str);
char* expand_path(const char *path, const char *dir);
int expand_string_array(char*** file_list, size_t cnt, size_t nmemb);
size_t check_str (const char* str, const char* pattern);

char* execute_line_comment(const char *str) {
    if (!str)
        return NULL;

    while (*str != '\n') {
        if (*str == 0 || *str == EOF || *str == '\0')
            return NULL;
        str++;
    }
    return (char*)str + 1;
}

char* execute_multiline_comment(const char *str) {
    if (!str)
        return NULL;
    // an exception for /*/ smtg */ situation
    if (*str == '/')
        str++;
    // totally safe :)
    while (*str != '/' || *(str - 1) != '*') {
        if (*str == 0 || *str == EOF || *str == '\0')
            return NULL;
        str++;
    }
    return (char*)str + 1;
}

int get_files_from_dir(const char* dir_name, const char* extension, char*** file_list, size_t* cnt) {
    if (!dir_name || !file_list || !cnt)
        return -1;
    // smtg/. & smtg/.. cases
    char* last_point = strrchr(dir_name, '.');
    if (last_point && *(last_point + 1) == '\0')
        return 0;

    if (DEBUG)
        printf("\ttrying to open %s directory\n", dir_name);
    DIR *dir = opendir(dir_name);
    if (!dir) {
        printf("Error | %s opening failed\n", dir_name);
        return -1;
    }
    struct dirent *entry = NULL;
    while (1) {
        errno = 0;
        entry = readdir(dir);
        if (!entry) {
            if (errno && DEBUG)
                printf("Error | %s reading failed | %d\n", dir_name, errno);
            break;
        }

        if (DEBUG)
            printf("Working with: [%d] | %s\n", entry->d_type, entry->d_name);

        // creating a filepath to current entry
        char* entry_path = expand_path(dir_name, entry->d_name);
        if (MEMORY_EXP_PATH)
            printf("\tentry path check | %s | strlen: %zu | size %zu\n", entry_path, strlen(entry_path), sizeof(entry_path));
        if (!entry_path) {
            if (DEBUG) {
                printf("\tError | failed to cancat %s and %s", dir_name, entry->d_name);
                if (closedir(dir))
                    printf("Error | %s directory closing failed\n", dir_name);
                return -1;
            }
        }
        // entry->d_type == DT_DIR (4) -> dir, DT_REG (8) -> file
        if (entry->d_type == DT_DIR) {
            if (!strcmp(entry->d_name, ".") && !strcmp(entry->d_name, ".."))
                continue;

            if (DEBUG)
                printf("directory %s\n", entry_path);
            get_files_from_dir(entry_path, extension, file_list, cnt);
        }
        if (entry->d_type == DT_REG) {
            int match = 0;
            if (extension) {
                char *type_extension = strstr(entry->d_name, extension);
                if (type_extension && *(type_extension + 2) == '\0')
                    match = 1;
            } else {
                match = 1;
            }

            if (match) {
                if (MEMORY)
                    printf("\tBefore expansion | filelist %p | cnt %zu\n", (void*)(*file_list), *cnt);
                if (expand_string_array(file_list, *cnt, 1)) {
                    if (DEBUG)
                        printf("\tExpand string array error\n");
                    continue;
                } else {
                    if (MEMORY)
                        printf("\tAfter | Filelist is %p\n", (void*)(*file_list));
                    size_t line_len = strlen(entry_path);
                    (*file_list)[*cnt] = calloc((line_len + 1), sizeof(char));
                    if (!strncpy((*file_list)[*cnt], entry_path, line_len)) {
                        free((*file_list)[*cnt]);
                    } else {
                        (*file_list)[*cnt][line_len] = '\0';
                        (*cnt)++;
                    }
                }
            }
        }
        free(entry_path);
    }
    if (closedir(dir)) {
        printf("Error | %s directory closing failed\n", dir_name);
        return -1;
    }
    return 0;
}

//this function allocates memory!
char* expand_path(const char *path, const char *dir) {
    char* res = NULL;
    int bs_flag = 0;
    char* last_backslash = strrchr(path, '/');
    if (last_backslash)
        if (*(last_backslash + 1) == '\0')
        bs_flag = 1;

    size_t res_len = strlen(path) + strlen(dir) + 1; // dir + '/' + new_dir
    if (bs_flag)
        res_len--;

    res = (char *) calloc(res_len + 1, sizeof(char));
    if (!res) {
        if (DEBUG)
            printf("\tfailed to calloc\n");
        return NULL;
    }

    if (!strncpy(res, path, strlen(path)))
        if (DEBUG)
            printf("\tfailed to strncpy\n");
    if (!bs_flag)
        res[strlen(path)] = '/';
    if (!strncat(res, dir, strlen(dir)))
        if (DEBUG)
            printf("\tfailed to strncat\n");
    res[res_len] = '\0';

    if (MEMORY_EXP_PATH) {
        printf("\texpand_path check | reslen: %zu\n\t", res_len);
        for (size_t i = 0; i <= res_len; i++)
            printf("%c (%i) ", res[i], res[i]);
        printf("\n");
    }
    return res;
}

int expand_string_array(char*** file_list, size_t cnt, size_t nmemb) {
    if (!file_list)
        return -1;

    if (!(*file_list)) {
        if (MEMORY)
            printf("\tFilelist is %p -> callocing 1\n", (void*)(*file_list));
        *file_list = (char **)calloc(1, sizeof(char *));
        if (!(*file_list)) {
            if (DEBUG)
                printf("\tfailed to calloc\n");
            return -1;
        }
    } else {

        if (MEMORY)
            printf("\tFilelist is %p -> reallocing %zu\n", (void*)(*file_list), cnt+nmemb);
        char **tmp = (char **)realloc(*file_list, (cnt + nmemb)*sizeof(char*));
        if (MEMORY)
            printf("\ttmp is %p\n", (void*)tmp);
        if (!tmp) {
            if (DEBUG)
                printf("\tfailed to realloc\n");
            return -1;
        }
        (*file_list) = tmp;
        if (MEMORY)
            printf("\tFilelist is %p\n", (void*)(*file_list));
    }
    return 0;
}
// this function allocates memory!
// create_word_search_result allocates memory for string_size_pair array
// and copies file names in "name" field and initializes "matches_amount" field to 0
string_size_pair* create_word_search_result(char** file_list, size_t files_amount) {
    if (!file_list || !files_amount)
        return NULL;
    string_size_pair* res = calloc(files_amount, sizeof(string_size_pair));
    if (!res) {
        if (DEBUG)
            printf("\tfailed to calloc\n");
        return NULL;
    }
    for (size_t i = 0; i < files_amount; ++i)
        res[i].name = file_list[i];
    return res;
}

int word_search(const char* pattern, string_size_pair** word_search_result, size_t files_amount) {
    if (!pattern || !word_search_result)
        return -1;
    if (!files_amount)
        return 0;
    for (size_t i = 0; i < files_amount; ++i) {
        struct stat stat_buf = {0};
        if (DEBUG)
            printf("\t reading %s\n", (*word_search_result)[i].name);

        char* str = file_input((*word_search_result)[i].name, &stat_buf);
        if (!str)
            continue;

        if (DEBUG)
            printf("\t%s\n", (*word_search_result)[i].name);

        (*word_search_result)[i].matches_amount = check_str(str, pattern);
        // file mapping closing
        if (munmap((void*)str, stat_buf.st_size))
            printf("Error | failed to unmap %s\n", (*word_search_result)[i].name);
    }
    return 0;
}

char* file_input(const char* filename, struct stat* stat_buf) {
    int fdin = 0;
    if ((fdin = open(filename, O_RDONLY)) < 0) {
        printf("Error | Usage %s invalid\n", filename);
        return NULL;
    }

    // gain file size
    if (fstat(fdin, stat_buf) < 0)
        return NULL;

    // using mmap
    void* content = NULL;
    if ((content = mmap(0, stat_buf->st_size, PROT_READ, MAP_SHARED, fdin, 0)) == MAP_FAILED) {
        close(fdin);
        return NULL;
    }

    close(fdin);
    return (char*)content;
}

size_t check_str (const char* str, const char* pattern) {
    if (!str || !pattern)
        return 0;
    int double_quote_flag = 0;
    int single_quote_flag = 0;
    size_t count = 0;
    while (*str != '\0' && *str != 0 && *str != EOF) {
        switch (*str) {
            case '\"':
                // do not raise double_quote_flag if a single quote was found
                if (QUOTES_CHECK) printf("\t\t\" found s: %d | d: %d\n", single_quote_flag, double_quote_flag);
                if (!single_quote_flag) {
                    if (double_quote_flag)
                        double_quote_flag = 0;
                    else
                        double_quote_flag = 1;
                }
                if (QUOTES_CHECK) printf("\t\t        s: %d | d: %d\n", single_quote_flag, double_quote_flag);
                str++;
                break;
            case '\'':
                if (QUOTES_CHECK) printf("\t\t\' found s: %d | d: %d\n", single_quote_flag, double_quote_flag);
                if (!double_quote_flag) {
                    if (single_quote_flag)
                        single_quote_flag = 0;
                    else
                        single_quote_flag = 1;
                }
                if (QUOTES_CHECK) printf("\t\t        s: %d | d: %d\n", single_quote_flag, double_quote_flag);
                str++;
                break;
            case '/':
                if (single_quote_flag || double_quote_flag) {
                    str++;
                    break;
                }
                if (*(str + 1) == '/') {
                    str = execute_line_comment(str + 2);
                    if (!str) {
                        if (DEBUG)
                            printf("\tError | single line failed\n");
                        return count;
                    }
                }
                if (*(str + 1) == '*') {
                    str = execute_multiline_comment(str + 2);
                    if (!str) {
                        if (DEBUG)
                            printf("\tError | multiline failed\n");
                        return count;
                    }
                }
                break;
            default: {
                size_t j = 0;
                int match = 1;
                for (; pattern[j] != '\0' && *str != '\0' && *str != EOF && *str != 0 && pattern[j] == str[j]; ++j);
                if (pattern[j] != '\0')
                    match = 0;
                if (match)
                    count++;
                str += (j + 1);
                break;
            }
        }
    }
    return count;
}

int merge_sort_desc(string_size_pair* arr, size_t len) {
    if (!arr) {
        printf("Error | Unable to sort an empty arr\n");
        return 0;
    }
    if (len <= 1)
        return 0;

    size_t lsize = len / 2;
    size_t rsize = len - lsize;

    if (merge_sort_desc(arr, lsize))
        return -1;
    if (merge_sort_desc(arr + lsize, rsize))
        return -1;

    string_size_pair* tmp = calloc(len, sizeof(string_size_pair));
    if (!tmp)
        return -1;

    if (merge_desc(arr, lsize, arr + lsize, rsize, tmp))
        return -1;

    if (!memcpy(arr, tmp, len * sizeof(string_size_pair))) {
        if (DEBUG)
            printf("\tError | memcpy failed\n");
        return -1;
    }
    free(tmp);
    return 0;
}

int merge_desc(string_size_pair* l, size_t lsize, string_size_pair* r, size_t rsize, string_size_pair* res) {
    size_t lpos = 0;
    size_t rpos = 0;
    while (lpos < lsize && rpos < rsize) {
        // lsize = (len \ 2) --> lsize <= rsize --> дмуаю, так (>=) немного быстрее
        if (l[lpos].matches_amount >= r[rpos].matches_amount) {
            res[lpos + rpos] = l[lpos];
            lpos++;
        } else {
            res[lpos + rpos] = r[rpos];
            rpos++;
        }
    }
    if (lpos == lsize) {
        if (!memcpy(res + lpos + rpos, r + rpos, (rsize - rpos) * sizeof(string_size_pair)))
            return -1;
    } else {
        if (!memcpy(res + lpos + rpos, l + lpos, (lsize - lpos) * sizeof(string_size_pair)))
            return -1;
    }
    return 0;
}

