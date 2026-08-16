from pathlib import Path
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np


def load_binary_data(filepath: Path):
    """シミュレーション結果のbinファイル．(t, x, y, z, vx, vy, vz)を読み込み，pd.DataFrameに格納"""
    data = np.fromfile(filepath, dtype=np.float64).reshape(-1, 7)
    cols = ["t", "x", "y", "z", "vx", "vy", "vz"]
    df = pd.DataFrame(data, columns=cols)
    return df


def load_data(filepath: Path):
    """7列のdatファイル．(t, x, y, z, vx, vy, vz)"""
    cols = ["t", "x", "y", "z", "vx", "vy", "vz"]
    df = pd.read_csv(filepath, sep=r"\s+", comment="#", names=cols)
    return df


def plot_orbit(df: pd.DataFrame, t_start: float, t_end: float, output_path: Path):
    # 指定された時間範囲でデータを抽出
    sub_df = df[(df["t"] >= t_start) & (df["t"] <= t_end)]

    if sub_df.empty:
        print(
            f"Warning: 指定された期間({t_start} ~ {t_end} yr)にデータが存在しません．"
        )
        return

    fig, ax = plt.subplots(figsize=(7, 7))

    # 軌道のプロット
    ax.plot(
        sub_df["x"],
        sub_df["y"],
        color="darkviolet",
        lw=0.7,
        alpha=0.85,
        label="Trajectory",
    )

    # 軸・表示設定
    ax.set_xlabel("$x$ [$r_H$]", fontsize=12)
    ax.set_ylabel("$y$ [$r_H$]", fontsize=12)
    ax.set_xlim(-1.5, 1.5)
    ax.set_ylim(-1.5, 1.5)
    ax.set_aspect("equal", adjustable="box")
    ax.set_title(f"Hill Orbit ($t = {t_start:.1f} - {t_end:.1f}$ yr)")
    ax.grid(True, linestyle=":", alpha=0.6)
    ax.legend(loc="upper right", framealpha=0.9)

    plt.tight_layout()
    output_path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(output_path, dpi=300)
    plt.close(fig)
    print(f"Plot saved: {output_path}")


def main():
    input_path = Path("out/result.bin")
    t_start = 15
    t_end = 20
    output_filename = f"orbit_{int(t_start):04d}-{int(t_end):04d}.png"
    output_path = Path("figures/orbit") / output_filename

    df = load_binary_data(input_path)

    plot_orbit(df, t_start, t_end, output_path)

    pass


if __name__ == "__main__":
    main()
