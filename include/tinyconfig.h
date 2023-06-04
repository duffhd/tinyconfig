// Copyright 2023 Alexandre Parra
// MIT License
// tiny config, a simple key-value pair config file reader.

#pragma once

#include <stdio.h>

typedef struct tc_config tc_config;

int tc_load_config(tc_config **config, const char *file_path);

char *tc_get_value(tc_config *config, const char *key_name);

char *tc_set_value(tc_config *config, const char *key_name, char *value);

void tc_save_to_file(tc_config *config, const char *file_path);

void tc_free(tc_config *config);