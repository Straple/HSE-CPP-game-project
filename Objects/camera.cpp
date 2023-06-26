#include "camera.hpp"
#include "move.hpp"

void Camera::simulate(const Dot &to, efloat delta_time) {
  simulate_move_to2d(pos, to, dp, (to - pos) * 20, delta_time);
}
