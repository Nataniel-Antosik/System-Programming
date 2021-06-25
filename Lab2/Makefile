cc = gcc
target1 = second.main
target2 = second.lib

outprogname : $(target2).o $(target1).o
	${cc} -o outprogname $(target1).o $(target2).o

$(target2).o : $(target2).c
	${cc} -c $(target2).c

$(target1).o : $(target1).c
	${cc} -c $(target1).c

clean:
	$(RM) outprogname
	$(RM) $(target1).o
	$(RM) $(target2).o
