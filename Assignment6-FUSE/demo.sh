#! /bin/bash

SRC_DIR="src"
TESTCASE_DIR="testcase"
OUTPUT_DIR="output"
ANSWER_DIR="answer"
PROGRAM_PATH="$(readlink -f "$1" || echo "$1")"

rm -f ${OUTPUT_DIR}/* test.tar

cp basic/test.tar test.tar

${PROGRAM_PATH} -f tarfs > /dev/null &
PROGRAM_PID=$!
sleep 1
for i in $( seq 1 4 ); do
  echo "[1;34m===== basic case ${i} =====[m"
  bash "./${TESTCASE_DIR}/$i.txt" 2>&1 | sed -e 's/sean/nctuos/g' -e 's#/root/Repo/NCTU-110A-OS/Assignment6-FUSE#/home/nctuos/Documents#' > "${OUTPUT_DIR}/${i}.txt"
  diff -w "${OUTPUT_DIR}/${i}.txt" "${ANSWER_DIR}/${i}.txt" > /dev/null
  if [ $? -eq 0 ]; then
    echo "Your answer is [0;32mcorrect[m"
    correct_cases="${correct_cases} ${i}"
  else
    echo "Your answer is [0;31mwrong[m"
    wrong_cases="${wrong_cases} ${i}"
  fi
done

kill ${PROGRAM_PID}
rm -f test.tar

cp bonus/test.tar test.tar

${PROGRAM_PATH} -f tarfs > /dev/null &
PROGRAM_PID=$!
sleep 1
for i in $( seq 5 6 ); do
  echo "[1;35m===== bonus case ${i} =====[m"
  bash "./${TESTCASE_DIR}/$i.txt" 2>&1 | sed 's/sean/nctuos/g' > "${OUTPUT_DIR}/${i}.txt"
  diff -w "${OUTPUT_DIR}/${i}.txt" "${ANSWER_DIR}/${i}.txt" > /dev/null
  if [ $? -eq 0 ]; then
    echo "Your answer is [0;32mcorrect[m"
    correct_cases="${correct_cases} ${i}"
  else
    echo "Your answer is [0;31mwrong[m"
    wrong_cases="${wrong_cases} ${i}"
  fi
done
kill ${PROGRAM_PID}
echo "[1;33m======= Summary =======[m"
[ -n "${correct_cases}" ] && echo "[0;32m[Correct][m:${correct_cases}"
[ -n "${wrong_cases}" ] && echo "[0;31m[ Wrong ][m:${wrong_cases}"
