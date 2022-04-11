dirs=`ls  -1 tests`

for dir in $dirs
do
    if [ $dir != "test.sh" ]; then
        "tests/"$dir"/test.sh" $PWD $PWD"/tests/"$dir"/"
        if [ $? != 0 ]; then
            echo "Problems in "$dir"/test.sh"
            exit 1
        fi
    fi
done

echo "ALL TESTS PASSED"

exit 0

