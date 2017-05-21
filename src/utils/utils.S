    .globl getApplicationEndAddr
getApplicationEndAddr:
    lis r3, __CODE_END@h
    ori r3, r3, __CODE_END@l
    blr

     .globl SC0x25_KernelCopyData
SC0x25_KernelCopyData:
    li r0, 0x2500
    sc
    blr
