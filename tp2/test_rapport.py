#!/usr/bin/env python3

import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns
import numpy as np
import sys

df = pd.read_csv(sys.argv[1]).groupby(['algo','taille']).mean().reset_index()

A = [
    ['vorace', lambda x: x*np.log(x), '$nlogn$'],
    ['progdyn', lambda x: x**3, '$n^3$'],
    ['tabou', lambda x: x**2, '$n^2$']
]

plt.figure(figsize=(12,4))

for pos, a in enumerate(A):
    algo, hyp_f, hyp_str = a
    data = df[df.algo == algo].copy()
    
    data['rapport'] = data.temps / hyp_f(data.taille)
    
    ax = plt.subplot(1, 3, pos+1)
    ax.scatter(x=data.taille, y=data.rapport)
    ax.plot(data.taille, data.rapport)
    
    ax.set_title('Test rapport ' + str(algo))
    
    ax.set_ylim(data.rapport.min()*0.9, data.rapport.max()*1.1)
    ax.set_ylabel('f(x) / ' + hyp_str)
    
    ax.set_xlabel('taille')
    
plt.tight_layout()
plt.savefig('test_rapport')
plt.show()
