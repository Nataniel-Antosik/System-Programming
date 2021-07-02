cc = gcc
w = -pthread
target1 = main.c
target2 = shortcut.c

cry = -lcrypt

out1 : $(target1) $(target2)
	${cc} ${w} $(target1) -o out1 ${cry}
	${cc} ${w} $(target2) -o out2 ${cry}
	
clean:
	$(RM) out1
	$(RM) out2

