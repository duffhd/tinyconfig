// Copyright 2023 Alexandre Parra
// MIT License
// tiny config, a simple key-value pair config file reader.

#include "include/tinyconfig.h"

#include <ctype.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
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

#ifndef TC_CONFIG_DEFAULT_SIZE
    #define TC_CONFIG_DEFAULT_SIZE 30
#endif

#ifndef TC_CONFIG_DEFAULT_GROW_SIZE
    #define TC_CONFIG_DEFAULT_GROW_SIZE 10
#endif

typedef struct {
    char *key;
    char *value;
} tc_pair;

struct tc_config {
    tc_pair *pair;
    size_t  size;
    size_t  alloc_size;
    size_t  buffer_size;
};

/////////////////////////////////////////////////////////////////////////////
// Static
//---------------------------------------------------------------------------

static void tc_str_copy(const char *source, size_t start, size_t end, char* dest)
{
    size_t i;
    for (i = 0; i < end - start + 1; i++)
        dest[i] = source[i+start];
    dest[i] = '\0';
}

static void tc_str_copy_to_section(const char *source, size_t start, size_t end, char* dest)
{
    for (size_t i = 0; (i + start) < end; i++)
        dest[start+i] = source[i];
}

/////////////////////////////////////////////////////////////////////////////
// tinyconfig.h
//---------------------------------------------------------------------------

int tc_load_config(tc_config **config, const char *file_path)
{
    FILE *fp;
    int err = fopen_s(&fp, file_path, "rb");
    if (err)
        return -1;
    if (fgetc(fp) == EOF || ferror(fp))
        return -1;

    TC_FSEEK(fp, 0L, SEEK_END);
    size_t file_size = TC_FTELL(fp);
    rewind(fp);

    char* file_buffer = malloc(file_size + 1);
    if (!file_buffer)
        return -1;

    size_t bytes_read = fread(file_buffer, sizeof(char), file_size, fp);
    file_buffer[bytes_read] = '\0';

    fclose(fp);

    tc_config *tmp_conf = malloc(sizeof(tc_config));
    if (!tmp_conf) return -1;

    tmp_conf->pair        = malloc(sizeof(tc_pair) * TC_CONFIG_DEFAULT_SIZE);
    tmp_conf->size        = 0;
    tmp_conf->alloc_size  = TC_CONFIG_DEFAULT_SIZE;
    tmp_conf->buffer_size = 0;

    int size            = 0;
    char c              = 0;
    bool is_key_parsing = true;
    size_t config_size  = TC_CONFIG_DEFAULT_SIZE;
    for(size_t pos = 0; pos < bytes_read; (c = file_buffer[pos++]))
    {
        switch (c) {
            case ' ':
            case '\t':
            case '=': break;
            case '#': {
                while ((c = file_buffer[pos++]) != '\0') {
                    if (c == '\n') break;
                }
                break;
            }
            case '\0':
            case '\r':
            case '\n': break;
            default: {
                if (isalnum(c) || c == '-') {
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
                        else if (c == ' ' || c == '\t')
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
                    tmp_conf->buffer_size += end - start;
                    tc_str_copy(file_buffer, start - 1, end - 2, dest);

                    if (size > config_size) {
                        config_size         += TC_CONFIG_DEFAULT_GROW_SIZE;
                        tmp_conf->alloc_size = config_size;

                        tc_pair *new_pair = realloc(tmp_conf->pair, config_size);
                        if (new_pair != NULL) {
                            tmp_conf->pair = new_pair;
                        }
                    }

                    if (is_key_parsing) {
                        tmp_conf->pair[size].key = dest;
                    }
                    else {
                        tmp_conf->pair[size].value = dest;
                        // Add space to later allocate exactly '=' and '\n'
                        tmp_conf->buffer_size += 2;
                        size += 1;
                    }

                    is_key_parsing = !is_key_parsing;
                }
            }
        }
    }

    tmp_conf->buffer_size += 1;
    tmp_conf->size         = size;
    *config                = tmp_conf;
    free(file_buffer);

    return 0;
}

char *tc_get_value(tc_config *config, const char *key_name)
{
    for (size_t i = 0; i <= config->size; i += 1) {
        if (config->pair[i].key[0] == key_name[0] && strcmp(config->pair[i].key, key_name) == 0) {
            return config->pair[i].value;
        }
    }

    return NULL;
}

char *tc_set_value(tc_config *config, const char *key_name, char *value)
{
    size_t i;
    for (i = 0; i != config->size; i += 1)
    {
        if (config->pair[i].key[0] == key_name[0] && strcmp(config->pair[i].key, key_name) == 0)
        {
            size_t new_value_size = strlen(value);
            size_t old_value_size = strlen(config->pair[i].value);

            if (new_value_size > old_value_size)
            {
                char *new_memory = calloc(new_value_size, sizeof(char));
                if (!new_memory) {
                    return NULL;
                }

                strcpy(new_memory, value);
                char *old_memory = config->pair[i].value;

                config->pair[i].value = new_memory;
                config->buffer_size += new_value_size - old_value_size;
                free(old_memory);
            }

            strcpy(config->pair[i].value, value);
            return config->pair[i].value;
        }
    }

    if (config->size + 1 > config->alloc_size) {
        tc_pair *new_pair = realloc(config->pair, config->alloc_size + TC_CONFIG_DEFAULT_GROW_SIZE);
        if (new_pair != NULL) {
            config->pair = new_pair;
        }
    }

    char *new_key = malloc(strlen(key_name) * sizeof(char));
    strcpy(new_key, key_name);
    config->pair[i].key = new_key;

    char *new_value = malloc(strlen(value) * sizeof(char));
    strcpy(new_value, value);
    config->pair[i].value = new_value;

    config->size += 1;
    return config->pair[i].value;
}

void tc_save_to_file(tc_config *config, const char *file_path)
{
    FILE *fp;
    int err = fopen_s(&fp, file_path, "w");
    if (err)
        return;

#ifdef _MSC_VER
    char *buffer = malloc(sizeof(char) * config->buffer_size);
#else
    char buffer[config->buffer_size];
#endif

    size_t buffer_location = 0;

    for (size_t i = 0; i != config->size; i += 1)
    {
        // Advance the buffer_location based on the key-value sizes.
        size_t key_size     = strlen(config->pair[i].key);
        size_t key_location = buffer_location + key_size;
        tc_str_copy_to_section(config->pair[i].key, buffer_location, key_location, buffer);
        buffer_location += key_size;
        buffer[buffer_location] = '=';
        buffer_location += 1;

        size_t value_size     = strlen(config->pair[i].value);
        size_t value_location = buffer_location + value_size;
        tc_str_copy_to_section(config->pair[i].value, buffer_location, value_location, buffer);
        buffer_location += value_size;
        buffer[buffer_location] = '\n';
        buffer_location += 1;
    }

    buffer[buffer_location] = '\0';
    fputs(buffer, fp);
    fclose(fp);

#ifdef _MSC_VER
    free(buffer);
#endif
}

void tc_free(tc_config *config)
{
    for (size_t i = 1; i != config->size; i += 1) {
        free(config->pair[i].key);
        free(config->pair[i].value);
    }

    free(config);
}
