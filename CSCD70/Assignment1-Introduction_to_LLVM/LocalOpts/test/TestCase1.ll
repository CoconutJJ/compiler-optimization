@.str = private unnamed_addr constant [31 x i8] c"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\0A\00", align 1

define dso_local void @foo(i32 noundef %0) {
  %2 = add nsw i32 %0, 0
  %3 = mul nsw i32 %2, 16
  %4 = mul nsw i32 %3, %2
  %5 = sdiv i32 %4, %0
  %6 = sdiv i32 %4, 10
  %7 = mul nsw i32 54, %5
  %8 = sdiv i32 %6, 128
  %9 = sdiv i32 %7, 54
  %10 = sdiv i32 %6, 1
  %11 = sub nsw i32 %9, 0
  %12 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %2, i32 noundef %3, i32 noundef %4, i32 noundef %5, i32 noundef %6, i32 noundef %7, i32 noundef %8, i32 noundef %9, i32 noundef %10, i32 noundef %11)
  ret void
}

declare i32 @printf(ptr noundef, ...) #1
