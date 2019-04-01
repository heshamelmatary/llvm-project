; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: %riscv32_cheri_purecap_llc -verify-machineinstrs < %s | FileCheck %s

define i8 addrspace(200)* @get_ith_cap(i32 signext %i, ...) nounwind {
; CHECK-LABEL: get_ith_cap:
; CHECK:       # %bb.0: # %entry
; CHECK-NEXT:    cincoffset csp, csp, -80
; CHECK-NEXT:    cincoffset ct0, csp, 72
; CHECK-NEXT:    sc.cap ca7, ct0
; CHECK-NEXT:    cincoffset ca7, csp, 64
; CHECK-NEXT:    sc.cap ca6, ca7
; CHECK-NEXT:    cincoffset ca6, csp, 56
; CHECK-NEXT:    sc.cap ca5, ca6
; CHECK-NEXT:    cincoffset ca5, csp, 48
; CHECK-NEXT:    sc.cap ca4, ca5
; CHECK-NEXT:    cincoffset ca4, csp, 40
; CHECK-NEXT:    sc.cap ca3, ca4
; CHECK-NEXT:    cincoffset ca3, csp, 32
; CHECK-NEXT:    sc.cap ca2, ca3
; CHECK-NEXT:    cincoffset ca2, csp, 24
; CHECK-NEXT:    sc.cap ca1, ca2
; CHECK-NEXT:    cincoffset ca1, csp, 8
; CHECK-NEXT:    cincoffset ca2, csp, 24
; CHECK-NEXT:    sc.cap ca2, ca1
; CHECK-NEXT:    addi a0, a0, 1
; CHECK-NEXT:  .LBB0_1: # %while.cond
; CHECK-NEXT:    # =>This Inner Loop Header: Depth=1
; CHECK-NEXT:    cgetaddr a3, ca2
; CHECK-NEXT:    addi a4, a3, 7
; CHECK-NEXT:    andi a4, a4, -8
; CHECK-NEXT:    sub a3, a4, a3
; CHECK-NEXT:    cincoffset ca3, ca2, a3
; CHECK-NEXT:    cincoffset ca2, ca3, 8
; CHECK-NEXT:    addi a0, a0, -1
; CHECK-NEXT:    bgtz a0, .LBB0_1
; CHECK-NEXT:  # %bb.2: # %while.end
; CHECK-NEXT:    sc.cap ca2, ca1
; CHECK-NEXT:    lc.cap ca0, ca3
; CHECK-NEXT:    cincoffset csp, csp, 80
; CHECK-NEXT:    ret
entry:
  %ap = alloca i8 addrspace(200)*, align 8, addrspace(200)
  %0 = bitcast i8 addrspace(200)* addrspace(200)* %ap to i8 addrspace(200)*
  call void @llvm.lifetime.start.p200i8(i64 8, i8 addrspace(200)* nonnull %0)
  call void @llvm.va_start.p200i8(i8 addrspace(200)* nonnull %0)
  %ap.promoted = load i8 addrspace(200)*, i8 addrspace(200)* addrspace(200)* %ap, align 8
  br label %while.cond

while.cond:
  %argp.next6 = phi i8 addrspace(200)* [ %ap.promoted, %entry ], [ %argp.next, %while.cond ]
  %i.addr.0 = phi i32 [ %i, %entry ], [ %dec, %while.cond ]
  %dec = add nsw i32 %i.addr.0, -1
  %cmp = icmp sgt i32 %i.addr.0, 0
  %1 = call i32 @llvm.cheri.cap.address.get(i8 addrspace(200)* %argp.next6)
  %2 = add i32 %1, 7
  %3 = and i32 %2, -8
  %4 = call i8 addrspace(200)* @llvm.cheri.cap.address.set(i8 addrspace(200)* %argp.next6, i32 %3)
  %argp.next = getelementptr inbounds i8, i8 addrspace(200)* %4, i64 8
  br i1 %cmp, label %while.cond, label %while.end

while.end:
  store i8 addrspace(200)* %argp.next, i8 addrspace(200)* addrspace(200)* %ap, align 8
  %5 = bitcast i8 addrspace(200)* %4 to i8 addrspace(200)* addrspace(200)*
  %6 = load i8 addrspace(200)*, i8 addrspace(200)* addrspace(200)* %5, align 8
  call void @llvm.va_end.p200i8(i8 addrspace(200)* nonnull %0)
  call void @llvm.lifetime.end.p200i8(i64 8, i8 addrspace(200)* nonnull %0)
  ret i8 addrspace(200)* %6
}

declare void @llvm.lifetime.start.p200i8(i64, i8 addrspace(200)* nocapture)
declare void @llvm.va_start.p200i8(i8 addrspace(200)*)
declare i32 @llvm.cheri.cap.address.get(i8 addrspace(200)*)
declare i8 addrspace(200)* @llvm.cheri.cap.address.set(i8 addrspace(200)*, i32)
declare void @llvm.va_end.p200i8(i8 addrspace(200)*)
declare void @llvm.lifetime.end.p200i8(i64, i8 addrspace(200)* nocapture)
