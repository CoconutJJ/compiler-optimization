; ModuleID = '<stdin>'
source_filename = "sample-code/matmul.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

@__const.main.m1 = private unnamed_addr constant [3 x [3 x double]] [[3 x double] [double 1.000000e+00, double 2.000000e+00, double 3.000000e+00], [3 x double] [double 3.000000e+00, double 2.000000e+00, double 1.000000e+00], [3 x double] [double 4.000000e+00, double 5.000000e+00, double 6.000000e+00]], align 8
@__const.main.m2 = private unnamed_addr constant [3 x [3 x double]] [[3 x double] [double 4.000000e+00, double 5.000000e+00, double 6.000000e+00], [3 x double] [double 4.000000e+00, double 3.000000e+00, double 2.000000e+00], [3 x double] [double 2.000000e+00, double 3.000000e+00, double 4.000000e+00]], align 8

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main(i32 noundef %0, ptr noundef %1) #0 {
  %3 = alloca [3 x [3 x double]], align 8
  %4 = alloca [3 x [3 x double]], align 8
  %5 = alloca [3 x [3 x double]], align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 8 %3, ptr align 8 @__const.main.m1, i64 72, i1 false)
  call void @llvm.memcpy.p0.p0.i64(ptr align 8 %4, ptr align 8 @__const.main.m2, i64 72, i1 false)
  call void @llvm.memset.p0.i64(ptr align 8 %5, i8 0, i64 72, i1 false)
  br label %6

6:                                                ; preds = %31, %2
  %.02 = phi i64 [ 0, %2 ], [ %32, %31 ]
  %7 = icmp ult i64 %.02, 3
  br i1 %7, label %8, label %33

8:                                                ; preds = %6
  br label %9

9:                                                ; preds = %28, %8
  %.01 = phi i64 [ 0, %8 ], [ %29, %28 ]
  %10 = icmp ult i64 %.01, 3
  br i1 %10, label %11, label %30

11:                                               ; preds = %9
  br label %12

12:                                               ; preds = %25, %11
  %.0 = phi i64 [ 0, %11 ], [ %26, %25 ]
  %13 = icmp ult i64 %.0, 3
  br i1 %13, label %14, label %27

14:                                               ; preds = %12
  %15 = getelementptr inbounds [3 x [3 x double]], ptr %3, i64 0, i64 %.02
  %16 = getelementptr inbounds [3 x double], ptr %15, i64 0, i64 %.0
  %17 = load double, ptr %16, align 8
  %18 = getelementptr inbounds [3 x [3 x double]], ptr %4, i64 0, i64 %.0
  %19 = getelementptr inbounds [3 x double], ptr %18, i64 0, i64 %.01
  %20 = load double, ptr %19, align 8
  %21 = getelementptr inbounds [3 x [3 x double]], ptr %5, i64 0, i64 %.02
  %22 = getelementptr inbounds [3 x double], ptr %21, i64 0, i64 %.01
  %23 = load double, ptr %22, align 8
  %24 = call double @llvm.fmuladd.f64(double %17, double %20, double %23)
  store double %24, ptr %22, align 8
  br label %25

25:                                               ; preds = %14
  %26 = add i64 %.0, 1
  br label %12, !llvm.loop !6

27:                                               ; preds = %12
  br label %28

28:                                               ; preds = %27
  %29 = add i64 %.01, 1
  br label %9, !llvm.loop !8

30:                                               ; preds = %9
  br label %31

31:                                               ; preds = %30
  %32 = add i64 %.02, 1
  br label %6, !llvm.loop !9

33:                                               ; preds = %6
  %34 = getelementptr inbounds [3 x [3 x double]], ptr %5, i64 0, i64 1
  %35 = getelementptr inbounds [3 x double], ptr %34, i64 0, i64 1
  %36 = load double, ptr %35, align 8
  %37 = fptosi double %36 to i32
  ret i32 %37
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #1

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #2

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.fmuladd.f64(double, double, double) #3

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "min-legal-vector-width"="0" "no-trapping-math"="true" "probe-stack"="__chkstk_darwin" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #3 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 2, !"SDK Version", [2 x i32] [i32 14, i32 4]}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 8, !"PIC Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{i32 7, !"frame-pointer", i32 1}
!5 = !{!"Apple clang version 15.0.0 (clang-1500.3.9.4)"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
!8 = distinct !{!8, !7}
!9 = distinct !{!9, !7}
