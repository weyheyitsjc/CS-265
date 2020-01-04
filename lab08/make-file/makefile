cc = gcc
JAVAC = javac
JRE = java
PAGER = less

.PHONY : clean run-py run-c read run-java test.out test-java

foo.o : foo.c foo.h 
	$(cc) -c foo.c

main.o : main.c foo.h
	$(cc) -c main.c

foo : foo.o main.o
	$(cc) -ofoo main.o foo.o

run-c : foo
	./foo

run-py : foo.py
	python foo.py

read : 
	if [ -e README ] ; then \
		$(PAGER) README ;\
	fi

foo.class : foo.java
	$(JAVAC) $<

run-java : foo.class
	$(JRE) foo

test.out : 
	$(JRE) foo > test.out

test-java : test.out expected.out
	-@if diff test.out expected.out &> /dev/null ; then \
		echo "Passed!" ;\
	else \
		echo "Not the same!" ;\
	fi

clean : 
	rm *.o 

cleanall : clean
	rm foo test.out foo.class