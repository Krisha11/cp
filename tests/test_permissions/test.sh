make --directory=$1

echo "Try copy file without permissions"
touch src
chmod 555 .

$1/main src nonExistDst
code=`echo $?`
if [ $code != 1 ]; then
    exit 1
fi
chmod 777 .
rm src


exit 0