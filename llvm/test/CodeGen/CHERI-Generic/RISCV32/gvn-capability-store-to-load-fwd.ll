; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; DO NOT EDIT -- This file was generated from test/CodeGen/CHERI-Generic/Inputs/gvn-capability-store-to-load-fwd.ll
; Check that  GVN does not attempt to read capability fields that it can't get the bits for
; This is https://github.com/CTSRD-CHERI/llvm-project/issues/385
; GVN was previously doing the following invalid transformation (Note the shift by 64 of the ptrtoint result)
;   %ai = alloca %suspicious_type, align 16, addrspace(200)
;   %tmp33 = bitcast %2 addrspace(200)* %ai to i8 addrspace(200)* addrspace(200)*
;   %tmp34 = load i8 addrspace(200)*, i8 addrspace(200)* addrspace(200)* %tmp33, align 16
;   %0 = ptrtoint i8 addrspace(200)* %tmp34 to i64 ; INCORRECT transformation (does not transfer all bits)
;   %1 = lshr i64 %0, 64   ; Shift right by 64 to get field #2
;   %2 = trunc i64 %1 to i32 ; truncate to drop the high bits
; It assumed it could get bits 32-63 by doing a ptrtoint, but on CHERI-MIPS ptrtoint returns bits 65-127

; RUN: %riscv32_cheri_purecap_opt -S -memdep -basicaa -gvn -o - %s | FileCheck %s
; RUN: %riscv32_cheri_purecap_opt -S -memdep -basicaa -gvn -o - %s | %riscv32_cheri_purecap_llc -O0 -o - | FileCheck %s --check-prefix=ASM

; Check in the baseline (broken test now) to show the diff in the fixed commit
; REQUIRES: bug_385_fixed


target datalayout = "e-m:e-pf200:64:64:64:32-p:32:32-i64:64-n32-S128-A200-P200-G200"

%0 = type { i8, i8, [14 x i8] }
%struct.addrinfo = type { i32, i32, i32, i32, i32, i8 addrspace(200)*, %0 addrspace(200)*, %struct.addrinfo addrspace(200)* }


define i32 @first_i32_store_to_load_fwd(i8 addrspace(200)* %arg) local_unnamed_addr addrspace(200) nounwind {
; ASM-LABEL: first_i32_store_to_load_fwd:
; ASM:       # %bb.0: # %bb
; ASM-NEXT:    cincoffset csp, csp, -48
; ASM-NEXT:    csc ca0, 0(csp)
; ASM-NEXT:    clw a0, 0(csp)
; ASM-NEXT:    cincoffset csp, csp, 48
; ASM-NEXT:    cret
; CHECK-LABEL: define {{[^@]+}}@first_i32_store_to_load_fwd
; CHECK-SAME: (i8 addrspace(200)* [[ARG:%.*]]) local_unnamed_addr addrspace(200)
; CHECK-NEXT:  bb:
; CHECK-NEXT:    [[STACKVAL:%.*]] = alloca [[STRUCT_ADDRINFO:%.*]], align 8, addrspace(200)
; CHECK-NEXT:    [[FIELD:%.*]] = getelementptr inbounds [[STRUCT_ADDRINFO]], [[STRUCT_ADDRINFO]] addrspace(200)* [[STACKVAL]], i64 0, i32 0
; CHECK-NEXT:    [[AS_CAP:%.*]] = bitcast [[STRUCT_ADDRINFO]] addrspace(200)* [[STACKVAL]] to i8 addrspace(200)* addrspace(200)*
; CHECK-NEXT:    store i8 addrspace(200)* [[ARG]], i8 addrspace(200)* addrspace(200)* [[AS_CAP]], align 8
; CHECK-NEXT:    [[RESULT:%.*]] = load i32, i32 addrspace(200)* [[FIELD]], align 4
; CHECK-NEXT:    ret i32 [[RESULT]]
;
bb:
  %stackval = alloca %struct.addrinfo, align 8, addrspace(200)
  %field = getelementptr inbounds %struct.addrinfo, %struct.addrinfo addrspace(200)* %stackval, i64 0, i32 0
  %as_cap = bitcast %struct.addrinfo addrspace(200)* %stackval to i8 addrspace(200)* addrspace(200)*
  store i8 addrspace(200)* %arg, i8 addrspace(200)* addrspace(200)* %as_cap, align 8
  %result = load i32, i32 addrspace(200)* %field, align 4
  ret i32 %result
}

