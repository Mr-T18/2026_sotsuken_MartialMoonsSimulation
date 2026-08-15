#ifndef INTEGRATOR
#define INTEGRATOR
#define _USE_MATH_DEFINES
#include <cmath>

#include "constants.hpp"

struct State {
  double x;
  double y;
  double z;
  double vx;
  double vy;
  double vz;

  State operator+(const State& other) const {
    return {x + other.x,   y + other.y,   z + other.z,
            vx + other.vx, vy + other.vy, vz + other.vz};
  }
  State operator*(double scalar) const {
    return {x * scalar,  y * scalar,  z * scalar,
            vx * scalar, vy * scalar, vz * scalar};
  }

  double r() const { return std::sqrt(x * x + y * y + z * z); }
  double r2() const { return x * x + y * y + z * z; }
  double r3() const { return std::pow(r(), 3); }

  double v() const { return std::sqrt(vx * vx + vy * vy + vz * vz); }
  double v2() const { return vx * vx + vy * vy + vz * vz; }
};

// 戻り値はStateで，各成分は微分係数(変化分)とし，これを現在の値に足すことで積分とする
// 運動方程式により，加速度を求める．

// ガス抵抗も加味した運動方程式．
// この結果からxi+vi*dtを引いてRK4の変化分とし，
// その結果をLeapfrogの結果に足すことでステップ完了とする
State get_derivatives(const State& state) {
  double r3 = state.r3();

  // 重力項
  double gx = 3.0 * state.x / r3;
  double gy = 3.0 * state.y / r3;
  double gz = 3.0 * state.z / r3;

  // ガス抵抗計算のための
  double C = -3.0 / 8.0 * physics::rho_atm / (physics::rho_b * physics ::a_b);
  double v =
      sqrt(state.vx * state.vx + state.vy * state.vy + state.vz * state.vz);

  // 加速度．ガス抵抗含む
  double ax = 2.0 * state.vy + 3.0 * state.x - gx + C * v * state.x;
  double ay = -2.0 * state.vx - gy + C * v * state.z;
  double az = -state.z - gz + C * v * state.z;

  return {state.vx, state.vy, state.vz, ax, ay, az};
}

struct SystemState {
  State sat;
  State acc_sat;
};

// リープフロッグ法で進める
SystemState leapfrog_step(const SystemState& current, double dt) {
  SystemState next;
  double dt2 = dt * 0.5;

  // 現在の衛星の速度を，前回のステップから引き継いだ加速度で半ステップ進める
  double v_half_x = current.sat.vx + current.acc_sat.vx * dt2;
  double v_half_y = current.sat.vy + current.acc_sat.vy * dt2;
  double v_half_z = current.sat.vz + current.acc_sat.vz * dt2;

  // 衛星の位置を1ステップ進める
  next.sat.x = current.sat.x + v_half_x * dt;
  next.sat.y = current.sat.y + v_half_y * dt;
  next.sat.z = current.sat.z + v_half_z * dt;

  // 新しい位置における速度を求める
  // 加速度を求めてからv_{n+1/2}に足す
  double r = next.sat.r();
  double r3 = next.sat.r3();
  // 重力項
  double gx = 3.0 * next.sat.x / r3;
  double gy = 3.0 * next.sat.y / r3;
  double gz = 3.0 * next.sat.z / r3;

  double h = dt;
  double h2 = h * h;
  double inv_h2_1 = 1.0 / (1.0 + h * h);

  // 加速度の更新
  double ax = 3.0 * next.sat.x - gx +
              2.0 *
                  (v_half_y - h * gy * 0.5 - h * v_half_x -
                   h2 * (3.0 * next.sat.x - gx) * 0.5) *
                  inv_h2_1;
  double ay = -gy - 2.0 *
                        (v_half_x + h * (3.0 * next.sat.x - gx) * 0.5 +
                         h * v_half_y - h2 * gy * 0.5) *
                        inv_h2_1;
  double az = -next.sat.z - gz;

  next.acc_sat.vx = ax;
  next.acc_sat.vy = ay;
  next.acc_sat.vz = az;

  next.sat.vx = v_half_x + ax * dt2;
  next.sat.vy = v_half_y + ay * dt2;
  next.sat.vz = v_half_z + az * dt2;

  return next;
}

State rk4_step(const State& state, double dt) {
  State k1 = get_derivatives(state);
  State k2 = get_derivatives(state + k1 * (0.5 * dt));
  State k3 = get_derivatives(state + k2 * (0.5 * dt));
  State k4 = get_derivatives(state + k3 * dt);

  return state + (k1 + k2 * 2.0 + k3 * 2.0 + k4) * (dt / 6.0);
}

#endif