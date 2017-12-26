#!/bin/bash

valgrind --tool=memcheck --leak-check=yes qSat-debug generate.tcl >& debug_log
