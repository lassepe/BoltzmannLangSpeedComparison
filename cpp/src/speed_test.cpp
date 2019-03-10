#include <math.h>
#include <chrono>
#include <Eigen/Dense>
#include <array>
#include <iostream>
#include <random>
#include <vector>

using State = Eigen::Vector2f;

struct Action {
  float dist;
  float direction;
};

using ActionSpace = std::vector<Action>;

ActionSpace gen_aspace() {
  auto aspace = ActionSpace();
  // add zero element
  aspace.push_back(Action({0, 0}));

  auto dist_actions = std::vector<float>({0.3, 0.6});
  // fill direction actions
  auto direction_actions = std::vector<float>();
  for (float phi = -M_PI; phi < M_PI; phi += M_PI / 4) {
    direction_actions.push_back(phi);
  }
  // fill action space
  for (auto& dist : dist_actions) {
    for (auto& dir : direction_actions) {
      aspace.push_back(Action({dist, dir}));
    }
  }

  return aspace;
}

State apply_action(State& s, Action& a) {
  return State(s.x() + cosf(a.direction) * a.dist,
               s.y() + sinf(a.direction) * a.dist);
}

float compute_qval(State& s, Action& a) {
  return -a.dist - (apply_action(s, a) - State(0, 0)).norm();
}

std::discrete_distribution<int> get_action_distribution(float beta, State& s,
                                                        ActionSpace& aspace) {
  auto aprobs = std::vector<float>();
  aprobs.resize(aspace.size());

  for (int i = 1; i < aspace.size(); i++) {
    aprobs[i] = expf(beta * compute_qval(s, aspace[i]));
  }

  std::discrete_distribution<int> d(aprobs.begin(), aprobs.end());
  return d;
}

State transition(State& s, ActionSpace& aspace, float beta,
                 std::random_device& rng) {
  auto d = get_action_distribution(beta, s, aspace);
  auto sampled_action = aspace[d(rng)];
  return apply_action(s, sampled_action);
}

void test_methods() {
  auto vec = Eigen::Vector2f(2, 3);

  std::cout << "Aspace" << std::endl;
  auto aspace = gen_aspace();
  for (auto& a : aspace) {
    std::cout << "Action: " << a.dist << ", " << a.direction << std::endl;
  }

  auto s = State(1, 2);
  auto a = Action({1, 2});

  std::cout << "apply_action:" << std::endl;
  std::cout << apply_action(s, a) << std::endl;

  std::cout << "compute_qval" << std::endl;
  std::cout << compute_qval(s, a) << std::endl;

  std::cout << "get_action_distribution" << std::endl;
  std::random_device rng;
  float beta = 1.0;

  auto d = get_action_distribution(beta, s, aspace);

  for (int i = 0; i < 10; i++) {
    std::cout << d(rng) << std::endl;
  }

  std::cout << "transition" << std::endl;
  std::cout << transition(s, aspace, beta, rng) << std::endl;
}

float randZeroToOne()
{
    return rand() / (RAND_MAX + 1.);
}

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::nanoseconds nanoseconds;

int main() {
  srand (static_cast <unsigned> (time(0)));

  std::random_device rng;
  auto aspace = gen_aspace();
  int n_samples = 10000;

  std::chrono::duration<long, std::ratio<1, 1000000000>> time_sum;

  for (int i = 0; i < n_samples; i++)
  {
    timespec time1, time2;
    State state = State::Random();
    auto beta = randZeroToOne();

    Clock::time_point t_start = Clock::now();
    transition(state, aspace, beta, rng);
    Clock::time_point t_end = Clock::now();
    time_sum += t_end - t_start;
  }
  std::cout << "Average over " << n_samples << " was: " << 1e-3*std::chrono::duration_cast<nanoseconds>(time_sum).count()/n_samples << " microseconds." << std::endl;
}
