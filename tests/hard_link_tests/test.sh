main_dir=$1
test_dir=$2

make --directory=$1

echo "Make Hard Link"

touch $test_dir"src"
touch $test_dir"sameEmptyAsSrc"

$main_dir/main $test_dir"src" $test_dir"dst"
code=`echo $?`
files=`ls $test_dir | wc -l`
hard_links=`find . -samefile $test_dir"src" | wc -l`

if [ $code != 0 ] || [ $files != 4 ] || [ $hard_links != 2 ]; then
    exit 1
fi

rm $test_dir"src"
rm $test_dir"sameEmptyAsSrc"
rm $test_dir"dst"
