cc = gcc
w = -pthread
target1 = main.c
target2 = timer.c

outprogname : $(target1) $(target2)
	${cc} ${w} $(target1) $(target2) -o outprogname

clean:
	$(RM) outprogname
