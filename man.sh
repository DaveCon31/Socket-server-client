#!/bin/bash

for N in {1..15}
do
	if ["$N" -lt "5" && "$N" -lt "10"]
	then
		./client 9000 developer.txt
	else
		./client 9000 developer.txt12313
	fi
done
wait
