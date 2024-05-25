; ModuleID = 'matmul.c'
source_filename = "matmul.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

@__const.main.m1 = private unnamed_addr constant [3 x [3 x double]] [[3 x double] [double 1.000000e+00, double 2.000000e+00, double 3.000000e+00], [3 x double] [double 3.000000e+00, double 2.000000e+00, double 1.000000e+00], [3 x double] [double 4.000000e+00, double 5.000000e+00, double 6.000000e+00]], align 8
@__const.main.m2 = private unnamed_addr constant [3 x [3 x double]] [[3 x double] [double 4.000000e+00, double 5.000000e+00, double 6.000000e+00], [3 x double] [double 4.000000e+00, double 3.000000e+00, double 2.000000e+00], [3 x double] [double 2.000000e+00, double 3.000000e+00, double 4.000000e+00]], align 8

; Function Attrs: noinline nounwind optnone ssp uwtable(sync)
define i32 @main(i32 noundef %0, ptr noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca ptr, align 8
  %6 = alloca [3 x [3 x double]], align 8
  %7 = alloca [3 x [3 x double]], align 8
  %8 = alloca [3 x [3 x double]], align 8
  %9 = alloca i64, align 8
  %10 = alloca i64, align 8
  %11 = alloca i64, align 8
  store i32 0, ptr %3, align 4
  store i32 %0, ptr %4, align 4
  store ptr %1, ptr %5, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 8 %6, ptr align 8 @__const.main.m1, i64 72, i1 false)
  call void @llvm.memcpy.p0.p0.i64(ptr align 8 %7, ptr align 8 @__const.main.m2, i64 72, i1 false)
  call void @llvm.memset.p0.i64(ptr align 8 %8, i8 0, i64 72, i1 false)
  store i64 0, ptr %9, align 8
  br label %12

12:                                               ; preds = %48, %2
  %13 = load i64, ptr %9, align 8
  %14 = icmp ult i64 %13, 3
  br i1 %14, label %15, label %51

15:                                               ; preds = %12
  store i64 0, ptr %10, align 8
  br label %16

16:                                               ; preds = %44, %15
  %17 = load i64, ptr %10, align 8
  %18 = icmp ult i64 %17, 3
  br i1 %18, label %19, label %47

19:                                               ; preds = %16
  store i64 0, ptr %11, align 8
  br label %20

20:                                               ; preds = %40, %19
  %21 = load i64, ptr %11, align 8
  %22 = icmp ult i64 %21, 3
  br i1 %22, label %23, label %43

23:                                               ; preds = %20
  %24 = load i64, ptr %9, align 8
  %25 = getelementptr inbounds [3 x [3 x double]], ptr %6, i64 0, i64 %24
  %26 = load i64, ptr %11, align 8
  %27 = getelementptr inbounds [3 x double], ptr %25, i64 0, i64 %26
  %28 = load double, ptr %27, align 8
  %29 = load i64, ptr %11, align 8
  %30 = getelementptr inbounds [3 x [3 x double]], ptr %7, i64 0, i64 %29
  %31 = load i64, ptr %10, align 8
  %32 = getelementptr inbounds [3 x double], ptr %30, i64 0, i64 %31
  %33 = load double, ptr %32, align 8
  %34 = load i64, ptr %9, align 8
  %35 = getelementptr inbounds [3 x [3 x double]], ptr %8, i64 0, i64 %34
  %36 = load i64, ptr %10, align 8
  %37 = getelementptr inbounds [3 x double], ptr %35, i64 0, i64 %36
  %38 = load double, ptr %37, align 8
  %39 = call double @llvm.fmuladd.f64(double %28, double %33, double %38)
  store double %39, ptr %37, align 8
  br label %40

40:                                               ; preds = %23
  %41 = load i64, ptr %11, align 8
  %42 = add i64 %41, 1
  store i64 %42, ptr %11, align 8
  br label %20, !llvm.loop !6

43:                                               ; preds = %20
  br label %44

44:                                               ; preds = %43
  %45 = load i64, ptr %10, align 8
  %46 = add i64 %45, 1
  store i64 %46, ptr %10, align 8
  br label %16, !llvm.loop !8

47:                                               ; preds = %16
  br label %48

48:                                               ; preds = %47
  %49 = load i64, ptr %9, align 8
  %50 = add i64 %49, 1
  store i64 %50, ptr %9, align 8
  br label %12, !llvm.loop !9

51:                                               ; preds = %12
  %52 = getelementptr inbounds [3 x [3 x double]], ptr %8, i64 0, i64 1
  %53 = getelementptr inbounds [3 x double], ptr %52, i64 0, i64 1
  %54 = load double, ptr %53, align 8
  %55 = fptosi double %54 to i32
  ret i32 %55
}

; Function Attrs: argmemonly nocallback nofree nounwind willreturn
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #1

; Function Attrs: argmemonly nocallback nofree nounwind willreturn writeonly
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #2

; Function Attrs: nocallback nofree nosync nounwind readnone speculatable willreturn
declare double @llvm.fmuladd.f64(double, double, double) #3

attributes #0 = { noinline nounwind optnone ssp uwtable(sync) "frame-pointer"="non-leaf" "min-legal-vector-width"="0" "no-trapping-math"="true" "probe-stack"="__chkstk_darwin" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { argmemonly nocallback nofree nounwind willreturn }
attributes #2 = { argmemonly nocallback nofree nounwind willreturn writeonly }
attributes #3 = { nocallback nofree nosync nounwind readnone speculatable willreturn }

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
