#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt

size = 5
transitions = [-1, 0, 1]
interations = 200

def prepare(size):
    o = np.zeros([size, size])
    rng = np.arange(size)
    for i in transitions:
        o[rng, (rng + i) % size] = 1
    o /=  o.sum(axis=1) # normalize

    v = np.zeros([size])
    v[0] = 1

    return o, v

def observable(x):
    return np.matmul(np.arange(len(x)).T, x)

T, v = prepare(size)

# results = [observable(v)]
# print(observable(v))

for i in range(interations):
    v = np.matmul(T, v)
    # print(observable(v))
    # results.append(observable(v))

fig, ax = plt.subplots()
ax.bar(range(1, 1 + len(v)), v)
fig.tight_layout()
plt.show()
