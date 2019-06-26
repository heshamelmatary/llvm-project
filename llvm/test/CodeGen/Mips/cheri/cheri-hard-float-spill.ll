; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: %cheri_purecap_llc -float-abi=hard -cheri-cap-table-abi=pcrel %s -o - | %cheri_FileCheck %s -check-prefix CAPTABLE
; RUN: %cheri_purecap_llc -float-abi=hard -cheri-cap-table-abi=legacy %s -o - | %cheri_FileCheck %s -check-prefix LEGACY

%struct.site.5.59.89.167.203.227.257.329.347.443.779 = type { i16, i16, i16, i16, i8, i32, %struct.double_prn.0.54.84.162.198.222.252.324.342.438.774, i32, [4 x %struct.su3_matrix.2.56.86.164.200.224.254.326.344.440.776], [4 x %struct.anti_hermitmat.3.57.87.165.201.225.255.327.345.441.777], [4 x double], %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778, [4 x %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778], [4 x %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778], %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778, %struct.su3_matrix.2.56.86.164.200.224.254.326.344.440.776, %struct.su3_matrix.2.56.86.164.200.224.254.326.344.440.776 }
%struct.double_prn.0.54.84.162.198.222.252.324.342.438.774 = type { i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, double }
%struct.anti_hermitmat.3.57.87.165.201.225.255.327.345.441.777 = type { %struct.complex.1.55.85.163.199.223.253.325.343.439.775, %struct.complex.1.55.85.163.199.223.253.325.343.439.775, %struct.complex.1.55.85.163.199.223.253.325.343.439.775, double, double, double, double }
%struct.complex.1.55.85.163.199.223.253.325.343.439.775 = type { double, double }
%struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 = type { [3 x %struct.complex.1.55.85.163.199.223.253.325.343.439.775] }
%struct.su3_matrix.2.56.86.164.200.224.254.326.344.440.776 = type { [3 x [3 x %struct.complex.1.55.85.163.199.223.253.325.343.439.775]] }

@act_path_coeff = external hidden unnamed_addr addrspace(200) global [6 x double], align 8
@sites_on_node = external addrspace(200) global i32, align 4
@lattice = external addrspace(200) global %struct.site.5.59.89.167.203.227.257.329.347.443.779 addrspace(200)*, align 32

; Function Attrs: nounwind argmemonly
declare void @llvm.lifetime.start.p200i8(i64, i8 addrspace(200)* nocapture) #0

; Function Attrs: nounwind argmemonly
declare void @llvm.lifetime.end.p200i8(i64, i8 addrspace(200)* nocapture) #0

; Function Attrs: nounwind
declare noalias i8 addrspace(200)* @calloc(i64 zeroext, i64 zeroext) #1

; Function Attrs: nounwind argmemonly
declare void @llvm.memcpy.p200i8.p200i8.i64(i8 addrspace(200)* nocapture, i8 addrspace(200)* nocapture readonly, i64, i32, i1) #0

; Function Attrs: nounwind
declare void @free(i8 addrspace(200)* nocapture) #1

