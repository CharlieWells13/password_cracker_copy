echo "Searching hash $3 for password $4, num_threads=$1, keysize=$2" 

time ./crack $1 $2 $3      #Run to produce output for student

./crack $1 $2 $3 > output  #Run again to capture output

#Check to see if output has more than 50 lines, and fail if true
if [[ $(wc -l <output) -ge 50 ]]; then
  echo -e "\033[0;31mAutograder failure: Too many output lines \033[0m"
  exit -1
fi

#Test output for target string
cat output | grep -q $4
if [[ $? -ne 0 ]]; then
  echo -e "\033[0;31mAutograder could not find password in output\033[0m"
  exit -1 #Returns failure status code and causes github workflow to fail
fi

echo -e "\033[0;32mAutograder found password $4\033[0m"
echo ""

