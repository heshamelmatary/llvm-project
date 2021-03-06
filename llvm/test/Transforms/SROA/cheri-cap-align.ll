; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt -mtriple=cheri-unknown-freebsd -mcpu=cheri128 -mattr=+cheri128 \
; RUN:     -S -passes=sroa < %s \
; RUN:     | FileCheck %s

target datalayout = "E-m:e-pf200:128:128:128:64-i8:8:32-i16:16:32-i64:64-n32:64-S128"
%struct = type { i32, i32, i32, i32, i8 addrspace(200)* }

; Reduced test case based on FreeBSD newsyslog's parseDWM.
define void @foo_struct(%struct* %px) {
; CHECK-LABEL: @foo_struct(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[PY_SROA_4:%.*]] = alloca { i32, i32 }, align 8
; CHECK-NEXT:    [[PY_SROA_0_0_PX_CAST_SROA_IDX:%.*]] = getelementptr inbounds [[STRUCT:%.*]], %struct* [[PX:%.*]], i64 0, i32 0
; CHECK-NEXT:    [[PY_SROA_0_0_COPYLOAD:%.*]] = load i32, i32* [[PY_SROA_0_0_PX_CAST_SROA_IDX]], align 16
; CHECK-NEXT:    [[PY_SROA_2_0_PX_CAST_SROA_IDX3:%.*]] = getelementptr inbounds [[STRUCT]], %struct* [[PX]], i64 0, i32 1
; CHECK-NEXT:    [[PY_SROA_2_0_COPYLOAD:%.*]] = load i32, i32* [[PY_SROA_2_0_PX_CAST_SROA_IDX3]], align 4
; CHECK-NEXT:    [[PY_SROA_4_0_PX_CAST_SROA_IDX:%.*]] = getelementptr inbounds [[STRUCT]], %struct* [[PX]], i64 0, i32 2
; CHECK-NEXT:    [[PY_SROA_4_0_PX_CAST_SROA_CAST:%.*]] = bitcast i32* [[PY_SROA_4_0_PX_CAST_SROA_IDX]] to i8*
; CHECK-NEXT:    [[PY_SROA_4_0_PY_CAST_SROA_CAST:%.*]] = bitcast { i32, i32 }* [[PY_SROA_4]] to i8*
; CHECK-NEXT:    call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 8 [[PY_SROA_4_0_PY_CAST_SROA_CAST]], i8* align 8 [[PY_SROA_4_0_PX_CAST_SROA_CAST]], i64 8, i1 false)
; CHECK-NEXT:    [[PY_SROA_49_0_PX_CAST_SROA_IDX10:%.*]] = getelementptr inbounds [[STRUCT]], %struct* [[PX]], i64 0, i32 4
; CHECK-NEXT:    [[PY_SROA_49_0_COPYLOAD:%.*]] = load i8 addrspace(200)*, i8 addrspace(200)** [[PY_SROA_49_0_PX_CAST_SROA_IDX10]], align 16
; CHECK-NEXT:    [[Y_1_NEW:%.*]] = call i32 @bar(i32 [[PY_SROA_2_0_COPYLOAD]])
; CHECK-NEXT:    [[PY_SROA_0_0_PX_CAST_SROA_IDX1:%.*]] = getelementptr inbounds [[STRUCT]], %struct* [[PX]], i64 0, i32 0
; CHECK-NEXT:    store i32 [[PY_SROA_0_0_COPYLOAD]], i32* [[PY_SROA_0_0_PX_CAST_SROA_IDX1]], align 16
; CHECK-NEXT:    [[PY_SROA_2_0_PX_CAST_SROA_IDX4:%.*]] = getelementptr inbounds [[STRUCT]], %struct* [[PX]], i64 0, i32 1
; CHECK-NEXT:    store i32 [[Y_1_NEW]], i32* [[PY_SROA_2_0_PX_CAST_SROA_IDX4]], align 4
; CHECK-NEXT:    [[PY_SROA_4_0_PX_CAST_SROA_IDX6:%.*]] = getelementptr inbounds [[STRUCT]], %struct* [[PX]], i64 0, i32 2
; CHECK-NEXT:    [[PY_SROA_4_0_PX_CAST_SROA_CAST7:%.*]] = bitcast i32* [[PY_SROA_4_0_PX_CAST_SROA_IDX6]] to i8*
; CHECK-NEXT:    [[PY_SROA_4_0_PY_CAST_SROA_CAST8:%.*]] = bitcast { i32, i32 }* [[PY_SROA_4]] to i8*
; CHECK-NEXT:    call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 8 [[PY_SROA_4_0_PX_CAST_SROA_CAST7]], i8* align 8 [[PY_SROA_4_0_PY_CAST_SROA_CAST8]], i64 8, i1 false)
; CHECK-NEXT:    [[PY_SROA_49_0_PX_CAST_SROA_IDX11:%.*]] = getelementptr inbounds [[STRUCT]], %struct* [[PX]], i64 0, i32 4
; CHECK-NEXT:    store i8 addrspace(200)* [[PY_SROA_49_0_COPYLOAD]], i8 addrspace(200)** [[PY_SROA_49_0_PX_CAST_SROA_IDX11]], align 16
; CHECK-NEXT:    ret void
;
entry:
  %py = alloca %struct, align 16
  %px.cast = bitcast %struct* %px to i8*
  %py.cast = bitcast %struct* %py to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %py.cast, i8* align 16 %px.cast, i64 32, i1 false)
  %py.1 = getelementptr inbounds %struct, %struct* %py, i64 0, i32 1
  %y.1 = load i32, i32* %py.1, align 4
  %y.1.new = call i32 @bar(i32 %y.1)
  store i32 %y.1.new, i32* %py.1, align 4
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %px.cast, i8* align 16 %py.cast, i64 32, i1 false)
  ret void
}

