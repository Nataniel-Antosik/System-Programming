cc = gcc
w = -pthread
target1 = Manager.c
target2 = Worker.c
cry = -lrt
crypt = -lcrypt

out : $(target1) $(target2) $(target3)
	${cc} ${w} $(target1) -o out1 ${cry}
	${cc} ${w} $(target2) -o out2 ${cry} ${crypt}
	
clean:
	$(RM) out1
	$(RM) out2
