main_dir=$1
test_dir=$2
dst=$test_dir"dst"
src=$test_dir"src"

make --directory=$1

echo "Make simple copy"

touch $src
echo "tst" > $src

$main_dir/main $src $dst
code=`echo $?`
files=`ls $test_dir | wc -l`
data=`cat $dst`

if [ $code != 0 ] || [ $files != 3 ] || [ $data != "tst" ]; then
    exit 1
fi

rm $src
rm $dst


exit 0