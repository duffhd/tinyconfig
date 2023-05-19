### tiny config

Tiny config is a simple key-value pair configuration file that cant hold
numbers (decimal or floating point) and strings.

A simple config example:
```txt
# Line comments
project_name=tinyconfig
decimal=54
floating_point=3.14
42=key_number # Numbers can be used as keys
# You can use whitepaces before and after equals =
example = 34
```

### How to use
Tiny config is a simple header and source file that can be called from any
build system easily, you can pull this repository or simply copy the source file
`tinyconfig.c` and include the header `tinyconfig.h`.

Inside your `C` or `C++` code:
```c
#include "tinyconfig.h"

int main() {
    // Create the tc_config struct containing every key-value inside your
    // config file.
    tc_config *config;
    int ok = tc_load_config(&config, "tiny.conf");
    // Check for errors
    if (!ok) {
        printf("Error loading config\n");
        return 1;
    }
    
    // Load any value by using the provided functions, simply
    // pass the key name you want to the correct function to get the type
    // you want.
    int value_from_config;
    // Always check for errors
    if (!tc_get_int(config, "decimal", &value_from_config)) {
        printf("Error parsing value\n");
        return 1;              
    }
    
    // Now you can safely use your values
    
    
    // Don't forget to free the config structure.
    tc_free(config);

    return 0;
}
```

### Parsing rules
Values-Keys:
- Values can be CamelCase or snake_case
- The only acceptable special character inside names are underlines _
- Numbers are accepted for names too, they can contain a . to indicate a floating point number

Comments:
- Comments are delimited with a #
- Once the program finds a # it reads until the line or file end and ignores everything

Whitespaces:
- All whitespaces are ignored