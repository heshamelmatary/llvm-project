; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: sed 's/iXLEN/i32/g' %s | %riscv32_cheri_llc -verify-machineinstrs \
; RUN:   | FileCheck --check-prefix=CHECK-ILP32 %s
; RUN: sed 's/iXLEN/i64/g' %s | %riscv64_cheri_llc -verify-machineinstrs \
; RUN:   | FileCheck --check-prefix=CHECK-LP64 %s
; RUN: sed 's/iXLEN/i32/g' %s | %riscv32_cheri_purecap_llc -verify-machineinstrs \
; RUN:   | FileCheck --check-prefix=CHECK-IL32PC64 %s
; RUN: sed 's/iXLEN/i64/g' %s | %riscv64_cheri_purecap_llc -verify-machineinstrs \
; RUN:   | FileCheck --check-prefix=CHECK-L64PC128 %s

; Capability-Inspection Instructions

declare iXLEN @llvm.cheri.cap.perms.get.iXLEN(i8 addrspace(200) *)
declare iXLEN @llvm.cheri.cap.type.get.iXLEN(i8 addrspace(200) *)
declare iXLEN @llvm.cheri.cap.base.get.iXLEN(i8 addrspace(200) *)
declare iXLEN @llvm.cheri.cap.length.get.iXLEN(i8 addrspace(200) *)
declare i1 @llvm.cheri.cap.tag.get(i8 addrspace(200) *)
declare i1 @llvm.cheri.cap.sealed.get(i8 addrspace(200) *)
declare iXLEN @llvm.cheri.cap.offset.get.iXLEN(i8 addrspace(200) *)
declare iXLEN @llvm.cheri.cap.flags.get.iXLEN(i8 addrspace(200) *)
declare iXLEN @llvm.cheri.cap.address.get.iXLEN(i8 addrspace(200) *)

define iXLEN @perms_get(i8 addrspace(200) *%cap) nounwind {
; CHECK-ILP32-LABEL: perms_get:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    cgetperm a0, ca0
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: perms_get:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    cgetperm a0, ca0
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: perms_get:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    cgetperm a0, ca0
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: perms_get:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    cgetperm a0, ca0
; CHECK-L64PC128-NEXT:    cret
  %perms = call iXLEN @llvm.cheri.cap.perms.get.iXLEN(i8 addrspace(200) *%cap)
  ret iXLEN %perms
}

define iXLEN @type_get(i8 addrspace(200) *%cap) nounwind {
; CHECK-ILP32-LABEL: type_get:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    cgettype a0, ca0
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: type_get:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    cgettype a0, ca0
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: type_get:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    cgettype a0, ca0
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: type_get:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    cgettype a0, ca0
; CHECK-L64PC128-NEXT:    cret
  %type = call iXLEN @llvm.cheri.cap.type.get.iXLEN(i8 addrspace(200) *%cap)
  ret iXLEN %type
}

define iXLEN @base_get(i8 addrspace(200) *%cap) nounwind {
; CHECK-ILP32-LABEL: base_get:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    cgetbase a0, ca0
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: base_get:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    cgetbase a0, ca0
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: base_get:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    cgetbase a0, ca0
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: base_get:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    cgetbase a0, ca0
; CHECK-L64PC128-NEXT:    cret
  %base = call iXLEN @llvm.cheri.cap.base.get.iXLEN(i8 addrspace(200) *%cap)
  ret iXLEN %base
}

define iXLEN @length_get(i8 addrspace(200) *%cap) nounwind {
; CHECK-ILP32-LABEL: length_get:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    cgetlen a0, ca0
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: length_get:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    cgetlen a0, ca0
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: length_get:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    cgetlen a0, ca0
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: length_get:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    cgetlen a0, ca0
; CHECK-L64PC128-NEXT:    cret
  %length = call iXLEN @llvm.cheri.cap.length.get.iXLEN(i8 addrspace(200) *%cap)
  ret iXLEN %length
}

