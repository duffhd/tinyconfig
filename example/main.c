#include <stdio.h>
#include <stdlib.h>

#include <tinyconfig.h>

int main() {
    tc_config *config;
    int err = tc_load_config(&config, "tiny.conf");
    if (err) {
        printf("Error loading config\n");
        return 1;
    }

    // More unsafe way
    char *player_power = tc_get_value(config, "player_power");
    printf("player_power: %i\n", atoi(player_power));

    // Safer
    char *player_int = tc_get_value(config, "base_attack");
    if (player_int != NULL) {
        printf("base_attack: %f\n", atof(player_int));
    }

    // Set a value to a certain key, if the value is does not exist it'll be created.
    char *new_player_power = tc_set_value(config, "player_power", "330");
    printf("modified player_power: %i\n", atoi(new_player_power));

    // Create a value that doesn't exist.
    tc_set_value(config, "player_dex", "50");
    char *player_dex = tc_get_value(config, "player_dex");
    printf("new value player_dex: %i", atoi(player_dex));

    // Save the modifications back to a file.
    tc_save_to_file(config, "modified.conf");

    tc_free(config);

    return 0;
}