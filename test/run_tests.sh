#!/bin/bash
echo "Running tests..."

gcc test/test_main.c -o test_runner
./test_runner