define iXLEN @tag_get(i8 addrspace(200) *%cap) nounwind {
; CHECK-ILP32-LABEL: tag_get:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    cgettag a0, ca0
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: tag_get:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    cgettag a0, ca0
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: tag_get:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    cgettag a0, ca0
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: tag_get:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    cgettag a0, ca0
; CHECK-L64PC128-NEXT:    cret
  %tag = call i1 @llvm.cheri.cap.tag.get(i8 addrspace(200) *%cap)
  %tag.zext = zext i1 %tag to iXLEN
  ret iXLEN %tag.zext
}

define iXLEN @sealed_get(i8 addrspace(200) *%cap) nounwind {
; CHECK-ILP32-LABEL: sealed_get:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    cgetsealed a0, ca0
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: sealed_get:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    cgetsealed a0, ca0
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: sealed_get:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    cgetsealed a0, ca0
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: sealed_get:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    cgetsealed a0, ca0
; CHECK-L64PC128-NEXT:    cret
  %sealed = call i1 @llvm.cheri.cap.sealed.get(i8 addrspace(200) *%cap)
  %sealed.zext = zext i1 %sealed to iXLEN
  ret iXLEN %sealed.zext
}

define iXLEN @offset_get(i8 addrspace(200) *%cap) nounwind {
; CHECK-ILP32-LABEL: offset_get:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    cgetoffset a0, ca0
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: offset_get:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    cgetoffset a0, ca0
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: offset_get:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    cgetoffset a0, ca0
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: offset_get:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    cgetoffset a0, ca0
; CHECK-L64PC128-NEXT:    cret
  %offset = call iXLEN @llvm.cheri.cap.offset.get.iXLEN(i8 addrspace(200) *%cap)
  ret iXLEN %offset
}

define iXLEN @flags_get(i8 addrspace(200) *%cap) nounwind {
; CHECK-ILP32-LABEL: flags_get:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    cgetflags a0, ca0
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: flags_get:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    cgetflags a0, ca0
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: flags_get:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    cgetflags a0, ca0
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: flags_get:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    cgetflags a0, ca0
; CHECK-L64PC128-NEXT:    cret
  %flags = call iXLEN @llvm.cheri.cap.flags.get.iXLEN(i8 addrspace(200) *%cap)
  ret iXLEN %flags
}

define iXLEN @address_get(i8 addrspace(200) *%cap) nounwind {
; CHECK-ILP32-LABEL: address_get:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    cgetaddr a0, ca0
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: address_get:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    cgetaddr a0, ca0
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: address_get:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    cgetaddr a0, ca0
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: address_get:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    cgetaddr a0, ca0
; CHECK-L64PC128-NEXT:    cret
  %address = call iXLEN @llvm.cheri.cap.address.get.iXLEN(i8 addrspace(200) *%cap)
  ret iXLEN %address
}

; Capability-Modification Instructions

declare i8 addrspace(200) *@llvm.cheri.cap.seal(i8 addrspace(200) *, i8 addrspace(200) *)
declare i8 addrspace(200) *@llvm.cheri.cap.unseal(i8 addrspace(200) *, i8 addrspace(200) *)
declare i8 addrspace(200) *@llvm.cheri.cap.perms.and.iXLEN(i8 addrspace(200) *, iXLEN)
declare i8 addrspace(200) *@llvm.cheri.cap.flags.set.iXLEN(i8 addrspace(200) *, iXLEN)
declare i8 addrspace(200) *@llvm.cheri.cap.offset.set.iXLEN(i8 addrspace(200) *, iXLEN)
declare i8 addrspace(200) *@llvm.cheri.cap.address.set.iXLEN(i8 addrspace(200) *, iXLEN)
declare i8 addrspace(200) *@llvm.cheri.cap.bounds.set.iXLEN(i8 addrspace(200) *, iXLEN)
declare i8 addrspace(200) *@llvm.cheri.cap.bounds.set.exact.iXLEN(i8 addrspace(200) *, iXLEN)
declare i8 addrspace(200) *@llvm.cheri.cap.tag.clear(i8 addrspace(200) *)
declare i8 addrspace(200) *@llvm.cheri.cap.build(i8 addrspace(200) *, i8 addrspace(200) *)
declare i8 addrspace(200) *@llvm.cheri.cap.type.copy(i8 addrspace(200) *, i8 addrspace(200) *)
declare i8 addrspace(200) *@llvm.cheri.cap.conditional.seal(i8 addrspace(200) *, i8 addrspace(200) *)

