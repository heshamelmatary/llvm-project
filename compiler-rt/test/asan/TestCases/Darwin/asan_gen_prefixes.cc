// Make sure __asan_gen_* strings have the correct prefixes on Darwin
// ("L" in __TEXT,__cstring, "l" in __TEXT,__const

// RUN: %clang_asan %s -S -o %t.s
// RUN: cat %t.s | FileCheck %s || exit 1

// UNSUPPORTED: ios

int x, y, z;
int main() { return 0; }
// CHECK: .section{{.*}}__TEXT,__const
// CHECK: l___asan_gen_
// CHECK: .section{{.*}}__TEXT,__cstring,cstring_literals
// CHECK: L___asan_gen_
// CHECK: L___asan_gen_
// CHECK: L___asan_gen_
