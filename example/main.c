#include <stdio.h>

#include <tinyconfig.h>

int main() {
    tc_config *config;
    int err = tc_load_config(&config, "tiny.conf");
    if (err) {
        printf("Error loading config\n");
        return 1;
    }

    int pp;
    tc_get_int(config, "player_power", &pp);
    printf("%i\n", pp);

    int pi;
    tc_get_int(config, "player_intelligence", &pi);
    printf("%i\n", pi);

    float ba;
    tc_get_float(config, "base_attack", &ba);

    char *pn = tc_get_char(config, "player_name");
    if (pn != NULL) {
        printf("%s\n", pn);
    }

    tc_free(config);

    return 0;
}