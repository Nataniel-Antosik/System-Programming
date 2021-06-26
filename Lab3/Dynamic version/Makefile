cc = gcc
main = shared.main
lib = shared.lib
sh = -shared

outprogname : $(main).c dynamic.so
	${cc} $(main).c -o outprogname -ldl

dynamic.so : $(lib).c
	${cc} ${sh} $(lib).c -o dynamic.so 

clean:
	$(RM) outprogname
	$(RM) dynamic.so 
