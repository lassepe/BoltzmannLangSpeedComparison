# BoltzmannLangSpeedComparison

Comparing the computation time for sampling from a Boltzmann model in Python, Julia and C++.

## Results:

On my machine I get:

| Language | Duration (microseconds) | Ratio (normalized to Julia) |
| -        | -                       | -                           |
| julia    | 1.166                   | 1.000                       |
| C++      | 9.525                   | 8.169                       |
| python   | 170.928545              | 146.593                     |

**Comments:**

**Julia**

Could be faster if `SparseCat` was using faster search on `pdf`.

**C++**

Cold probably be improved to performance comparable to Julia if reduced code
readability was accepted. (using static array for action space and implementing
a custom categorical distribution sampler and `eigen` vectors for actions).

**python**

Using `numpy` arrays wherever possible.


## Running Experiments

Starting from the project root ...

### Julia

```bash
cd julia
# install dependencies
julia --project -e "using Pkg; Pkg.instantiate()"
# run experiment
juluia --project speed_test.jl
```

### Python

Dependencies:

- `numpy`

```bash
cd python
# run experiment
python speed_test.py
```

### C++

Dependencies:

- `eigen3.3`

```bash
cd cpp
# setup build env
mkdir build
cd build
cmake ..
# compile code
make
# run experiment
./speed_test
```
