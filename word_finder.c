//
// Created by denactive on 20.03.2021.
//

#include "word_finder.h"

char* skip_to_single_quote(const char *str);
char* skip_to_double_quote(const char *str);
char* execute_line_comment(const char *str);
char* execute_multiline_comment(const char *str);
char* expand_path(const char *path, const char *dir);
int expand_string_array(char*** file_list, size_t* cnt, size_t nmemb);

// starts from a sym after ' and returns a sym after closing '
char* skip_to_single_quote(const char* str) {
    if (!str)
        return NULL;
    while (*str != '\'') {
        if (*str == 0 || *str == EOF || *str == '\0')
            return NULL;
        // \' avoidance
        if (*str == '\\') {
            str++;
            if (*str == 0 || *str == EOF || *str == '\0')
                str--;
        }
        str++;
    }
    return (char*)str + 1;
}

// starts from a sym after " and returns a sym after closing "
char* skip_to_double_quote(const char* str) {
    if (!str)
        return NULL;
    while (*str != '\"') {
        if (*str == 0 || *str == EOF || *str == '\0')
            return NULL;
        // \" avoidance
        if (*str == '\\') {
            str++;
            if (*str == 0 || *str == EOF || *str == '\0')
                str--;
        }
        str++;
    }
    return (char*)str + 1;
}

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

    //FILE *file = fopen(dir_name, "O_RDONLY");
    errno = 0;
    if (DEBUG)
        printf("\ttrying to open %s directory\n", dir_name);
    DIR *dir = opendir(dir_name);
    if (!dir) {
        printf("Error | %s opening failed\n", dir_name);
        return -1;
    }
    struct dirent *entry = NULL;
    // TODO: readdir_r для мультипотока
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

        // entry->d_type == DT_DIR (4) -> dir, DT_REG (8) -> file
        if (entry->d_type == DT_DIR) {
            if (!strcmp(entry->d_name, ".") && !strcmp(entry->d_name, ".."))
                continue;

            char* dir_name_r = expand_path(dir_name, entry->d_name);
            if (!dir_name_r) {
                if (DEBUG) {
                    printf("\tError | failed to cancat %s and %s", dir_name, entry->d_name);
                    if (closedir(dir))
                        printf("Error | %s directory closing failed\n", dir_name);
                    return -1;
                }
            }
            if (DEBUG)
                printf("directory %s\n", dir_name_r);
            get_files_from_dir(dir_name_r, extension, file_list, cnt);
            free(dir_name_r);
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
                if (expand_string_array(file_list, cnt, 1)) {
                    if (DEBUG)
                        printf("\tExpand string array error\n");
                    continue;
                } else {
                    size_t line_len = strlen(entry->d_name);
                    (*file_list)[(*cnt) - 1] = malloc((line_len + 1) * sizeof(char));
                    if (!strncpy((*file_list)[(*cnt) - 1], entry->d_name, line_len))
                        free((*file_list)[--(*cnt)]);
                    else
                        (*file_list)[(*cnt) - 1][line_len] = '\0';
                }
                printf("%d | %s\n", entry->d_type, entry->d_name);
            }
        }
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
    if (last_backslash && *(last_backslash + 1) == '\0')
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

    return res;
}

int expand_string_array(char*** file_list, size_t* cnt, size_t nmemb) {
    if (!file_list || !cnt)
        return -1;

    if (!(*file_list)) {
        *file_list = (char **)calloc(1, sizeof(char *));
        if (!(*file_list)) {
            if (DEBUG)
                printf("\tfailed to calloc\n");
            return -1;
        }
    } else {
        char **tmp = (char **)realloc(*file_list, (*cnt) + nmemb);
        if (!tmp) {
            if (DEBUG)
                printf("\tfailed to realloc\n");
            return -1;
        }
        (*file_list) = tmp;
    }
    (*cnt) += nmemb;
    return 0;
}

char* file_input(const char* filename, struct stat* stat_buf) {
    char chunk[BUFFER_SIZE];
    fread(chunk, sizeof(char), BUFFER_SIZE, stdin);
    printf("%s\n\n", chunk);
    //return content;
    return NULL;
}