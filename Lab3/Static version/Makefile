cc = gcc
target1 = static.main
target2 = static.lib

outprogname : $(target2).o $(target1).c
	${cc} $(target1).c -o outprogname -L. -lstatic

$(target2).o : $(target2).c
	${cc} -c $(target2).c
	ar cr libstatic.a $(target2).o
	
clean:
	$(RM) outprogname
	$(RM) $(target2).o
	$(RM) libstatic.a
