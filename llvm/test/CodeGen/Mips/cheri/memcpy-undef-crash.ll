; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: %cheri128_purecap_llc -o - -O2 -verify-machineinstrs %s | FileCheck %s
source_filename = "thr_umtx-f3c559.c"

module asm ".ident\09\22$FreeBSD$\22"

%struct.umutex = type { i32, i32, [2 x i32], i8 addrspace(200)*, i32, [2 x i32] }

@_thr_umutex_init.default_mtx = internal addrspace(200) constant %struct.umutex zeroinitializer, align 16

declare void @llvm.memcpy.p200i8.p200i8.i64(i8 addrspace(200)* nocapture writeonly, i8 addrspace(200)* nocapture readonly, i64, i1) addrspace(200) #1

define hidden void @_thr_umutex_init_undef(i8 addrspace(200)* %arg) addrspace(200) #0 {
; memcpy can be removed:
; CHECK-LABEL: _thr_umutex_init_undef:
; CHECK:       # %bb.0: # %entry
; CHECK-NEXT:    cjr $c17
; CHECK-NEXT:    nop
entry:
  call void @llvm.memcpy.p200i8.p200i8.i64(i8 addrspace(200)* align 16 %arg, i8 addrspace(200)* align 16 undef, i64 48, i1 false)
  ret void
}

define hidden void @_thr_umutex_init_zero(i8 addrspace(200)* %arg) addrspace(200) #0 {
; CHECK-LABEL: _thr_umutex_init_zero:
; CHECK:       # %bb.0: # %entry
; CHECK-NEXT:    csc $cnull, $zero, 0($c3)
; CHECK-NEXT:    csc $cnull, $zero, 32($c3)
; CHECK-NEXT:    cjr $c17
; CHECK-NEXT:    csc $cnull, $zero, 16($c3)
entry:
  call void @llvm.memcpy.p200i8.p200i8.i64(i8 addrspace(200)* align 16 %arg, i8 addrspace(200)* align 16 bitcast (%struct.umutex addrspace(200)* @_thr_umutex_init.default_mtx to i8 addrspace(200)*), i64 48, i1 false)
  ret void
}

; Function Attrs: argmemonly nounwind

attributes #0 = { "use-soft-float"="true" }
attributes #1 = { argmemonly nounwind }

!llvm.ident = !{!0}

!0 = !{!"clang version 9.0.0 (https://github.com/CTSRD-CHERI/llvm-project 243d101440bcf9fd5f909af13905ee77e33cf08c)"}
