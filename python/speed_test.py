#!/usr/bin/env python
# -*- coding: utf-8 -*-

import numpy as np
import random
from timeit import default_timer as timer

def gen_aspace():
    dist_actions = np.array([0.3, 0.6])
    direction_actions = np.linspace(-np.pi, 3/4*np.pi, 8)
    return np.vstack((np.zeros(2), np.array([[dist, direction] for dist in dist_actions for direction in direction_actions])))

def dist_to_pos(p1, p2, p):
    return np.linalg.norm(p2 - p1, ord=p)

# action[0] is the distance
# action[1] is the angle
def apply_action(state, action):
    return np.array([state[0] + np.cos(action[1])*action[0], state[1] + np.sin(action[1])*action[0]])

def compute_qval(state, action):
    return -action[0] - dist_to_pos(apply_action(state, action), np.array([0, 0]), 2)

def get_action_distribution(beta, state, aspace):
    aprobs = np.array([np.exp(beta * compute_qval(state, action)) for action in aspace])
    return aprobs / sum(aprobs)

def transition(state, aspace, beta):
    d = get_action_distribution(beta, state, aspace)
    action_index = (np.where(np.random.multinomial(1, d)==1))[0][0]
    return apply_action(state, aspace[action_index, :])

def main():
    aspace = gen_aspace()
    n_samples = 10000
    time_sum = 0

    print("Run experiments with %d samples:" % (n_samples))

    for i in range(n_samples):
        # sample random state
        state = np.random.rand(1, 2)[0]
        beta = random.random()

        # start experiment
        start = timer()
        transition(state, aspace, beta)
        end = timer()

        # add to measured times
        time_sum += end - start

    print("Average runtime over %d samples was: %f micro seconds." % (n_samples, time_sum / n_samples * 1e6))

if __name__ == "__main__":
    main()
