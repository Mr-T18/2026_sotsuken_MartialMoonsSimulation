import argparse
from pathlib import Path
import matplotlib.pyplot as plt
import pandas as pd


def parse_args():
    parser = argparse.ArgumentParser(
        description="指定した時間範囲 (t_start ~ t_end) の軌道をxy平面にプロットします。"
    )
    parser.add_argument(
        "t_start",
        type=float,
        help="プロット開始年 [yr]",
    )
    parser.add_argument(
        "t_end",
        type=float,
        help="プロット終了年 [yr]",
    )
    parser.add_argument(
        "--input",
        "-i",
        type=Path,
        default=None,
        help="データファイルのパス (指定しない場合は out/result.dat または result.dat を自動探索)",
    )
    parser.add_argument(
        "--output-dir",
        "-o",
        type=Path,
        default=Path("figures/orbit"),
        help="画像の保存先ディレクトリ (デフォルト: figures/)",
    )
    return parser.parse_args()


def load_data(filepath: Path) -> pd.DataFrame:
    """7列のdatファイル (t, x, y, z, vx, vy, vz) を読み込む"""
    cols = ["t", "x", "y", "z", "vx", "vy", "vz"]
    df = pd.read_csv(filepath, sep=r"\s+", comment="#", names=cols)
    return df


def plot_orbit(df: pd.DataFrame, t_start: float, t_end: float, output_path: Path):
    # 指定された時間範囲でデータを抽出
    sub_df = df[(df["t"] >= t_start) & (df["t"] <= t_end)]

    if sub_df.empty:
        print(
            f"Warning: 指定された期間 ({t_start} ~ {t_end} yr) にデータが存在しません。"
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

    # 開始点・終了点
    ax.scatter(
        sub_df["x"].iloc[0],
        sub_df["y"].iloc[0],
        color="green",
        s=35,
        zorder=5,
        label=f"Start ($t={sub_df['t'].iloc[0]:.2f}$ yr)",
    )
    ax.scatter(
        sub_df["x"].iloc[-1],
        sub_df["y"].iloc[-1],
        color="red",
        s=35,
        zorder=5,
        label=f"End ($t={sub_df['t'].iloc[-1]:.2f}$ yr)",
    )

    # 火星中心 (原点)
    ax.scatter(0, 0, color="crimson", marker="o", s=60, zorder=6, label="Mars")

    # 軸・表示設定
    ax.set_xlabel("$x$ [$r_H$]", fontsize=12)
    ax.set_ylabel("$y$ [$r_H$]", fontsize=12)
    ax.set_xlim(-1.5, 1.5)
    ax.set_ylim(-1.5, 1.5)
    ax.set_aspect("equal", adjustable="box")
    ax.set_title(f"Hill Orbit ($t = {t_start:.1f} - {t_end:.1f}$ yr)", fontsize=13)
    ax.grid(True, linestyle=":", alpha=0.6)
    ax.legend(loc="upper right", framealpha=0.9)

    plt.tight_layout()
    output_path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(output_path, dpi=300)
    plt.close(fig)
    print(f"Plot saved: {output_path}")


def main():
    args = parse_args()

    # 入力ファイルの探索
    if args.input is not None:
        input_path = args.input
    elif Path("out/result.dat").exists():
        input_path = Path("out/result.dat")
    elif Path("result.dat").exists():
        input_path = Path("result.dat")
    else:
        print(
            "Error: データファイル (out/result.dat または result.dat) が見つかりません。"
        )
        return

    if args.t_start > args.t_end:
        print(
            f"Error: 開始年 ({args.t_start}) が終了年 ({args.t_end}) より大きくなっています。"
        )
        return

    print(f"Loading {input_path} ...")
    df = load_data(input_path)

    # 出力ファイル名（4桁ゼロ埋めフォーマット）
    output_filename = f"orbit_{int(args.t_start):04d}-{int(args.t_end):04d}.png"
    output_path = args.output_dir / output_filename

    plot_orbit(df, args.t_start, args.t_end, output_path)


if __name__ == "__main__":
    main()
