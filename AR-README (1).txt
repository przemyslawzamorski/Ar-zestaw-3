10. Right click on the project in "Solution Explorer", mpiProject in the above image, and select "Properties". Highlight and expand "Configuration Properties" on the left.
* Select "All Configurations" from the "Configuration:" drop-down box at the upper left.
* Select "General" under the "C/C++" menu on the left. In "Additional Include Directories" on the right, put the directory in which you installed the SDK's include files to in Step 2.
In the default case, this is: ""C:\Program Files (x86)\MPICH2\lib"
* Select "Advanced" under the same "C/C++" menu on the left. In "Compile As" on the right side, select "Compile as C Code (/TC)" if your program is just C and not C++.
* Select "General" under the "Linker" menu on the left. In "Additional Library Directories" on the right, put the directory in which you installed the SDK's lib files to in Step 2. 
In the default case, this is: ""C:\Program Files (x86)\MPICH2\lib"
* Select "Input" under the same "Linker" menu on the left. In "Additional Dependencies" on the right side, type in "mpi.lib", without the quotes, in order to use the MPI libraries.


Dodanie do zmiennych œrodowiskowych “C:\Program Files (x86)\MPICH2\bin”

Z konsoli wejœcie do folderu z plikiem wykonywalnym

Uruchom komendê „mpiexec.exe -n ILOSC_PROCESOW NAZWA_PROGRAMU.exe”
