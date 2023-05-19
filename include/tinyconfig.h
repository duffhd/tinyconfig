// Copyright 2023 Alexandre Parra
// MIT License
// tiny config, a simple key-value pair config file reader.

#pragma once

#include <stdio.h>

#define CONFIG_DEFAULT_SIZE 50

typedef struct {
    char *key;
    char *value;
} tc_pair;

typedef struct {
    tc_pair pair[CONFIG_DEFAULT_SIZE];
    size_t  size;
} tc_config;

int tc_load_config(tc_config **config, const char *file_path);

char *tc_get_char(tc_config *config, const char *key_name);

int tc_get_int(tc_config *config, const char *key_name, int *result);

int tc_get_float(tc_config *config, const char *key_name, float *result);

void tc_free(tc_config *config);