# flint-vm

## Build and Run Tests

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
./test/flint-vm-test
```

If the build is successful, all test cases should pass.

### Notes

Test bytecode files are stored in `test/byte_code` and are automatically copied
to the build directory during configuration.