define i8 addrspace(200) *@seal(i8 addrspace(200) *%cap1, i8 addrspace(200) *%cap2) nounwind {
; CHECK-ILP32-LABEL: seal:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    cseal ca0, ca0, ca1
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: seal:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    cseal ca0, ca0, ca1
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: seal:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    cseal ca0, ca0, ca1
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: seal:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    cseal ca0, ca0, ca1
; CHECK-L64PC128-NEXT:    cret
  %sealed = call i8 addrspace(200) *@llvm.cheri.cap.seal(i8 addrspace(200) *%cap1, i8 addrspace(200) *%cap2)
  ret i8 addrspace(200) *%sealed
}

define i8 addrspace(200) *@unseal(i8 addrspace(200) *%cap1, i8 addrspace(200) *%cap2) nounwind {
; CHECK-ILP32-LABEL: unseal:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    cunseal ca0, ca0, ca1
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: unseal:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    cunseal ca0, ca0, ca1
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: unseal:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    cunseal ca0, ca0, ca1
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: unseal:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    cunseal ca0, ca0, ca1
; CHECK-L64PC128-NEXT:    cret
  %unsealed = call i8 addrspace(200) *@llvm.cheri.cap.unseal(i8 addrspace(200) *%cap1, i8 addrspace(200) *%cap2)
  ret i8 addrspace(200) *%unsealed
}

define i8 addrspace(200) *@perms_and(i8 addrspace(200) *%cap, iXLEN %perms) nounwind {
; CHECK-ILP32-LABEL: perms_and:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    candperm ca0, ca0, a1
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: perms_and:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    candperm ca0, ca0, a1
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: perms_and:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    candperm ca0, ca0, a1
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: perms_and:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    candperm ca0, ca0, a1
; CHECK-L64PC128-NEXT:    cret
  %newcap = call i8 addrspace(200) *@llvm.cheri.cap.perms.and.iXLEN(i8 addrspace(200) *%cap, iXLEN %perms)
  ret i8 addrspace(200) *%newcap
}

define i8 addrspace(200) *@flags_set(i8 addrspace(200) *%cap, iXLEN %flags) nounwind {
; CHECK-ILP32-LABEL: flags_set:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    csetflags ca0, ca0, a1
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: flags_set:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    csetflags ca0, ca0, a1
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: flags_set:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    csetflags ca0, ca0, a1
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: flags_set:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    csetflags ca0, ca0, a1
; CHECK-L64PC128-NEXT:    cret
  %newcap = call i8 addrspace(200) *@llvm.cheri.cap.flags.set.iXLEN(i8 addrspace(200) *%cap, iXLEN %flags)
  ret i8 addrspace(200) *%newcap
}

define i8 addrspace(200) *@offset_set(i8 addrspace(200) *%cap, iXLEN %offset) nounwind {
; CHECK-ILP32-LABEL: offset_set:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    csetoffset ca0, ca0, a1
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: offset_set:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    csetoffset ca0, ca0, a1
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: offset_set:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    csetoffset ca0, ca0, a1
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: offset_set:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    csetoffset ca0, ca0, a1
; CHECK-L64PC128-NEXT:    cret
  %newcap = call i8 addrspace(200) *@llvm.cheri.cap.offset.set.iXLEN(i8 addrspace(200) *%cap, iXLEN %offset)
  ret i8 addrspace(200) *%newcap
}

