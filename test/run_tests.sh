#!/usr/bin/env bash

# The tests to run
TESTS=(test_init test_clear test_copy test_push_back \
    test_push_front test_get test_insert test_remove \
    test_iteration test_sort test_split)

# The number of succeeded tests
SUCCEEDED=0
MISSING=0

# Run all the tests
for test in "${TESTS[@]}"
do
    printf "\n### %s ###\n" "$test"

    if [ -e "bin/$test" ]; then
        bin/$test

        if (($? == 0)); then
            SUCCEEDED=$((SUCCEEDED + 1))
        fi
    else
        echo "### Test ${test} not found! ###"
        MISSING=$((MISSING + 1))
    fi
done

COMPLETEDMSG="### Tests completed! ###"
printf "\n%s\n\tPassed: %d\n\tFailed: %d\n\tMissing: %d\n" \
    "$COMPLETEDMSG" "$SUCCEEDED" "$((${#TESTS[@]}-$SUCCEEDED-$MISSING))" \
    "$MISSING"
