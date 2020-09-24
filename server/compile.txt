compile:

$ gcc -c -Wall -Werror -fpic cJSON.c cJSON.h
$ gcc -shared -o libcjson.so cJson.o
$ g++ -Wall -o server tcpServer.cpp libcjson.so
$ export LD_LIBRARY_PATH={your_path}:$LD_LIBRARY_PATH