define i8 addrspace(200) *@address_set(i8 addrspace(200) *%cap, iXLEN %address) nounwind {
; CHECK-ILP32-LABEL: address_set:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    csetaddr ca0, ca0, a1
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: address_set:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    csetaddr ca0, ca0, a1
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: address_set:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    csetaddr ca0, ca0, a1
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: address_set:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    csetaddr ca0, ca0, a1
; CHECK-L64PC128-NEXT:    cret
  %newcap = call i8 addrspace(200) *@llvm.cheri.cap.address.set.iXLEN(i8 addrspace(200) *%cap, iXLEN %address)
  ret i8 addrspace(200) *%newcap
}

define i8 addrspace(200) *@bounds_set(i8 addrspace(200) *%cap, iXLEN %bounds) nounwind {
; CHECK-ILP32-LABEL: bounds_set:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    csetbounds ca0, ca0, a1
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: bounds_set:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    csetbounds ca0, ca0, a1
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: bounds_set:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    csetbounds ca0, ca0, a1
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: bounds_set:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    csetbounds ca0, ca0, a1
; CHECK-L64PC128-NEXT:    cret
  %newcap = call i8 addrspace(200) *@llvm.cheri.cap.bounds.set.iXLEN(i8 addrspace(200) *%cap, iXLEN %bounds)
  ret i8 addrspace(200) *%newcap
}

define i8 addrspace(200) *@bounds_set_exact(i8 addrspace(200) *%cap, iXLEN %bounds) nounwind {
; CHECK-ILP32-LABEL: bounds_set_exact:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    csetboundsexact ca0, ca0, a1
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: bounds_set_exact:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    csetboundsexact ca0, ca0, a1
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: bounds_set_exact:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    csetboundsexact ca0, ca0, a1
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: bounds_set_exact:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    csetboundsexact ca0, ca0, a1
; CHECK-L64PC128-NEXT:    cret
  %newcap = call i8 addrspace(200) *@llvm.cheri.cap.bounds.set.exact.iXLEN(i8 addrspace(200) *%cap, iXLEN %bounds)
  ret i8 addrspace(200) *%newcap
}

define i8 addrspace(200) *@bounds_set_immediate(i8 addrspace(200) *%cap) nounwind {
; CHECK-ILP32-LABEL: bounds_set_immediate:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    csetbounds ca0, ca0, 42
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: bounds_set_immediate:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    csetbounds ca0, ca0, 42
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: bounds_set_immediate:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    csetbounds ca0, ca0, 42
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: bounds_set_immediate:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    csetbounds ca0, ca0, 42
; CHECK-L64PC128-NEXT:    cret
  %newcap = call i8 addrspace(200) *@llvm.cheri.cap.bounds.set.iXLEN(i8 addrspace(200) *%cap, iXLEN 42)
  ret i8 addrspace(200) *%newcap
}

define i8 addrspace(200) *@tag_clear(i8 addrspace(200) *%cap) nounwind {
; CHECK-ILP32-LABEL: tag_clear:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    ccleartag ca0, ca0
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: tag_clear:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    ccleartag ca0, ca0
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: tag_clear:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    ccleartag ca0, ca0
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: tag_clear:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    ccleartag ca0, ca0
; CHECK-L64PC128-NEXT:    cret
  %untagged = call i8 addrspace(200) *@llvm.cheri.cap.tag.clear(i8 addrspace(200) *%cap)
  ret i8 addrspace(200) *%untagged
}

define i8 addrspace(200) *@build(i8 addrspace(200) *%cap1, i8 addrspace(200) *%cap2) nounwind {
; CHECK-ILP32-LABEL: build:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    cbuildcap ca0, ca0, ca1
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: build:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    cbuildcap ca0, ca0, ca1
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: build:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    cbuildcap ca0, ca0, ca1
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: build:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    cbuildcap ca0, ca0, ca1
; CHECK-L64PC128-NEXT:    cret
  %built = call i8 addrspace(200) *@llvm.cheri.cap.build(i8 addrspace(200) *%cap1, i8 addrspace(200) *%cap2)
  ret i8 addrspace(200) *%built
}

