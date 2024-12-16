segment code
    mov rsp, FFFFh
    mov ss, stack
    mov cs, code
    mov ds, data

    mov es, ds
    mova msg, 5b

    movr ax, msg
ends

segment data
    msg0: "h"
    msg: "hello world"
ends

segment stack
ends