# Dynamic Version

## General info
The program tries to dynamically load (using the dlfcn library function) the previously prepared shared library and import the shared function provided by it. 

## Program is created with:
* C

## Setup
To run program:
### No switch
```
$ make
$ ./outprogname
```
### Switch h
```
$ make
$ ./outprogname -h
```
### Switch g
```
$ make
$ ./outprogname -g
```
### Switch h and g
```
$ make
$ ./outprogname -h -g
```

## Terminal display
<img src ="Terminal_Dynamic.PNG" widith="400" height="300">
