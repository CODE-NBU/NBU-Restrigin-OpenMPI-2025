clear
astyle "*.cpp" --indent=force-tab --style=java / -A2 --recursive
astyle "*.c" --indent=force-tab --style=java / -A2 --recursive
astyle "*.h" --indent=force-tab --style=java / -A2 --recursive
find . -name "*.orig" -type f -delete
rm restrigin.exe
mpicxx -o restrigin.exe restrigin.cpp
nohup nice mpirun -np 8 ./restrigin.exe $1
