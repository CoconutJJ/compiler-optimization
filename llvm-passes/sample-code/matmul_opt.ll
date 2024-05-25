; ModuleID = 'matmul.c'
source_filename = "matmul.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

@__const.main.m1 = private unnamed_addr constant [3 x [3 x double]] [[3 x double] [double 1.000000e+00, double 2.000000e+00, double 3.000000e+00], [3 x double] [double 3.000000e+00, double 2.000000e+00, double 1.000000e+00], [3 x double] [double 4.000000e+00, double 5.000000e+00, double 6.000000e+00]], align 8
@__const.main.m2 = private unnamed_addr constant [3 x [3 x double]] [[3 x double] [double 4.000000e+00, double 5.000000e+00, double 6.000000e+00], [3 x double] [double 4.000000e+00, double 3.000000e+00, double 2.000000e+00], [3 x double] [double 2.000000e+00, double 3.000000e+00, double 4.000000e+00]], align 8

; Function Attrs: nofree nosync nounwind readnone ssp uwtable(sync)
define i32 @main(i32 noundef %0, ptr nocapture noundef readnone %1) local_unnamed_addr #0 {
  %3 = alloca [3 x [3 x double]], align 8
  call void @llvm.lifetime.start.p0(i64 72, ptr nonnull %3) #4
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(72) %3, i8 0, i64 72, i1 false)
  br label %4

4:                                                ; preds = %2, %14
  %5 = phi i64 [ 0, %2 ], [ %15, %14 ]
  br label %10

6:                                                ; preds = %14
  %7 = getelementptr inbounds [3 x [3 x double]], ptr %3, i64 0, i64 1, i64 1
  %8 = load double, ptr %7, align 8, !tbaa !6
  %9 = fptosi double %8 to i32
  call void @llvm.lifetime.end.p0(i64 72, ptr nonnull %3) #4
  ret i32 %9

10:                                               ; preds = %4, %17
  %11 = phi i64 [ 0, %4 ], [ %18, %17 ]
  %12 = getelementptr inbounds [3 x [3 x double]], ptr %3, i64 0, i64 %5, i64 %11
  %13 = load double, ptr %12, align 8, !tbaa !6
  br label %20

14:                                               ; preds = %17
  %15 = add nuw nsw i64 %5, 1
  %16 = icmp eq i64 %15, 3
  br i1 %16, label %6, label %4, !llvm.loop !10

17:                                               ; preds = %20
  store double %27, ptr %12, align 8, !tbaa !6
  %18 = add nuw nsw i64 %11, 1
  %19 = icmp eq i64 %18, 3
  br i1 %19, label %14, label %10, !llvm.loop !13

20:                                               ; preds = %10, %20
  %21 = phi i64 [ 0, %10 ], [ %28, %20 ]
  %22 = phi double [ %13, %10 ], [ %27, %20 ]
  %23 = getelementptr inbounds [3 x [3 x double]], ptr @__const.main.m1, i64 0, i64 %5, i64 %21
  %24 = load double, ptr %23, align 8, !tbaa !6
  %25 = getelementptr inbounds [3 x [3 x double]], ptr @__const.main.m2, i64 0, i64 %21, i64 %11
  %26 = load double, ptr %25, align 8, !tbaa !6
  %27 = tail call double @llvm.fmuladd.f64(double %24, double %26, double %22)
  %28 = add nuw nsw i64 %21, 1
  %29 = icmp eq i64 %28, 3
  br i1 %29, label %17, label %20, !llvm.loop !14
}

; Function Attrs: argmemonly mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: argmemonly mustprogress nocallback nofree nounwind willreturn writeonly
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #2

; Function Attrs: mustprogress nocallback nofree nosync nounwind readnone speculatable willreturn
declare double @llvm.fmuladd.f64(double, double, double) #3

; Function Attrs: argmemonly mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #1

attributes #0 = { nofree nosync nounwind readnone ssp uwtable(sync) "frame-pointer"="non-leaf" "min-legal-vector-width"="0" "no-trapping-math"="true" "probe-stack"="__chkstk_darwin" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { argmemonly mustprogress nocallback nofree nosync nounwind willreturn }
attributes #2 = { argmemonly mustprogress nocallback nofree nounwind willreturn writeonly }
attributes #3 = { mustprogress nocallback nofree nosync nounwind readnone speculatable willreturn }
attributes #4 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 2, !"SDK Version", [2 x i32] [i32 14, i32 4]}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 8, !"PIC Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{i32 7, !"frame-pointer", i32 1}
!5 = !{!"Apple clang version 15.0.0 (clang-1500.3.9.4)"}
!6 = !{!7, !7, i64 0}
!7 = !{!"double", !8, i64 0}
!8 = !{!"omnipotent char", !9, i64 0}
!9 = !{!"Simple C/C++ TBAA"}
!10 = distinct !{!10, !11, !12}
!11 = !{!"llvm.loop.mustprogress"}
!12 = !{!"llvm.loop.unroll.disable"}
!13 = distinct !{!13, !11, !12}
!14 = distinct !{!14, !11, !12}
