### tiny config

Tiny config is a simple key-value pair configuration file that can hold
numbers (decimal or floating point) and strings.

A simple config example:
```txt
# Line comments
project_name=tinyconfig
decimal=54
floating_point=3.14
# Numbers can be used as keys
42=key_number 
# You can use whitepaces before and after equals =
example = 34
```

### How to use
Tiny config is a simple header and source file that can be called from any
build system easily, you can pull this repository or simply copy the source/header file
`tinyconfig.c` and include the header `tinyconfig.h`.

Inside your `C` or `C++` code:
```c
#include "tinyconfig.h"

int main() {
    // Create the tc_config struct containing every key-value inside your
    // config file.
    tc_config *config;
    int err = tc_load_config(&config, "tiny.conf");
    // Check for errors
    if (err) {
        printf("Error loading config\n");
        return 1;
    }
    
    // You can now retrieve values from the file,
    // you can bring in your own methods of conversion.
    // More unsafe way:
    char *player_power = tc_get_value(config, "player_power");
    printf("player_power: %i\n", atoi(player_power));

    // Safer:
    char *player_int = tc_get_value(config, "base_attack");
    if (player_int != NULL) {
        printf("base_attack: %f\n", atof(player_int));
    }
    
    // You can create new values if they don't exist or just update already existing ones:
    tc_set_value(config, "player_dex", "14");
    
    // Now if you need you can persist the changes back to a file,
    // don't forget to check if the file already exists.
    tc_save_to_file(config, "modified.conf");
    
    // Don't forget to free the config structure.
    tc_free(config);

    return 0;
}
```

You can manually define the initial config allocation size and grow size using you build system.
A CMake example:
```cmake
add_compile_definitions(TC_CONFIG_DEFAULT_SIZE=10)
add_compile_definitions(TC_CONFIG_DEFAULT_GROW_SIZE=4)
```

This will overwrite the default values provided by tinyconfig.

### Parsing rules
Values-Keys:
- Values can be CamelCase or snake_case
- The only acceptable special character inside names are underlines _
- Numbers are accepted for keys too
- Numbers can contain a . to indicate a floating point number

Comments:
- Comments are delimited with a #
- Once the program finds a # it reads until the line or file end and ignores everything

Whitespaces:
- All whitespaces are ignored