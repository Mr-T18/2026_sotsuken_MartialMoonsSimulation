load "config.gp"

Ueff(x, y, z) = -1.5*x**2 + 0.5 * z**2 - 3.0/r_O(x, y, z) + 4.5
J(x, y, z, vx, vy, vz) = 0.5 * (vx**2 + vy**2 + vz**2) + Ueff(x, y, z)

# 太陽中心慣性座標での火星と衛星の位置
set output "../plot/jacobi.png"
set title "衛星のJacobiエネルギー"
set xlabel "t [yr]"
set ylabel "質量あたりのJacobiエネルギー [J/kg]"
set xrange [*:*]
set yrange [*:*]
plot "../result.dat" u 1:(J($2, $3, $4, $5, $6, $7)) w p t "Energy"