cmake_minimum_required(VERSION 3.30)
project(TestClion C)

set(CMAKE_C_STANDARD 11)

add_executable(TestClion main.c
        dataframe.c
        dataframe.h
        import.h
        wordhash.c
        utf8_vn.c)