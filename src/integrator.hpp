#ifndef INTEGRATOR
#define INTEGRATOR
#define _USE_MATH_DEFINES
#include <cmath>

#include "constants.hpp"

// 3次元ベクトル
struct Vec3 {
  double x, y, z;
  Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
  Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
  Vec3 operator*(double s) const { return {x * s, y * s, z * s}; }
  Vec3 operator/(double s) const { return {x / s, y / s, z / s}; }
  Vec3& operator+=(const Vec3& o) {
    x += o.x;
    y += o.y;
    z += o.z;
    return *this;
  }
  double norm() const { return std::sqrt(x * x + y * y + z * z); }
  double norm2() const { return x * x + y * y + z * z; }
  double norm3() const {
    double n = norm();
    return n * n * n;
  }
};

struct State {
  Vec3 r;  // 位置ベクトル
  Vec3 v;  // 速度ベクトル
};

// ガス抵抗のみの影響を受けたとして運動方程式を解く
// 位置と速度から加速度を求めるため，引数はstate
inline Vec3 get_drag_acceleration(const Vec3& r, const Vec3& v) {
  double r_norm = r.norm();

  // 火星からの距離によって変化するガス密度．正規化した距離の計算ではないので，距離rにr_Hを掛けている
  double rho_atm =
      physics::rho_neb *
      exp((physics::H / (r_norm * physics::r_H)) - (physics::H / physics::r_H));

  // ガス抵抗計算のための速度にかかる比例係数
  double C_norm =
      -(3.0 / 8.0) * (rho_atm * physics::r_H) / (physics::rho_b * physics::a_b);
  double v_norm = v.norm();

  // 加速度ベクトルを返す
  return v * (C_norm * v_norm);
}

// 潮汐力・遠心力・重力を含めた保存力の項を返す
// ただし，速度依存のコリオリ力項は含めない
inline Vec3 get_gravity_acceleration(const Vec3& r) {
  double r3 = r.norm3();

  // 火星中心重力
  double gx = 3.0 * r.x / r3;
  double gy = 3.0 * r.y / r3;
  double gz = 3.0 * r.z / r3;

  // 重力，潮汐力，遠心力を含めたもの
  return {3.0 * r.x - gx, -gy, -r.z - gz};
}

// RK4ではガス抗力項のみ計算するが，更新するのは加速度(による速度)のみ
inline Vec3 rk4_step(const State& state, double dt) {
  Vec3 k1 = get_drag_acceleration(state.r, state.v);
  Vec3 k2 = get_drag_acceleration(state.r, state.v + k1 * (0.5 * dt));
  Vec3 k3 = get_drag_acceleration(state.r, state.v + k2 * (0.5 * dt));
  Vec3 k4 = get_drag_acceleration(state.r, state.v + k3 * dt);

  // 戻り値：{ax, ay, az}
  return state.v + (k1 + k2 * 2.0 + k3 * 2.0 + k4) * (dt / 6.0);
}

inline State leapfrog_step(const State& current, double dt) {
  double dt_half = dt * 0.5;

  // 現在の位置での重力を計算
  Vec3 g_current = get_gravity_acceleration(current.r);

  // First Kick:
  // コリオリ力を含めて速度をdt/2更新．コリオリ力は既知なので，陰的に解くことができる
  Vec3 v_half = {current.v.x + dt_half * (g_current.x + 2.0 * current.v.y),
                 current.v.y + dt_half * (g_current.y - 2.0 * current.v.x),
                 current.v.z + dt_half * g_current.z};

  // Full Drift: 位置の更新
  State next;
  next.r = current.r + v_half * dt;

  // Second Kick: 新しい位置での速度の更新
  // コリオリ力は速度依存で既知ではないので，行列の式変形をして，なんとか陰的に解く
  // 新しい位置での重力加速度の計算
  Vec3 g_next = get_gravity_acceleration(next.r);
  double Ax = v_half.x + dt_half * g_next.x;
  double Ay = v_half.y + dt_half * g_next.y;
  double denom = 1.0 / (1.0 + dt * dt);

  next.v.x = (Ax + dt * Ay) * denom;
  next.v.y = (Ay - dt * Ax) * denom;
  next.v.z = v_half.z + dt_half * g_next.z;

  return next;
}

#endif