// Copyright 2023 Alexandre Parra
// MIT License
// tiny config, a simple key-value pair config file reader.

#include "include/tinyconfig.h"

#include <ctype.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Modified #define names, originally from tinyxml2
// https://github.com/leethomason/tinyxml2
#if defined(_WIN64)
#define TC_FSEEK _fseeki64
#define TC_FTELL _ftelli64
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__) || (__CYGWIN__)
#define TC_FSEEK fseeko
	#define TC_FTELL ftello
#elif defined(__ANDROID__)
    #if __ANDROID_API__ > 24
        #define TC_FSEEK fseeko64
        #define TC_FTELL ftello64
    #else
        #define TC_FSEEK fseeko
        #define TC_FTELL ftello
    #endif
#elif defined(__unix__) && defined(__x86_64__)
	#define TC_FSEEK fseeko64
	#define TC_FTELL ftello64
#else
	#define TC_FSEEK fseek
	#define TC_FTELL ftell
#endif


static void strcopy(const char *source, size_t start, size_t end, char* dest)
{
    size_t i;
    for (i = 0; i < end - start + 1; i++)
        dest[i] = source[i+start];
    dest[i] = '\0';
}

int tc_load_config(tc_config **config, const char *file_path)
{
    FILE *fp;
    int err = fopen_s(&fp, file_path, "rb");
    if (err) {
        return -1;
    }

    if (fgetc(fp) == EOF || ferror(fp)) {
        return -1;
    }

    TC_FSEEK(fp, 0L, SEEK_END);
    size_t file_size = TC_FTELL(fp);
    rewind(fp);

    char* file_buffer = (char *) malloc(file_size + 1);
    if (!file_buffer) {
        return -1;
    }

    size_t bytes_read = fread(file_buffer, sizeof(char), file_size, fp);
    file_buffer[bytes_read] = '\0';

    fclose(fp);

    tc_config *tmp_conf = (tc_config *) malloc(sizeof(tc_config));
    if (!tmp_conf) {
        return -1;
    }
    tmp_conf->size = 0;

    bool is_key_parsing = true;

    size_t config_size = CONFIG_DEFAULT_SIZE;
    int size           = 0;
    char c             = 0;
    for(size_t pos = 0; pos < bytes_read; (c = file_buffer[pos++])) {
        switch (c) {
            case ' ': break;
            case '=': {
                break;
            }
            case '#': {
                while ((c = file_buffer[pos++]) != '\0') {
                    if (c == '\n') break;
                }
                break;
            }
            case '\0':
            case '\r':
            case '\n': {
                break;
            }
            default: {
                if (isalnum(c)) {
                    bool count_end = true;
                    size_t start = pos;
                    size_t end;

                    for (;;) {
                        c = file_buffer[pos++];

                        if (c == '\0' || c == '\n' || c == '\r' || c == '=')
                        {
                            break;
                        }
                        else if (isalnum(c) || c == '_')
                        {
                            continue;
                        }
                        else if (c == ' ')
                        {
                            end = pos;
                            count_end = false;
                            while ((c = file_buffer[pos++]) != '\0')
                                if (c != ' ') break;
                            break;
                        }
                    }

                    if (count_end) end = pos;
                    char *dest = malloc(sizeof(char) * (end - start + 1));
                    strcopy(file_buffer, start - 1, end - 2, dest);

                    if (size > config_size) {
                        config_size += 20;
                        realloc(tmp_conf->pair, config_size);
                    }

                    if (is_key_parsing) {
                        tmp_conf->pair[size].key = dest;
                    }
                    else {
                        tmp_conf->pair[size].value = dest;
                        size += 1;
                    }

                    is_key_parsing = !is_key_parsing;
                }
            }
        }
    }

    tmp_conf->size = size;
    *config = tmp_conf;
    free(file_buffer);

    return 0;
}

static inline char *get_value(tc_config *config, const char *key_name)
{
    for (size_t i = 0; i != config->size; i += 1) {
        if (config->pair[i].key[0] == key_name[0] && strcmp(config->pair[i].key, key_name) == 0) {
            return config->pair[i].value;
        }
    }

    return NULL;
}

char *tc_get_char(tc_config *config, const char *key_name)
{
    return get_value(config, key_name);
}

int tc_get_int(tc_config *config, const char *key_name, int *result)
{
    char *value = get_value(config, key_name);
    if (value != NULL) {
        *result = atoi(get_value(config, key_name));
        return 0;
    }
    return -1;
}

int tc_get_float(tc_config *config, const char *key_name, float *result)
{
    char *value = get_value(config, key_name);
    if (value != NULL) {
        *result = atof(get_value(config, key_name));
        return 0;
    }
    return -1;
}

void tc_free(tc_config *config)
{
    for (size_t i = 0; i != config->size; i += 1) {
        free(config->pair[i].key);
        free(config->pair[i].value);
    }

    free(config);
}
