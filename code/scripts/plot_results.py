import os
import pandas as pd
import matplotlib.pyplot as plt

BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RESULTS = os.path.join(BASE, "results")

def save_line(csv_name, x, ys, labels, out_name, y_label):
    path = os.path.join(RESULTS, csv_name)
    if not os.path.exists(path):
        print(f"skip {csv_name}: file not found")
        return
    df = pd.read_csv(path)
    plt.figure(figsize=(7, 4.5))
    for y, label in zip(ys, labels):
        plt.plot(df[x], df[y], marker='o', label=label)
    plt.xlabel(x)
    plt.ylabel(y_label)
    plt.legend()
    plt.tight_layout()
    plt.savefig(os.path.join(RESULTS, out_name), dpi=200)
    plt.close()

if __name__ == "__main__":
    save_line("dot_basic_O2_default.csv", "n", ["naive_ms", "cache_ms"], ["naive", "cache"], "dot_basic_runtime.png", "time (ms)")
    save_line("dot_advanced_O2_default.csv", "n", ["speedup_cache", "speedup_unroll4"], ["cache", "cache+unroll4"], "dot_advanced_speedup.png", "speedup")
    save_line("sum_basic_O2_default.csv", "n", ["naive_ms", "dual_ms"], ["naive", "dual"], "sum_basic_runtime.png", "time (ms)")
    save_line("sum_advanced_O2_default.csv", "n", ["speedup_dual", "speedup_dual_unroll4", "speedup_pairwise"], ["dual", "dual+unroll4", "pairwise"], "sum_advanced_speedup.png", "speedup")
    print("plots saved into results/")