; Function Attrs: nounwind
define void @eo_fermion_force(double %eps, i32 signext %nflavors, i32 signext %x_off) nounwind {
; LEGACY-LABEL: eo_fermion_force:
; LEGACY:       # %bb.0: # %entry
; LEGACY-NEXT:    cincoffset $c11, $c11, -[[#STACKFRAME_SIZE:]]
; LEGACY-NEXT:    dmfc1 $1, $f27
; LEGACY-NEXT:    csd $1, $zero, [[#STACKFRAME_SIZE - 8]]($c11)
; LEGACY-NEXT:    dmfc1 $1, $f26
; LEGACY-NEXT:    csd $1, $zero, [[#STACKFRAME_SIZE - 16]]($c11)
; LEGACY-NEXT:    dmfc1 $1, $f25
; LEGACY-NEXT:    csd $1, $zero, [[#STACKFRAME_SIZE - 24]]($c11)
; LEGACY-NEXT:    dmfc1 $1, $f24
; LEGACY-NEXT:    csd $1, $zero, [[#STACKFRAME_SIZE - 32]]($c11)
; LEGACY-NEXT:    csd $gp, $zero, [[#STACKFRAME_SIZE - 40]]($c11)
; LEGACY-NEXT:    csd $16, $zero, [[#STACKFRAME_SIZE - 48]]($c11)
; LEGACY-NEXT:    csc $c17, $zero, 0($c11)
; LEGACY-NEXT:    cgetoffset $25, $c12
; LEGACY-NEXT:    lui $1, %hi(%neg(%gp_rel(eo_fermion_force)))
; LEGACY-NEXT:    b .LBB0_19
; LEGACY-NEXT:    daddu $2, $1, $25
; LEGACY-NEXT:  # %bb.1: # %for.cond.30.preheader
; LEGACY-NEXT:    daddiu $gp, $2, %lo(%neg(%gp_rel(eo_fermion_force)))
; LEGACY-NEXT:    ld $1, %got_page(.LCPI0_0)($gp)
; LEGACY-NEXT:    ldc1 $f24, %got_ofst(.LCPI0_0)($1)
; LEGACY-NEXT:    neg.d $f25, $f24
; LEGACY-NEXT:    div.d $f26, $f24, $f0
; LEGACY-NEXT:    neg.d $f27, $f0
; LEGACY-NEXT:    addiu $2, $zero, 1
; LEGACY-NEXT:  .LBB0_2: # %for.body.37
; LEGACY-NEXT:    # =>This Inner Loop Header: Depth=1
; LEGACY-NEXT:    bnez $2, .LBB0_2
; LEGACY-NEXT:    nop
; LEGACY-NEXT:  # %bb.3: # %if.then
; LEGACY-NEXT:    ld $1, %call16(u_shift_fermion)($gp)
; LEGACY-NEXT:    cgetpccsetoffset $c12, $1
; LEGACY-NEXT:    cjalr $c12, $c17
; LEGACY-NEXT:    daddiu $4, $zero, 0
; LEGACY-NEXT:    addiu $16, $zero, 1
; LEGACY-NEXT:    bnez $16, .LBB0_20
; LEGACY-NEXT:    nop
; LEGACY-NEXT:  .LBB0_4: # %for.body.55
; LEGACY-NEXT:    # =>This Inner Loop Header: Depth=1
; LEGACY-NEXT:    bnez $zero, .LBB0_13
; LEGACY-NEXT:    nop
; LEGACY-NEXT:  # %bb.5: # %if.then.69
; LEGACY-NEXT:    # in Loop: Header=BB0_4 Depth=1
; LEGACY-NEXT:    bnez $16, .LBB0_16
; LEGACY-NEXT:    nop
; LEGACY-NEXT:  # %bb.6: # %for.body.84.preheader
; LEGACY-NEXT:    # in Loop: Header=BB0_4 Depth=1
; LEGACY-NEXT:    bnez $zero, .LBB0_10
; LEGACY-NEXT:    nop
; LEGACY-NEXT:  # %bb.7: # %if.then.105
; LEGACY-NEXT:    # in Loop: Header=BB0_4 Depth=1
; LEGACY-NEXT:    bnez $zero, .LBB0_18
; LEGACY-NEXT:    nop
; LEGACY-NEXT:  # %bb.8: # %if.then.113
; LEGACY-NEXT:    # in Loop: Header=BB0_4 Depth=1
; LEGACY-NEXT:    ld $1, %call16(add_force_to_mom)($gp)
; LEGACY-NEXT:    cgetpccsetoffset $c12, $1
; LEGACY-NEXT:    daddiu $4, $zero, 0
; LEGACY-NEXT:    cjalr $c12, $c17
; LEGACY-NEXT:    mov.d $f13, $f25
; LEGACY-NEXT:    ld $1, %call16(add_force_to_mom)($gp)
; LEGACY-NEXT:    cgetpccsetoffset $c12, $1
; LEGACY-NEXT:    daddiu $4, $zero, 0
; LEGACY-NEXT:    cjalr $c12, $c17
; LEGACY-NEXT:    mov.d $f13, $f24
; LEGACY-NEXT:    bnez $zero, .LBB0_10
; LEGACY-NEXT:    nop
; LEGACY-NEXT:  # %bb.9: # %for.body.128.preheader
; LEGACY-NEXT:    # in Loop: Header=BB0_4 Depth=1
; LEGACY-NEXT:    ld $1, %call16(scalar_mult_add_su3_vector)($gp)
; LEGACY-NEXT:    cgetpccsetoffset $c12, $1
; LEGACY-NEXT:    cjalr $c12, $c17
; LEGACY-NEXT:    mov.d $f12, $f26
; LEGACY-NEXT:  .LBB0_10: # %for.inc.143
; LEGACY-NEXT:    # in Loop: Header=BB0_4 Depth=1
; LEGACY-NEXT:    b .LBB0_17
; LEGACY-NEXT:    nop
; LEGACY-NEXT:  # %bb.11: # %if.else.8.i.415
; LEGACY-NEXT:    # in Loop: Header=BB0_4 Depth=1
; LEGACY-NEXT:    ld $1, %call16(add_force_to_mom)($gp)
; LEGACY-NEXT:    cgetpccsetoffset $c12, $1
; LEGACY-NEXT:    daddiu $4, $zero, 0
; LEGACY-NEXT:    cjalr $c12, $c17
; LEGACY-NEXT:    mov.d $f13, $f27
; LEGACY-NEXT:    bnez $zero, .LBB0_13
; LEGACY-NEXT:    nop
; LEGACY-NEXT:  # %bb.12: # %for.body.157.preheader
; LEGACY-NEXT:    # in Loop: Header=BB0_4 Depth=1
; LEGACY-NEXT:    ld $1, %call16(scalar_mult_add_su3_vector)($gp)
; LEGACY-NEXT:    cgetpccsetoffset $c12, $1
; LEGACY-NEXT:    cjalr $c12, $c17
; LEGACY-NEXT:    mov.d $f12, $f24
; LEGACY-NEXT:  .LBB0_13: # %for.inc.172
; LEGACY-NEXT:    # in Loop: Header=BB0_4 Depth=1
; LEGACY-NEXT:    bnez $zero, .LBB0_4
; LEGACY-NEXT:    nop
; LEGACY-NEXT:  # %bb.14: # %for.end.174
; LEGACY-NEXT:    b .LBB0_21
; LEGACY-NEXT:    nop
; LEGACY-NEXT:  .LBB0_15: # %for.body.197
; LEGACY-NEXT:    # =>This Inner Loop Header: Depth=1
; LEGACY-NEXT:    ld $1, %call16(scalar_mult_add_su3_vector)($gp)
; LEGACY-NEXT:    cgetpccsetoffset $c12, $1
; LEGACY-NEXT:    cjalr $c12, $c17
; LEGACY-NEXT:    mov.d $f12, $f24
; LEGACY-NEXT:    b .LBB0_15
; LEGACY-NEXT:    nop
; LEGACY-NEXT:  .LBB0_16: # %if.then.77
; LEGACY-NEXT:    ld $1, %call16(add_force_to_mom)($gp)
; LEGACY-NEXT:    cgetpccsetoffset $c12, $1
; LEGACY-NEXT:    cjalr $c12, $c17
; LEGACY-NEXT:    daddiu $4, $zero, 0
; LEGACY-NEXT:  .LBB0_17: # %if.then.6.i.413
; LEGACY-NEXT:    .insn
; LEGACY-NEXT:  .LBB0_18: # %if.else.i.critedge
; LEGACY-NEXT:    .insn
; LEGACY-NEXT:  .LBB0_19: # %for.body.24.lr.ph
; LEGACY-NEXT:    .insn
; LEGACY-NEXT:  .LBB0_20: # %if.then.48
; LEGACY-NEXT:    ld $1, %call16(add_force_to_mom)($gp)
; LEGACY-NEXT:    cgetpccsetoffset $c12, $1
; LEGACY-NEXT:    cjalr $c12, $c17
; LEGACY-NEXT:    daddiu $4, $zero, 0
; LEGACY-NEXT:  .LBB0_21: # %if.then.182
; LEGACY-NEXT:    ld $1, %call16(add_force_to_mom)($gp)
; LEGACY-NEXT:    cgetpccsetoffset $c12, $1
; LEGACY-NEXT:    daddiu $4, $zero, 0
; LEGACY-NEXT:    cjalr $c12, $c17
; LEGACY-NEXT:    mov.d $f13, $f24

; CAPTABLE-LABEL: eo_fermion_force:
; CAPTABLE:       # %bb.0: # %entry
; CAPTABLE-NEXT:    cincoffset $c11, $c11, -[[#STACKFRAME_SIZE:]]
; CAPTABLE-NEXT:    dmfc1 $1, $f28
; CAPTABLE-NEXT:    csd $1, $zero, [[#STACKFRAME_SIZE - 8]]($c11)
; CAPTABLE-NEXT:    dmfc1 $1, $f27
; CAPTABLE-NEXT:    csd $1, $zero, [[#STACKFRAME_SIZE - 16]]($c11)
; CAPTABLE-NEXT:    dmfc1 $1, $f26
; CAPTABLE-NEXT:    csd $1, $zero, [[#STACKFRAME_SIZE - 24]]($c11)
; CAPTABLE-NEXT:    dmfc1 $1, $f25
; CAPTABLE-NEXT:    csd $1, $zero, [[#STACKFRAME_SIZE - 32]]($c11)
; CAPTABLE-NEXT:    dmfc1 $1, $f24
; CAPTABLE-NEXT:    csd $1, $zero, [[#STACKFRAME_SIZE - 40]]($c11)
; CAPTABLE-NEXT:    csd $16, $zero, [[#STACKFRAME_SIZE - 48]]($c11)
; CAPTABLE-NEXT:    csc $c18, $zero, [[#CAP_SIZE * 1]]($c11)
; CAPTABLE-NEXT:    csc $c17, $zero, 0($c11)
; CAPTABLE-NEXT:    lui $1, %hi(%neg(%captab_rel(eo_fermion_force)))
; CAPTABLE-NEXT:    daddiu $1, $1, %lo(%neg(%captab_rel(eo_fermion_force)))
; CAPTABLE-NEXT:    cincoffset $c18, $c12, $1
; CAPTABLE-NEXT:    clcbi $c1, %captab20(.LCPI0_0)($c18)
; CAPTABLE-NEXT:    cld $2, $zero, 0($c1)
; CAPTABLE-NEXT:    b .LBB0_19
; CAPTABLE-NEXT:    nop
; CAPTABLE-NEXT:  # %bb.1: # %for.cond.30.preheader
; CAPTABLE-NEXT:    dmtc1 $2, $f24
; CAPTABLE-NEXT:    neg.d $f26, $f24
; CAPTABLE-NEXT:    cld $1, $zero, 0($c1)
; CAPTABLE-NEXT:    div.d $f27, $f24, $f0
; CAPTABLE-NEXT:    neg.d $f28, $f0
; CAPTABLE-NEXT:    dmtc1 $1, $f25
; CAPTABLE-NEXT:    addiu $2, $zero, 1
; CAPTABLE-NEXT:  .LBB0_2: # %for.body.37
; CAPTABLE-NEXT:    # =>This Inner Loop Header: Depth=1
; CAPTABLE-NEXT:    bnez $2, .LBB0_2
; CAPTABLE-NEXT:    nop
; CAPTABLE-NEXT:  # %bb.3: # %if.then
; CAPTABLE-NEXT:    clcbi $c12, %capcall20(u_shift_fermion)($c18)
; CAPTABLE-NEXT:    cjalr $c12, $c17
; CAPTABLE-NEXT:    daddiu $4, $zero, 0
; CAPTABLE-NEXT:    addiu $16, $zero, 1
; CAPTABLE-NEXT:    bnez $16, .LBB0_20
; CAPTABLE-NEXT:    nop
; CAPTABLE-NEXT:  .LBB0_4: # %for.body.55
; CAPTABLE-NEXT:    # =>This Inner Loop Header: Depth=1
; CAPTABLE-NEXT:    bnez $zero, .LBB0_13
; CAPTABLE-NEXT:    nop
; CAPTABLE-NEXT:  # %bb.5: # %if.then.69
; CAPTABLE-NEXT:    # in Loop: Header=BB0_4 Depth=1
; CAPTABLE-NEXT:    bnez $16, .LBB0_16
; CAPTABLE-NEXT:    nop
; CAPTABLE-NEXT:  # %bb.6: # %for.body.84.preheader
; CAPTABLE-NEXT:    # in Loop: Header=BB0_4 Depth=1
; CAPTABLE-NEXT:    bnez $zero, .LBB0_10
; CAPTABLE-NEXT:    nop
; CAPTABLE-NEXT:  # %bb.7: # %if.then.105
; CAPTABLE-NEXT:    # in Loop: Header=BB0_4 Depth=1
; CAPTABLE-NEXT:    bnez $zero, .LBB0_18
; CAPTABLE-NEXT:    nop
; CAPTABLE-NEXT:  # %bb.8: # %if.then.113
; CAPTABLE-NEXT:    # in Loop: Header=BB0_4 Depth=1
; CAPTABLE-NEXT:    clcbi $c12, %capcall20(add_force_to_mom)($c18)
; CAPTABLE-NEXT:    daddiu $4, $zero, 0
; CAPTABLE-NEXT:    cjalr $c12, $c17
; CAPTABLE-NEXT:    mov.d $f13, $f26
; CAPTABLE-NEXT:    clcbi $c12, %capcall20(add_force_to_mom)($c18)
; CAPTABLE-NEXT:    daddiu $4, $zero, 0
; CAPTABLE-NEXT:    cjalr $c12, $c17
; CAPTABLE-NEXT:    mov.d $f13, $f24
; CAPTABLE-NEXT:    bnez $zero, .LBB0_10
; CAPTABLE-NEXT:    nop
; CAPTABLE-NEXT:  # %bb.9: # %for.body.128.preheader
; CAPTABLE-NEXT:    # in Loop: Header=BB0_4 Depth=1
; CAPTABLE-NEXT:    clcbi $c12, %capcall20(scalar_mult_add_su3_vector)($c18)
; CAPTABLE-NEXT:    cjalr $c12, $c17
; CAPTABLE-NEXT:    mov.d $f12, $f27
; CAPTABLE-NEXT:  .LBB0_10: # %for.inc.143
; CAPTABLE-NEXT:    # in Loop: Header=BB0_4 Depth=1
; CAPTABLE-NEXT:    b .LBB0_17
; CAPTABLE-NEXT:    nop
; CAPTABLE-NEXT:  # %bb.11: # %if.else.8.i.415
; CAPTABLE-NEXT:    # in Loop: Header=BB0_4 Depth=1
; CAPTABLE-NEXT:    clcbi $c12, %capcall20(add_force_to_mom)($c18)
; CAPTABLE-NEXT:    daddiu $4, $zero, 0
; CAPTABLE-NEXT:    cjalr $c12, $c17
; CAPTABLE-NEXT:    mov.d $f13, $f28
; CAPTABLE-NEXT:    bnez $zero, .LBB0_13
; CAPTABLE-NEXT:    nop
; CAPTABLE-NEXT:  # %bb.12: # %for.body.157.preheader
; CAPTABLE-NEXT:    # in Loop: Header=BB0_4 Depth=1
; CAPTABLE-NEXT:    clcbi $c12, %capcall20(scalar_mult_add_su3_vector)($c18)
; CAPTABLE-NEXT:    cjalr $c12, $c17
; CAPTABLE-NEXT:    mov.d $f12, $f25
; CAPTABLE-NEXT:  .LBB0_13: # %for.inc.172
; CAPTABLE-NEXT:    # in Loop: Header=BB0_4 Depth=1
; CAPTABLE-NEXT:    bnez $zero, .LBB0_4
; CAPTABLE-NEXT:    nop
; CAPTABLE-NEXT:  # %bb.14: # %for.end.174
; CAPTABLE-NEXT:    b .LBB0_21
; CAPTABLE-NEXT:    nop
; CAPTABLE-NEXT:  .LBB0_15: # %for.body.197
; CAPTABLE-NEXT:    # =>This Inner Loop Header: Depth=1
; CAPTABLE-NEXT:    clcbi $c12, %capcall20(scalar_mult_add_su3_vector)($c18)
; CAPTABLE-NEXT:    cjalr $c12, $c17
; CAPTABLE-NEXT:    mov.d $f12, $f25
; CAPTABLE-NEXT:    b .LBB0_15
; CAPTABLE-NEXT:    nop
; CAPTABLE-NEXT:  .LBB0_16: # %if.then.77
; CAPTABLE-NEXT:    clcbi $c12, %capcall20(add_force_to_mom)($c18)
; CAPTABLE-NEXT:    cjalr $c12, $c17
; CAPTABLE-NEXT:    daddiu $4, $zero, 0
; CAPTABLE-NEXT:  .LBB0_17: # %if.then.6.i.413
; CAPTABLE-NEXT:    .insn
; CAPTABLE-NEXT:  .LBB0_18: # %if.else.i.critedge
; CAPTABLE-NEXT:    .insn
; CAPTABLE-NEXT:  .LBB0_19: # %for.body.24.lr.ph
; CAPTABLE-NEXT:    .insn
; CAPTABLE-NEXT:  .LBB0_20: # %if.then.48
; CAPTABLE-NEXT:    clcbi $c12, %capcall20(add_force_to_mom)($c18)
; CAPTABLE-NEXT:    cjalr $c12, $c17
; CAPTABLE-NEXT:    daddiu $4, $zero, 0
; CAPTABLE-NEXT:  .LBB0_21: # %if.then.182
; CAPTABLE-NEXT:    clcbi $c12, %capcall20(add_force_to_mom)($c18)
; CAPTABLE-NEXT:    daddiu $4, $zero, 0
; CAPTABLE-NEXT:    cjalr $c12, $c17
; CAPTABLE-NEXT:    mov.d $f13, $f24

entry:
  %0 = load double, double addrspace(200)* getelementptr inbounds ([6 x double], [6 x double] addrspace(200)* @act_path_coeff, i64 0, i64 5), align 8
  %mul5 = fmul double undef, undef
  %mul6 = fmul double undef, 0.000000e+00
  %mul7 = fmul double undef, %0
  br i1 undef, label %for.body.24.lr.ph, label %for.cond.30.preheader

for.body.24.lr.ph:                                ; preds = %entry
  unreachable

for.cond.30.preheader:                            ; preds = %entry
  %sub51 = fsub double -0.000000e+00, undef
  %sub116 = fsub double -0.000000e+00, %mul6
  %div124 = fdiv double %mul6, %mul5
  %sub148 = fsub double -0.000000e+00, %mul5
  %div153 = fdiv double %mul5, undef
  %div193 = fdiv double %mul7, undef
  %cmp46 = icmp slt i64 0, 4
  br label %for.body.37

for.body.37:                                      ; preds = %for.inc.246, %for.cond.30.preheader
  %or.cond464 = or i1 undef, undef
  br i1 %or.cond464, label %for.inc.246, label %if.then

if.then:                                          ; preds = %for.body.37
  tail call void @u_shift_fermion(%struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef, i32 signext undef)
  br i1 %cmp46, label %if.then.48, label %for.body.55

if.then.48:                                       ; preds = %if.then
  tail call void @add_force_to_mom(%struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef, i32 signext undef, double %sub51)
  unreachable

for.body.55:                                      ; preds = %for.inc.172, %if.then
  br i1 undef, label %for.inc.172, label %if.then.69

if.then.69:                                       ; preds = %for.body.55
  br i1 %cmp46, label %if.then.77, label %for.body.84

if.then.77:                                       ; preds = %if.then.69
  tail call void @add_force_to_mom(%struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef, i32 signext undef, double %mul5)
  unreachable

for.body.84:                                      ; preds = %for.inc.143, %if.then.69
  br i1 undef, label %for.inc.143, label %if.then.105

if.then.105:                                      ; preds = %for.body.84
  br i1 %cmp46, label %if.then.113, label %if.else.i.critedge

if.then.113:                                      ; preds = %if.then.105
  tail call void @add_force_to_mom(%struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef, i32 signext undef, double %sub116)
  tail call void @add_force_to_mom(%struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef, i32 signext undef, double %mul6) #3
  br i1 undef, label %for.body.128, label %for.inc.143

if.else.i.critedge:                               ; preds = %if.then.105
  unreachable

for.body.128:                                     ; preds = %for.body.128, %if.then.113
  tail call void @scalar_mult_add_su3_vector(%struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef, double %div124, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef) #3
  br i1 undef, label %for.body.128, label %for.inc.143

for.inc.143:                                      ; preds = %for.body.128, %if.then.113, %for.body.84
  br i1 undef, label %for.end.145, label %for.body.84

for.end.145:                                      ; preds = %for.inc.143
  br i1 %cmp46, label %if.then.6.i.413, label %if.else.8.i.415

if.then.6.i.413:                                  ; preds = %for.end.145
  unreachable

if.else.8.i.415:                                  ; preds = %for.end.145
  tail call void @add_force_to_mom(%struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef, i32 signext undef, double %sub148) #3
  br i1 undef, label %for.body.157, label %for.inc.172

for.body.157:                                     ; preds = %for.body.157, %if.else.8.i.415
  tail call void @scalar_mult_add_su3_vector(%struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef, double %div153, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef) #3
  br i1 undef, label %for.body.157, label %for.inc.172

for.inc.172:                                      ; preds = %for.body.157, %if.else.8.i.415, %for.body.55
  %exitcond470 = icmp eq i32 undef, 8
  br i1 %exitcond470, label %for.end.174, label %for.body.55

for.end.174:                                      ; preds = %for.inc.172
  br i1 %cmp46, label %if.then.182, label %if.end.185

if.then.182:                                      ; preds = %for.end.174
  tail call void @add_force_to_mom(%struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef, i32 signext undef, double %mul7)
  unreachable

if.end.185:                                       ; preds = %for.end.174
  br label %for.body.197

for.body.197:                                     ; preds = %for.body.197, %if.end.185
  tail call void @scalar_mult_add_su3_vector(%struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef, double %div193, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)* undef) #3
  br label %for.body.197

for.inc.246:                                      ; preds = %for.body.37
  br label %for.body.37
}

; Function Attrs: nounwind
declare void @u_shift_fermion(%struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)*, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)*, i32 signext) #1

; Function Attrs: nounwind
declare void @add_force_to_mom(%struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)*, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)*, i32 signext, double) #1

declare void @scalar_mult_add_su3_vector(%struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)*, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)*, double, %struct.su3_vector.4.58.88.166.202.226.256.328.346.442.778 addrspace(200)*) #2
