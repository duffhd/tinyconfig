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