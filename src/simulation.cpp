#include <fstream>
#include <iomanip>
#include <iostream>

#include "constants.hpp"
#include "integrator.hpp"

int main() {
  // 初期条件の設定
  State sat = {{physics::x0, physics::y0, physics::z0},
               {physics::vx0, physics::vy0, physics::vz0}};

  std::ofstream ofs("out/result.dat");
  if (!ofs) {
    std::cerr << "Error: Cannot open resulut.dat" << std::endl;
  }
  std::cout << "Start Hill Simulation" << std::endl;

  double t = 0.0;
  const double OUTPUT_INTERVAL = 1e-3;  // 出力間隔
  double next_output_time = 0.0;

  while (t <= physics::MAX_YEARS) {
    // データ出力
    if (t >= next_output_time - 1e-9) {
      ofs << std::scientific << std::setprecision(15) << t << " " << sat.r.x
          << " " << sat.r.y << " " << sat.r.z << " " << sat.v.x << " "
          << sat.v.y << " " << sat.v.z << "\n";
      next_output_time += OUTPUT_INTERVAL;
    }

    // 回文式での更新
    // ガス抗力項で速度をdt/2だけ更新
    sat.v = rk4_step(sat, physics::DT * 0.5);

    // 保存力・コリオリ力項で位置と速度をリープフロッグ法で更新
    sat = leapfrog_step(sat, physics::DT);

    // 最後にガス抗力項で速度をもう一度半ステップ分更新
    sat.v = rk4_step(sat, physics::DT * 0.5);

    t += physics::DT_YEARS;
  }

  std::cout << "Simulation Finish!" << std::endl;
  return 0;
}