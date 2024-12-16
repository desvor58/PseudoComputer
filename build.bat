rem compiler 
    rem comp [src/deasm/compiler.cpp]
    clang++ src/deasm/compiler.cpp -o src/deasm/comp.exe

rem pseudo computer
    rem PC [src/pseudo_coputer.d]
    dmd src/pseudo_computer.d src/types.d -of= pc.exe
    rm pc.obj