define i8 addrspace(200) *@type_copy(i8 addrspace(200) *%cap1, i8 addrspace(200) *%cap2) nounwind {
; CHECK-ILP32-LABEL: type_copy:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    ccopytype ca0, ca0, ca1
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: type_copy:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    ccopytype ca0, ca0, ca1
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: type_copy:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    ccopytype ca0, ca0, ca1
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: type_copy:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    ccopytype ca0, ca0, ca1
; CHECK-L64PC128-NEXT:    cret
  %newcap = call i8 addrspace(200) *@llvm.cheri.cap.type.copy(i8 addrspace(200) *%cap1, i8 addrspace(200) *%cap2)
  ret i8 addrspace(200) *%newcap
}

define i8 addrspace(200) *@conditional_seal(i8 addrspace(200) *%cap1, i8 addrspace(200) *%cap2) nounwind {
; CHECK-ILP32-LABEL: conditional_seal:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    ccseal ca0, ca0, ca1
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: conditional_seal:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    ccseal ca0, ca0, ca1
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: conditional_seal:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    ccseal ca0, ca0, ca1
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: conditional_seal:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    ccseal ca0, ca0, ca1
; CHECK-L64PC128-NEXT:    cret
  %newcap = call i8 addrspace(200) *@llvm.cheri.cap.conditional.seal(i8 addrspace(200) *%cap1, i8 addrspace(200) *%cap2)
  ret i8 addrspace(200) *%newcap
}

; Pointer-Arithmetic Instructions

declare iXLEN @llvm.cheri.cap.to.pointer(i8 addrspace(200) *, i8 addrspace(200) *)
declare i8 addrspace(200) *@llvm.cheri.cap.from.pointer(i8 addrspace(200) *, iXLEN)
declare i8 addrspace(200) *@llvm.cheri.cap.from.ddc(iXLEN)
declare iXLEN @llvm.cheri.cap.diff(i8 addrspace(200) *, i8 addrspace(200) *)
declare i8 addrspace(200) *@llvm.cheri.ddc.get()
declare i8 addrspace(200) *@llvm.cheri.pcc.get()
declare i8 addrspace(200) *@llvm.cheri.stack.cap.get()

define iXLEN @to_pointer(i8 addrspace(200) *%cap1, i8 addrspace(200) *%cap2) nounwind {
; CHECK-ILP32-LABEL: to_pointer:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    ctoptr a0, ca0, ca1
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: to_pointer:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    ctoptr a0, ca0, ca1
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: to_pointer:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    ctoptr a0, ca0, ca1
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: to_pointer:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    ctoptr a0, ca0, ca1
; CHECK-L64PC128-NEXT:    cret
  %ptr = call iXLEN @llvm.cheri.cap.to.pointer(i8 addrspace(200) *%cap1, i8 addrspace(200) *%cap2)
  ret iXLEN %ptr
}

define i8 addrspace(200) *@from_pointer(i8 addrspace(200) *%cap, iXLEN %ptr) nounwind {
; CHECK-ILP32-LABEL: from_pointer:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    cfromptr ca0, ca0, a1
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: from_pointer:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    cfromptr ca0, ca0, a1
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: from_pointer:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    cfromptr ca0, ca0, a1
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: from_pointer:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    cfromptr ca0, ca0, a1
; CHECK-L64PC128-NEXT:    cret
  %newcap = call i8 addrspace(200) *@llvm.cheri.cap.from.pointer(i8 addrspace(200) *%cap, iXLEN %ptr)
  ret i8 addrspace(200) *%newcap
}

