testing:
    addI X29,x15,#4095
    addI X29,x15,#4095
    .test1:
        addI X29,x15,#4095
testing52:
    .test:
        addi x29,x15,#14
        jal x2,.test1
        addi x29,x15,#14
    .test1:
        and x15,x23,x12
        jal x5,.test
testing5:
    add x26,x13,x12
    add x26,x13,x12
testing2:
testing23:
testing4:
    addi x29,X15,#14
    jal x7,testing23
    jal X7,#476
    jalr X3,X7,testing5
    jalr x7,x3,testing2
