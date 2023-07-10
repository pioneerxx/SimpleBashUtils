#!bin/bash


COUNTER=0
LEAK_SUCCESS=0
LEAK_FAIL=0

declare -a flags=(
"-b"
"-e"
"-n"
"-s"
"-t"
"-v"
)

declare -a files=(
"s21_cattest1.txt"
"s21_cattest2.txt"
"s21_cattest3.txt"
)

declare -a commline=(
"OPT FILE"
)

declare -a exotic=(
"-b s21_cat.txt -s"
"-benstv nope.txt"
)


function testing()
{
    str=$(echo $@ | sed "s/OPT/$options/")
    str=$(echo $str | sed -e "s/FILE/$file/g")
    leaks -quiet -atExit -- ./s21_cat $str > s21_cat_leakstest.log
    leak=$(grep '0 leaks for 0 total leaked bytes' s21_cat_leakstest.log)
    (( COUNTER++ ))
    if [[ $leak != "" ]]
    then
        (( LEAK_SUCCESS++ ))
        echo "cat $str\n\033[0;32mLEAK SUCCESS \033[0;31m$LEAK_FAIL|\033[0;32m$LEAK_SUCCESS|\033[1;33m$COUNTER\033[0m"
    else
        (( LEAK_FAIL++ ))
        echo "cat $str\n\033[0;31mLEAK FAIL \033[0;31m$LEAK_FAIL|\033[0;32m$LEAK_SUCCESS|\033[1;33m$COUNTER\033[0m"
    fi
    rm s21_cat_leakstest.log
}



for i in "${exotic[@]}"
do
    testing $i
done

for opt1 in "${flags[@]}"
do
    for opt2 in "${flags[@]}"
    do
        for opt3 in "${flags[@]}"
        do
            for opt4 in "${flags[@]}"
            do
                if [ $opt1 != $opt2 ] && [ $opt1 != $opt3 ] \
                && [ $opt1 != $opt4 ] && [ $opt2 != $opt3 ] \
                && [ $opt2 != $opt4 ] && [ $opt3 != $opt4 ]
                then
                    for file in "${files[@]}"
                    do
                        options="$opt1 $opt2 $opt3 $opt4"
                        testing $commline
                    done
                fi
            done
        done
    done
done

echo "\n\033[0;31mFAIL: $LEAK_FAIL"
echo " \033[0m| \033[0;32mSUCCESS: $LEAK_SUCCESS"
echo " \033[0m| \033[1;33mALL: $COUNTER"
