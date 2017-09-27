printf "Time testing...\n"
printf "C++:"
g++ cpptest.cpp -o temp
./temp
rm temp

printf "Java:"
javac javatest.java
java javatest
rm javatest.class

printf "lns:"
lns lnstest.lns

printf "Python:"
python pythontest.py