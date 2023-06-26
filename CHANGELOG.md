## 1.1.2
- Fixed freeing the new config buffer on `tc_save_to_file` to work on every compiler.

## 1.1.1
- Fixed file opening on systems other than Windows (use plain fopen)
- Fixed `tc_set_value` not incrementing the config struct `buffer_size` if the setted value was associated with a new key.
- Always malloc the buffer to save the new config to a file on `tc_save_to_file`.
- Stop using fseeko64 and fteelo64 on x64 \_\_unix\_\_

## 1.1.0
- Add the possibility to override the #define values: `TC_CONFIG_DEFAULT_SIZE` and `TC_CONFIG_DEFAULT_GROW_SIZE` using the build system.
- Add negative number parsing.

## 1.0.0
- Changed how to get values from the config file, removed get string, int and float.
- Added `tc_get_value` to get the string representation of the value you want, you'll need to provide the means to convert the variable.
- Added `tc_set_value` so that you can change the config on runtime and later save it to some file.
- Added `tc_save_to_file` to make any changes for the config during runtime persist on disk.
- `tc_pair` struct is now private.
- Updated README and example to reflect the new changes.

## 0.1.2
- Added tab character check on parsing.

## 0.1.1
- Fixed MSVC build.

## 0.1.0
- Initial release
- Simple key-value parsing
- Support to get floats, ints and characters