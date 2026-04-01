# Lab1 代码包说明

## 1. 这份代码解决了什么
本代码包对应并行程序设计 Lab1 的两道题：
- `n*n` 矩阵每一列与向量内积：`naive`、`cache`、`cache + unroll4`
- `n` 个数求和：`naive`、`dual`、`dual + unroll4`、`pairwise`

其中：
- `cache` 用来体现访存局部性改善；
- `dual` 用来体现减少加法链依赖、适应超标量执行；
- `unroll4` 用来完成进阶要求中的循环展开；
- `pairwise` 是额外扩展，用来讨论“并行结构更强不一定更快”。

## 2. 如何运行
### 基础实验
双击：
- `run_all.bat`

它会输出：
- `results/dot_basic_O2_default.csv`
- `results/dot_advanced_O2_default.csv`
- `results/sum_basic_O2_default.csv`
- `results/sum_advanced_O2_default.csv`

### 进阶：不同编译优化选项
双击：
- `run_flags_compare.bat`

它会分别编译并运行 `-O0 / -O2 / -O3` 三组版本，便于写“编译器优化力度影响性能”的部分。

### 平台信息采集
右键用 PowerShell 运行：
- `collect_system_info.ps1`

它会把 CPU、核心数、内存、系统版本写入 `results/system_info.txt`。

## 3. 文件说明
- `src/dot.c`：矩阵内积相关算法
- `src/reduce.c`：求和相关算法
- `src/timer.c`：高精度计时
- `src/main.c`：正确性检查、重复执行、输出 CSV
- `scripts/plot_results.py`：把 CSV 画成图（可选）

## 4. 为什么之前会出现 cannot open results/dot_results.csv
通常是因为程序运行时的当前目录不在项目根目录，或者 `results` 文件夹不存在。本版本在程序启动时会自动创建 `results` 文件夹，并且 `run_all.bat` 会先切换到脚本所在目录再运行，因此这个问题已经修复。
