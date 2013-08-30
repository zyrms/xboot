
	.text

    .global CoDisableL2Cache
    .type CoDisableL2Cache, %function
    .align 4
CoDisableL2Cache:
	MRC     p15,0,r0,c1,c0,1
	BIC     r0,r0,#(1<<1)
	MCR     p15,0,r0,c1,c0,1
	MOV     pc,lr


    .global CoInvalidateDCacheIndex
    .type CoInvalidateDCacheIndex, %function
    .align 4
CoInvalidateDCacheIndex:
	;/* r0 = index */
	MCR     p15,0,r0,c7,c6,2
	MOV     pc,lr

    .global CoSetL2CacheAuxCrtlReg
    .type CoSetL2CacheAuxCrtlReg, %function
    .align 4
CoSetL2CacheAuxCrtlReg:
    ;/* r0 = uRegValue */
    MCR     p15,1,r0,c9,c0,2
    MOV     pc,lr

    .global CoEnableL2Cache
    .type CoEnableL2Cache, %function
    .align 4
CoEnableL2Cache:
    MRC     p15,0,r0,c1,c0,1
    ORR     r0,r0,#(1<<1)
    MCR     p15,0,r0,c1,c0,1
    MOV     pc,lr

    .global CoInvalidateICache
    .type CoInvalidateICache, %function
    .align 4
CoInvalidateICache:
    MOV     r0,#0
    MCR     p15,0,r0,c7,c5,0
    MOV     pc,lr

    .global CoInvalidateDTlb
    .type CoInvalidateDTlb, %function
    .align 4
CoInvalidateDTlb:
	MOV     r0,#0
	MCR     p15,0,r0,c8,c6,0
	MOV     pc,lr

    .global CoDisableICache
    .type CoDisableICache, %function
    .align 4
CoDisableICache:
	MRC     p15,0,r0,c1,c0,0
	BIC     r0,r0,#(1<<12)
	MCR     p15,0,r0,c1,c0,0
	MOV     pc,lr

    .global CoDisableDCache
    .type CoDisableDCache, %function
    .align 4
CoDisableDCache:
	MRC     p15,0,r0,c1,c0,0
	BIC     r0,r0,#(1<<2)
	MCR     p15,0,r0,c1,c0,0
	MOV     pc,lr

    .global CoDisableMmu
    .type CoDisableMmu, %function
    .align 4
CoDisableMmu:
	MRC     p15,0,r0,c1,c0,0
	BIC     r0,r0,#(1<<0)
	MCR     p15,0,r0,c1,c0,0
	MOV     pc,lr

    .global CoSetTTBase
    .type CoSetTTBase, %function
    .align 4
CoSetTTBase:
	;/* r0 = base */
	MCR     p15,0,r0,c2,c0,0
	MOV     pc,lr

    .global CoSetDomain
    .type CoSetDomain, %function
    .align 4
CoSetDomain:
	;/* r0 = domain */
	MCR     p15,0,r0,c3,c0,0
	MOV     pc,lr

    .global CoEnableAlignFault
    .type CoEnableAlignFault, %function
    .align 4
CoEnableAlignFault:
	MRC     p15,0,r0,c1,c0,0
	ORR	r0,r0,#(1<<1)
	MCR     p15,0,r0,c1,c0,0
	MOV     pc,lr

    .global CoEnableMmu
    .type CoEnableMmu, %function
    .align 4
CoEnableMmu:
	MRC     p15,0,r0,c1,c0,0
	ORR     r0,r0,#(1<<0)
	MCR     p15,0,r0,c1,c0,0
	MOV     pc,lr

    .global CoEnableICache
    .type CoEnableICache, %function
    .align 4
CoEnableICache:
	MRC     p15,0,r0,c1,c0,0
	ORR     r0,r0,#(1<<12)
	MCR     p15,0,r0,c1,c0,0
	MOV     pc,lr

    .global CoEnableDCache
    .type CoEnableDCache, %function
    .align 4
CoEnableDCache:
	MRC     p15,0,r0,c1,c0,0
	ORR     r0,r0,#(1<<2)
	MCR     p15,0,r0,c1,c0,0
	MOV     pc,lr

    .global CoEnableBranchPrediction
    .type CoEnableBranchPrediction, %function
    .align 4
CoEnableBranchPrediction:
	MRC     p15,0,r0,c1,c0,0
	ORR     r0,r0,#(1<<11)
	MCR     p15,0,r0,c1,c0,0
	MOV     pc,lr
