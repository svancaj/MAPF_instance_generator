#!/bin/bash

#for instance in maps/*10* #60 agents
#for instance in maps/*20* #200 agents
for instance in maps/* #300 agents
do
	./generator $instance
done