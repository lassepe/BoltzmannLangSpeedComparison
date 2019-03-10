using Revise


using Distributions
using StaticArrays
using Random
using BenchmarkTools
using LinearAlgebra
using POMDPModelTools

import Base.rand

struct Pos <: FieldVector{2, Float64}
    x::Float64
    y::Float64
end

struct Action <: FieldVector{2, Float64}
    d::Float64
    phi::Float64
end

function gen_aspace()
    dist_actions = @SVector[0.3, 0.6]
    direction_actions = @SVector[i for i in -pi:pi/4:(pi-pi/4)]
    SVector{length(dist_actions)*length(direction_actions)+1, Action}([zero(Action),(Action(d, direction) for d in dist_actions, direction in direction_actions)...])
end

const State = Pos

# determines the 2D vector from p_start to p_end
vec_from_to(p_start::Pos, p_end::Pos) = SVector(p_end.x - p_start.x, p_end.y - p_start.y)
# computes the 2-norm distance between p1 and p2 (orientation ignored)
dist_to_pos(p1::Pos, p2::Pos, p=1)::Float64 = norm(vec_from_to(p1, p2), p)
# compute the new position given the old position and an action
apply_action(s::State, a::Action)::State = State(s.x + cos(a.phi)*a.d, s.y + sin(a.phi)*a.d)

rand(rng::AbstractRNG, d::UnivariateDistribution) = Distributions.quantile(d, rand(rng)) # fallback

function compute_qval(s::State, a::Action)
    return -a.d - dist_to_pos(apply_action(s, a), State(0, 0), 2)
end

function get_action_distribution(beta::Float64, s::State, as::AS) where AS <: AbstractArray{Action}
    aprobs = [exp(beta * compute_qval(s, a)) for a in as]
    return SparseCat(1:17, aprobs)
end

function transition(s::State, as::AS, beta::Float64, rng::AbstractRNG) where AS <: AbstractArray{Action}
    d = get_action_distribution(beta, s, as)
    sampled_action = as[rand(rng, d)]
    return apply_action(s, sampled_action)
end

function main()
    aspace = gen_aspace()

    rng = MersenneTwister(1)
    s = rand(rng, Pos)
    beta = rand(rng)

    bench = @benchmark transition($s, $aspace, $beta, $rng)
    display(bench)
end

main()
