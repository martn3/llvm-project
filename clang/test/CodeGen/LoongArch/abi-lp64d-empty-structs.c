// RUN: %clang_cc1 -triple loongarch64 -target-feature +f -target-feature +d -target-abi lp64d -emit-llvm %s -o - | \
// RUN:   FileCheck --check-prefix=CHECK-C %s
// RUN: %clang_cc1 -triple loongarch64 -target-feature +f -target-feature +d -target-abi lp64d -emit-llvm %s -o - -x c++ | \
// RUN:   FileCheck --check-prefix=CHECK-CXX %s

// Fields containing empty structs or unions are ignored when flattening
// structs to examine whether the structs can be passed via FARs, even in C++.
// But there is an exception that non-zero-length array of empty structures are
// not ignored in C++. These rules are not documented in psABI <https://www.github.com/loongson/la-abi-specs>
// but they match GCC behaviours.

#include <stdint.h>

struct empty { struct { struct { } e; }; };
struct s1 { struct empty e; float f; };

// CHECK-C: define{{.*}} float @test_s1(float {{.*}})
// CHECK-CXX: define{{.*}} float @_Z7test_s12s1(float {{.*}})
struct s1 test_s1(struct s1 a) {
  return a;
}

struct s2 { struct empty e; int32_t i; float f; };

// CHECK-C: define{{.*}} { i32, float } @test_s2(i32 {{.*}}, float {{.*}})
// CHECK-CXX: define{{.*}} { i32, float } @_Z7test_s22s2(i32 {{.*}}, float {{.*}})
struct s2 test_s2(struct s2 a) {
  return a;
}

struct s3 { struct empty e; float f; float g; };

// CHECK-C: define{{.*}} { float, float } @test_s3(float {{.*}}, float {{.*}})
// CHECK-CXX: define{{.*}} { float, float } @_Z7test_s32s3(float {{.*}}, float {{.*}})
struct s3 test_s3(struct s3 a) {
  return a;
}

struct s4 { struct empty e; float __complex__ c; };

// CHECK-C: define{{.*}} { float, float } @test_s4(float {{.*}}, float {{.*}})
// CHECK-CXX: define{{.*}} { float, float } @_Z7test_s42s4(float {{.*}}, float {{.*}})
struct s4 test_s4(struct s4 a) {
  return a;
}

// An array of empty fields isn't ignored in C++ (this isn't explicit in the
// psABI, but matches observed g++ behaviour).

struct s5 { struct empty e[1]; float f; };

// CHECK-C: define{{.*}} float @test_s5(float {{.*}})
// CHECK-CXX: define{{.*}} i64 @_Z7test_s52s5(i64 {{.*}})
struct s5 test_s5(struct s5 a) {
  return a;
}

struct empty_arr { struct { struct { } e[1]; }; };
struct s6 { struct empty_arr e; float f; };

// CHECK-C: define{{.*}} float @test_s6(float {{.*}})
// CHECK-CXX: define{{.*}} i64 @_Z7test_s62s6(i64 {{.*}})
struct s6 test_s6(struct s6 a) {
  return a;
}

struct s7 { struct empty e[0]; float f; };

// CHECK-C: define{{.*}} float @test_s7(float {{.*}})
// CHECK-CXX: define{{.*}} float @_Z7test_s72s7(float {{.*}})
struct s7 test_s7(struct s7 a) {
  return a;
}

struct empty_arr0 { struct { struct { } e[0]; }; };
struct s8 { struct empty_arr0 e; float f; };

// CHECK-C: define{{.*}} float @test_s8(float {{.*}})
// CHECK-CXX: define{{.*}} float @_Z7test_s82s8(float {{.*}})
struct s8 test_s8(struct s8 a) {
  return a;
}

/// Note: Below tests check how empty structs are passed while above tests check
/// empty structs as fields of container struct are ignored when flattening
/// structs to examine whether the container structs can be passed via FARs.

// CHECK-C: define{{.*}} void @test_s9()
// CHECK-CXX: define{{.*}} i64 @_Z7test_s92s9(i64 {{.*}})
struct s9 { struct empty e; };
struct s9 test_s9(struct s9 a) {
  return a;
}

// CHECK-C: define{{.*}} void @test_s10()
// CHECK-CXX: define{{.*}} i64 @_Z8test_s103s10(i64 {{.*}})
struct s10 { };
struct s10 test_s10(struct s10 a) {
  return a;
}

// CHECK-C: define{{.*}} void @test_s11()
// CHECK-CXX: define{{.*}} i64 @_Z8test_s113s11(i64 {{.*}})
struct s11 { struct { } s; };
struct s11 test_s11(struct s11 a) {
  return a;
}

// CHECK-C: define{{.*}} void @test_s12()
// CHECK-CXX: define{{.*}} void @_Z8test_s123s12()
struct s12 { int i[0]; };
struct s12 test_s12(struct s12 a) {
  return a;
}

// CHECK-C: define{{.*}} void @test_s13()
// CHECK-CXX: define{{.*}} void @_Z8test_s133s13()
struct s13 { struct { } s[0]; };
struct s13 test_s13(struct s13 a) {
  return a;
}

// CHECK-C: define{{.*}} void @test_s14()
// CHECK-CXX: define{{.*}} i64 @_Z8test_s143s14(i64 {{.*}})
struct s14 { struct { } s[1]; };
struct s14 test_s14(struct s14 a) {
  return a;
}

// CHECK-C: define{{.*}} void @test_s15()
// CHECK-CXX: define{{.*}} i64 @_Z8test_s153s15(i64 {{.*}})
struct s15 { int : 0; };
struct s15 test_s15(struct s15 a) {
  return a;
}

// CHECK-C: define{{.*}} i64 @test_s16(i64 {{.*}})
// CHECK-CXX: define{{.*}} i64 @_Z8test_s163s16(i64 {{.*}})
struct s16 { int : 1; };
struct s16 test_s16(struct s16 a) {
  return a;
}
