#include <fstream>
#include <iomanip>
#include <iostream>

#include "constants.hpp"
#include "integrator.hpp"

int main() {
  SystemState sys;

  // 初期条件の代入
  sys.sat.x = physics::x0;
  sys.sat.y = physics::y0;
  sys.sat.z = physics::z0;
  sys.sat.vx = physics::vx0;
  sys.sat.vy = physics::vy0;
  sys.sat.vz = physics::vz0;

  sys.acc_sat = get_derivatives(sys.sat);

  std::ofstream ofs("out/result.dat");
  if (!ofs) {
    std::cerr << "Error: Cannot open resulut.dat" << std::endl;
  }

  std::cout << "Start Hill Simulation" << std::endl;

  double t = 0.0;
  long long step = 0;

  const double OUTPUT_INTERVAL = 1e-3;  // 出力間隔
  double next_output_time = 0.0;        // 次に出力

  while (t <= physics::MAX_YEARS) {
    if (t >= next_output_time - 1e-9) {
      double t_years = t;

      double x = sys.sat.x;
      double y = sys.sat.y;
      double z = sys.sat.z;
      double vx = sys.sat.vx;
      double vy = sys.sat.vy;
      double vz = sys.sat.vz;

      ofs << std::scientific << std::setprecision(15) << t_years << " " << x
          << " " << y << " " << z << " " << vx << " " << vy << " " << vz
          << "\n";
      next_output_time += OUTPUT_INTERVAL;
    }

    sys = leapfrog_step(sys, physics::DT);
    // sys.sat = rk4_step(sys.sat, physics::DT);
    t += physics::DT_YEARS;
    step++;
  }

  std::cout << "Simulation Finish!" << std::endl;
}