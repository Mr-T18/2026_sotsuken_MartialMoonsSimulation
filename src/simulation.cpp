#include <fstream>
#include <iomanip>
#include <iostream>

#include "constants.hpp"
#include "integrator.hpp"

inline void output_state(std::ofstream ofs, double t, const State& sat) {
  ofs << std::scientific << std::setprecision(15) << t << " " << sat.r.x << " "
      << sat.r.y << " " << sat.r.z << " " << sat.v.x << " " << sat.v.y << " "
      << sat.v.z << "\n";
}

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
  const double OUTPUT_INTERVAL = 1e-3;  // 出力間隔[年]
  double next_output_time = 0.0;

  // 初期位置，初速度の出力
  ofs << std::scientific << std::setprecision(15) << t << " " << sat.r.x << " "
      << sat.r.y << " " << sat.r.z << " " << sat.v.x << " " << sat.v.y << " "
      << sat.v.z << "\n";
  next_output_time += OUTPUT_INTERVAL;

  // 最初に半ステップだけガス抗力項で速度のみ進める
  sat.v = rk4_step(sat, physics::DT * 0.5);

  while (t <= physics::MAX_YEARS) {
    t += physics::DT_YEARS;

    // 回文式での更新
    // 保存力・コリオリ力項で位置と速度をリープフロッグ法で更新
    sat = leapfrog_step(sat, physics::DT);

    bool is_last_step = (t > physics::MAX_YEARS - physics::DT_YEARS);
    if (is_last_step) {
      // 最後にガス抗力項で速度をもう一度半ステップ分更新
      sat.v = rk4_step(sat, physics::DT * 0.5);
    } else {
      // ガス抗力項で全ステップ分更新
      sat.v = rk4_step(sat, physics::DT);
    }

    // データ出力
    if (t >= next_output_time - 1e-9) {
      State output_sat = sat;
      output_sat.v = rk4_step(sat, physics::DT * 0.5);

      ofs << std::scientific << std::setprecision(15) << t << " "
          << output_sat.r.x << " " << output_sat.r.y << " " << output_sat.r.z
          << " " << output_sat.v.x << " " << output_sat.v.y << " "
          << output_sat.v.z << "\n";
      next_output_time += OUTPUT_INTERVAL;
    }
  }

  std::cout << "Simulation Finish!" << std::endl;
  return 0;
}