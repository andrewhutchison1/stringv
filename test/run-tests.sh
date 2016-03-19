#!/usr/bin/env bash

# The tests to run
TESTS=(test_init test_clear test_copy test_push_back \
    test_push_front test_get test_insert test_remove \
    test_iteration)

# The number of succeeded tests
SUCCEEDED=0

# Run all the tests
for i in "${TESTS[@]}"
do
    printf "\n### %s ###\n" "$i"
    bin/$i
    if (($? == 0)); then
        SUCCEEDED=$((SUCCEEDED + 1))
    fi
done

printf "\n### TESTS COMPLETED: %d/%d PASSED ###\n" "$SUCCEEDED" \
    ${#TESTS[@]}
