#!/bin/bash

for N in {1..15}
do
	./client 9000 developer.txt1231
	if [N>5] && [N<10]
	then
		./client 9000 developer.txt
	fi
done
wait