; This is the same as @foo_struct, but with an opaque aligned byte buffer
; instead of a struct. Without the type information (and without control flow
; analysis), we have to assume that the first half of the buffer could contain
; a capability too, and thus it cannot be further broken down by SROA alone.
define void @foo_buf(%struct* %px) {
; CHECK-LABEL: @foo_buf(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[PY_SROA_0:%.*]] = alloca [16 x i8], align 16
; CHECK-NEXT:    [[PY_SROA_6:%.*]] = alloca [16 x i8], align 16
; CHECK-NEXT:    [[PY_SROA_0_0_PX_CAST_SROA_CAST:%.*]] = bitcast %struct* [[PX:%.*]] to i8*
; CHECK-NEXT:    [[PY_SROA_0_0_PY_CAST_SROA_IDX:%.*]] = getelementptr inbounds [16 x i8], [16 x i8]* [[PY_SROA_0]], i64 0, i64 0
; CHECK-NEXT:    call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 [[PY_SROA_0_0_PY_CAST_SROA_IDX]], i8* align 16 [[PY_SROA_0_0_PX_CAST_SROA_CAST]], i64 16, i1 false)
; CHECK-NEXT:    [[PY_SROA_6_0_PX_CAST_SROA_IDX:%.*]] = getelementptr inbounds [[STRUCT:%.*]], %struct* [[PX]], i64 0, i32 4
; CHECK-NEXT:    [[PY_SROA_6_0_PX_CAST_SROA_CAST:%.*]] = bitcast i8 addrspace(200)** [[PY_SROA_6_0_PX_CAST_SROA_IDX]] to i8*
; CHECK-NEXT:    [[PY_SROA_6_0_PY_CAST_SROA_IDX11:%.*]] = getelementptr inbounds [16 x i8], [16 x i8]* [[PY_SROA_6]], i64 0, i64 0
; CHECK-NEXT:    call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 [[PY_SROA_6_0_PY_CAST_SROA_IDX11]], i8* align 16 [[PY_SROA_6_0_PX_CAST_SROA_CAST]], i64 16, i1 false)
; CHECK-NEXT:    [[PY_SROA_0_4_PY_1_CAST_SROA_IDX14:%.*]] = getelementptr inbounds [16 x i8], [16 x i8]* [[PY_SROA_0]], i64 0, i64 4
; CHECK-NEXT:    [[PY_SROA_0_4_PY_1_CAST_SROA_CAST15:%.*]] = bitcast i8* [[PY_SROA_0_4_PY_1_CAST_SROA_IDX14]] to i32*
; CHECK-NEXT:    [[PY_SROA_0_4_PY_SROA_0_4_Y_1:%.*]] = load i32, i32* [[PY_SROA_0_4_PY_1_CAST_SROA_CAST15]]
; CHECK-NEXT:    [[Y_1_NEW:%.*]] = call i32 @bar(i32 [[PY_SROA_0_4_PY_SROA_0_4_Y_1]])
; CHECK-NEXT:    [[PY_SROA_0_4_PY_1_CAST_SROA_IDX16:%.*]] = getelementptr inbounds [16 x i8], [16 x i8]* [[PY_SROA_0]], i64 0, i64 4
; CHECK-NEXT:    [[PY_SROA_0_4_PY_1_CAST_SROA_CAST17:%.*]] = bitcast i8* [[PY_SROA_0_4_PY_1_CAST_SROA_IDX16]] to i32*
; CHECK-NEXT:    store i32 [[Y_1_NEW]], i32* [[PY_SROA_0_4_PY_1_CAST_SROA_CAST17]]
; CHECK-NEXT:    [[PY_SROA_0_0_PX_CAST_SROA_CAST3:%.*]] = bitcast %struct* [[PX]] to i8*
; CHECK-NEXT:    [[PY_SROA_0_0_PY_CAST_SROA_IDX13:%.*]] = getelementptr inbounds [16 x i8], [16 x i8]* [[PY_SROA_0]], i64 0, i64 0
; CHECK-NEXT:    call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 [[PY_SROA_0_0_PX_CAST_SROA_CAST3]], i8* align 16 [[PY_SROA_0_0_PY_CAST_SROA_IDX13]], i64 16, i1 false)
; CHECK-NEXT:    [[PY_SROA_6_0_PX_CAST_SROA_IDX7:%.*]] = getelementptr inbounds [[STRUCT]], %struct* [[PX]], i64 0, i32 4
; CHECK-NEXT:    [[PY_SROA_6_0_PX_CAST_SROA_CAST8:%.*]] = bitcast i8 addrspace(200)** [[PY_SROA_6_0_PX_CAST_SROA_IDX7]] to i8*
; CHECK-NEXT:    [[PY_SROA_6_0_PY_CAST_SROA_IDX12:%.*]] = getelementptr inbounds [16 x i8], [16 x i8]* [[PY_SROA_6]], i64 0, i64 0
; CHECK-NEXT:    call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 [[PY_SROA_6_0_PX_CAST_SROA_CAST8]], i8* align 16 [[PY_SROA_6_0_PY_CAST_SROA_IDX12]], i64 16, i1 false)
; CHECK-NEXT:    ret void
;
entry:
  %py = alloca [32 x i8], align 16
  %px.cast = bitcast %struct* %px to i8*
  %py.cast = bitcast [32 x i8]* %py to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %py.cast, i8* align 16 %px.cast, i64 32, i1 false)
  %py.1 = getelementptr inbounds [32 x i8], [32 x i8]* %py, i64 0, i32 4
  %py.1.cast = bitcast i8* %py.1 to i32*
  %y.1 = load i32, i32* %py.1.cast, align 4
  %y.1.new = call i32 @bar(i32 %y.1)
  store i32 %y.1.new, i32* %py.1.cast, align 4
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %px.cast, i8* align 16 %py.cast, i64 32, i1 false)
  ret void
}

declare i32 @bar(i32)
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i1 immarg)
