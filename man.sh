#!/bin/bash

for N in {1..1000}
do
	./client 9001 developer.txt &
wait
done