define i8 addrspace(200) *@from_ddc(iXLEN %ptr) nounwind {
; CHECK-ILP32-LABEL: from_ddc:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    cfromptr ca0, ddc, a0
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: from_ddc:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    cfromptr ca0, ddc, a0
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: from_ddc:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    cfromptr ca0, ddc, a0
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: from_ddc:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    cfromptr ca0, ddc, a0
; CHECK-L64PC128-NEXT:    cret
  %cap = call i8 addrspace(200) *@llvm.cheri.cap.from.ddc(iXLEN %ptr)
  ret i8 addrspace(200) *%cap
}

define iXLEN @diff(i8 addrspace(200) *%cap1, i8 addrspace(200) *%cap2) nounwind {
; CHECK-ILP32-LABEL: diff:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    csub a0, ca0, ca1
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: diff:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    csub a0, ca0, ca1
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: diff:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    csub a0, ca0, ca1
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: diff:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    csub a0, ca0, ca1
; CHECK-L64PC128-NEXT:    cret
  %diff = call iXLEN @llvm.cheri.cap.diff(i8 addrspace(200) *%cap1, i8 addrspace(200) *%cap2)
  ret iXLEN %diff
}

define i8 addrspace(200) *@ddc_get() nounwind {
; CHECK-ILP32-LABEL: ddc_get:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    cspecialr ca0, ddc
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: ddc_get:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    cspecialr ca0, ddc
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: ddc_get:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    cspecialr ca0, ddc
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: ddc_get:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    cspecialr ca0, ddc
; CHECK-L64PC128-NEXT:    cret
  %cap = call i8 addrspace(200) *@llvm.cheri.ddc.get()
  ret i8 addrspace(200) *%cap
}

define i8 addrspace(200) *@pcc_get() nounwind {
; CHECK-ILP32-LABEL: pcc_get:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    cspecialr ca0, pcc
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: pcc_get:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    cspecialr ca0, pcc
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: pcc_get:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    auipcc ca0, 0
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: pcc_get:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    auipcc ca0, 0
; CHECK-L64PC128-NEXT:    cret
  %cap = call i8 addrspace(200) *@llvm.cheri.pcc.get()
  ret i8 addrspace(200) *%cap
}

define i8 addrspace(200)* @stack_get() nounwind {
; CHECK-ILP32-LABEL: stack_get:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    cmove ca0, csp
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: stack_get:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    cmove ca0, csp
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: stack_get:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    cmove ca0, csp
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: stack_get:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    cmove ca0, csp
; CHECK-L64PC128-NEXT:    cret
  %cap = call i8 addrspace(200) *@llvm.cheri.stack.cap.get()
  ret i8 addrspace(200)* %cap
}

; Assertion Instructions

declare i1 @llvm.cheri.cap.subset.test(i8 addrspace(200) *%cap1, i8 addrspace(200) *%cap2)

define iXLEN @subset_test(i8 addrspace(200) *%cap1, i8 addrspace(200) *%cap2) nounwind {
; CHECK-ILP32-LABEL: subset_test:
; CHECK-ILP32:       # %bb.0:
; CHECK-ILP32-NEXT:    ctestsubset a0, ca0, ca1
; CHECK-ILP32-NEXT:    ret
;
; CHECK-LP64-LABEL: subset_test:
; CHECK-LP64:       # %bb.0:
; CHECK-LP64-NEXT:    ctestsubset a0, ca0, ca1
; CHECK-LP64-NEXT:    ret
;
; CHECK-IL32PC64-LABEL: subset_test:
; CHECK-IL32PC64:       # %bb.0:
; CHECK-IL32PC64-NEXT:    ctestsubset a0, ca0, ca1
; CHECK-IL32PC64-NEXT:    cret
;
; CHECK-L64PC128-LABEL: subset_test:
; CHECK-L64PC128:       # %bb.0:
; CHECK-L64PC128-NEXT:    ctestsubset a0, ca0, ca1
; CHECK-L64PC128-NEXT:    cret
  %subset = call i1 @llvm.cheri.cap.subset.test(i8 addrspace(200) *%cap1, i8 addrspace(200) *%cap2)
  %subset.zext = zext i1 %subset to iXLEN
  ret iXLEN %subset.zext
}
