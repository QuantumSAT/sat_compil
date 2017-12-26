#!/bin/bash

valgrind --tool=memcheck --leak-check=yes --show-reachable=yes qSat-debug route.tcl >& debug_log
