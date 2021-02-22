all: vtkWrapFreeMat

CFLAGS = -I/usr/include/vtk

vtkWrapFreeMat: vtkWrapFreeMat.o vtkParse.tab.o
	$(CC) -g -o $@ $^

clean:
	rm -rf *.o vtkWrapFreeMat
