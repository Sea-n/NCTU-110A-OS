#!/usr/bin/env bash
cd "$(dirname "$1")" || exit 1

if [[ $# -ne 1 ]]; then
	echo "Usage: $1 ./your-tar-fuse"
	exit 1
fi

WD="$(pwd)"
TESTCASE_DIR="testcase"
OUTPUT_DIR="output"
ANSWER_DIR="answer"
PROGRAM_PATH="$(readlink -f "$1" || echo "$1")"

# Colors
RED=$'\033[0;31m'
GREEN=$'\033[0;32m'
YELLOW=$'\033[1;33m'
BLUE=$'\033[1;34m'
PURPLE=$'\033[1;35m'
NC=$'\033[m'

rm -f ${OUTPUT_DIR}/* test.tar


### Basic ###
cp basic/test.tar test.tar

${PROGRAM_PATH} -f tarfs > /dev/null &
PROGRAM_PID=$!
sleep 1
USER="$(stat -c '%U' tarfs/* | grep -v root | head -n1)"
for i in {1..4}; do
	echo "${BLUE}===== basic case $i =====${NC}"

	bash "./${TESTCASE_DIR}/$i.txt" 2>&1 \
		| sed -e "s/ $USER / nctuos /g" \
		| sed -e "s#$WD#/home/nctuos/Documents#" \
		> "${OUTPUT_DIR}/${i}.txt"

	diff -w "${OUTPUT_DIR}/${i}.txt" "${ANSWER_DIR}/${i}.txt" > /dev/null
	if [ $? -eq 0 ]; then
		echo "Your answer is ${GREEN}correct${NC}"
		correct_cases="${correct_cases} ${i}"
	else
		echo "Your answer is ${RED}wrong${NC}"
		wrong_cases="${wrong_cases} ${i}"
	fi
done

kill ${PROGRAM_PID}
rm -f test.tar


### Bonus ###
cp bonus/test.tar test.tar

${PROGRAM_PATH} -f tarfs > /dev/null &
PROGRAM_PID=$!
sleep 1
USER="$(stat -c '%U' tarfs/* | grep -v root | head -n1)"
for i in {5..6}; do
	echo "${PURPLE}===== bonus case $i =====${NC}"

	bash "./$TESTCASE_DIR/$i.txt" 2>&1 \
		| sed -e "s/ $USER / nctuos /g" \
		> "$OUTPUT_DIR/$i.txt"

	diff -w "$OUTPUT_DIR/$i.txt" "$ANSWER_DIR/$i.txt" > /dev/null
	if [ $? -eq 0 ]; then
		echo "Your answer is ${GREEN}correct${NC}"
		correct_cases="${correct_cases} ${i}"
	else
		echo "Your answer is ${RED}wrong${NC}"
		wrong_cases="${wrong_cases} ${i}"
	fi
done

kill ${PROGRAM_PID}
rm -f test.tar


### Summary ###
echo "${YELLOW}======= Summary =======${NC}"

if [ -n "${correct_cases}" ]; then
	echo "${GREEN}[Correct]${NC}:${correct_cases}"
fi

if [ -n "${wrong_cases}" ]; then
	echo "${RED}[ Wrong ]${NC}:${wrong_cases}"
fi
