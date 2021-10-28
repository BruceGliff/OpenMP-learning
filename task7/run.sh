#!/bin/bash
./run.out &> out.txt && cat out.txt | grep time