define i32 @second_i32_store_to_load_fwd(i8 addrspace(200)* %arg) local_unnamed_addr addrspace(200) nounwind {
; ASM-LABEL: second_i32_store_to_load_fwd:
; ASM:       # %bb.0: # %bb
; ASM-NEXT:    cincoffset csp, csp, -48
; ASM-NEXT:    csc ca0, 0(csp)
; ASM-NEXT:    clw a0, 4(csp)
; ASM-NEXT:    cincoffset csp, csp, 48
; ASM-NEXT:    cret
; CHECK-LABEL: define {{[^@]+}}@second_i32_store_to_load_fwd
; CHECK-SAME: (i8 addrspace(200)* [[ARG:%.*]]) local_unnamed_addr addrspace(200)
; CHECK-NEXT:  bb:
; CHECK-NEXT:    [[STACKVAL:%.*]] = alloca [[STRUCT_ADDRINFO:%.*]], align 8, addrspace(200)
; CHECK-NEXT:    [[FIELD:%.*]] = getelementptr inbounds [[STRUCT_ADDRINFO]], [[STRUCT_ADDRINFO]] addrspace(200)* [[STACKVAL]], i64 0, i32 1
; CHECK-NEXT:    [[AS_CAP:%.*]] = bitcast [[STRUCT_ADDRINFO]] addrspace(200)* [[STACKVAL]] to i8 addrspace(200)* addrspace(200)*
; CHECK-NEXT:    store i8 addrspace(200)* [[ARG]], i8 addrspace(200)* addrspace(200)* [[AS_CAP]], align 8
; CHECK-NEXT:    [[RESULT:%.*]] = load i32, i32 addrspace(200)* [[FIELD]], align 4
; CHECK-NEXT:    ret i32 [[RESULT]]
;
bb:
  %stackval = alloca %struct.addrinfo, align 8, addrspace(200)
  %field = getelementptr inbounds %struct.addrinfo, %struct.addrinfo addrspace(200)* %stackval, i64 0, i32 1
  %as_cap = bitcast %struct.addrinfo addrspace(200)* %stackval to i8 addrspace(200)* addrspace(200)*
  store i8 addrspace(200)* %arg, i8 addrspace(200)* addrspace(200)* %as_cap, align 8
  %result = load i32, i32 addrspace(200)* %field, align 4
  ret i32 %result
}

define i32 @third_i32_store_to_load_fwd(i8 addrspace(200)* %arg) local_unnamed_addr addrspace(200) nounwind {
; ASM-LABEL: third_i32_store_to_load_fwd:
; ASM:       # %bb.0: # %bb
; ASM-NEXT:    cincoffset csp, csp, -48
; ASM-NEXT:    csc ca0, 0(csp)
; ASM-NEXT:    # implicit-def: $x10
; ASM-NEXT:    cincoffset csp, csp, 48
; ASM-NEXT:    cret
; CHECK-LABEL: define {{[^@]+}}@third_i32_store_to_load_fwd
; CHECK-SAME: (i8 addrspace(200)* [[ARG:%.*]]) local_unnamed_addr addrspace(200)
; CHECK-NEXT:  bb:
; CHECK-NEXT:    [[STACKVAL:%.*]] = alloca [[STRUCT_ADDRINFO:%.*]], align 8, addrspace(200)
; CHECK-NEXT:    [[FIELD:%.*]] = getelementptr inbounds [[STRUCT_ADDRINFO]], [[STRUCT_ADDRINFO]] addrspace(200)* [[STACKVAL]], i64 0, i32 2
; CHECK-NEXT:    [[AS_CAP:%.*]] = bitcast [[STRUCT_ADDRINFO]] addrspace(200)* [[STACKVAL]] to i8 addrspace(200)* addrspace(200)*
; CHECK-NEXT:    store i8 addrspace(200)* [[ARG]], i8 addrspace(200)* addrspace(200)* [[AS_CAP]], align 8
; CHECK-NEXT:    ret i32 undef
;
bb:
  %stackval = alloca %struct.addrinfo, align 8, addrspace(200)
  %field = getelementptr inbounds %struct.addrinfo, %struct.addrinfo addrspace(200)* %stackval, i64 0, i32 2
  %as_cap = bitcast %struct.addrinfo addrspace(200)* %stackval to i8 addrspace(200)* addrspace(200)*
  store i8 addrspace(200)* %arg, i8 addrspace(200)* addrspace(200)* %as_cap, align 8
  %result = load i32, i32 addrspace(200)* %field, align 4
  ret i32 %result
}

define i32 @fourth_i32_store_to_load_fwd(i8 addrspace(200)* %arg) local_unnamed_addr addrspace(200) nounwind {
; ASM-LABEL: fourth_i32_store_to_load_fwd:
; ASM:       # %bb.0: # %bb
; ASM-NEXT:    cincoffset csp, csp, -48
; ASM-NEXT:    csc ca0, 0(csp)
; ASM-NEXT:    # implicit-def: $x10
; ASM-NEXT:    cincoffset csp, csp, 48
; ASM-NEXT:    cret
; CHECK-LABEL: define {{[^@]+}}@fourth_i32_store_to_load_fwd
; CHECK-SAME: (i8 addrspace(200)* [[ARG:%.*]]) local_unnamed_addr addrspace(200)
; CHECK-NEXT:  bb:
; CHECK-NEXT:    [[STACKVAL:%.*]] = alloca [[STRUCT_ADDRINFO:%.*]], align 8, addrspace(200)
; CHECK-NEXT:    [[FIELD:%.*]] = getelementptr inbounds [[STRUCT_ADDRINFO]], [[STRUCT_ADDRINFO]] addrspace(200)* [[STACKVAL]], i64 0, i32 3
; CHECK-NEXT:    [[AS_CAP:%.*]] = bitcast [[STRUCT_ADDRINFO]] addrspace(200)* [[STACKVAL]] to i8 addrspace(200)* addrspace(200)*
; CHECK-NEXT:    store i8 addrspace(200)* [[ARG]], i8 addrspace(200)* addrspace(200)* [[AS_CAP]], align 8
; CHECK-NEXT:    ret i32 undef
;
bb:
  %stackval = alloca %struct.addrinfo, align 8, addrspace(200)
  %field = getelementptr inbounds %struct.addrinfo, %struct.addrinfo addrspace(200)* %stackval, i64 0, i32 3
  %as_cap = bitcast %struct.addrinfo addrspace(200)* %stackval to i8 addrspace(200)* addrspace(200)*
  store i8 addrspace(200)* %arg, i8 addrspace(200)* addrspace(200)* %as_cap, align 8
  %result = load i32, i32 addrspace(200)* %field, align 4
  ret i32 %result
}
