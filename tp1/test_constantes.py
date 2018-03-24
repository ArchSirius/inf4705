import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns
import numpy as np
from scipy.stats import linregress
import sys

df = pd.read_csv(sys.argv[1]).groupby(['algo','taille']).mean().reset_index()

A = [
    ['stdsort', lambda x: x*np.log(x), '$nlogn$'],
    ['qsort', lambda x: x*np.log(x), '$nlogn$'],
    ['insertion', lambda x: x**2, '$n^2$'],
    ['merge', lambda x: x*np.log(x), '$nlogn$'],
    ['mergeSeuil', lambda x: x*np.log(x), '$nlogn$']
]

plt.figure(figsize=(12,4))

for pos, a in enumerate(A):
    algo, hyp_f, hyp_str = a
    data = df[df.algo == algo].copy()
    
    data['hyp'] = hyp_f(data.taille)
    fit = np.polyfit(data.hyp, data.temps, 1)
    fit_fn = np.poly1d(fit)
    
    ax = plt.subplot(2, 3, pos+1)
    ax.scatter(x=data.hyp, y=data.temps)
    ax.plot(data.hyp, fit_fn(data.hyp))
    
    ax.set_title('Test des constantes ' + str(algo))
    
    ax.set_ylim(data.temps.min()*0.9, data.temps.max()*1.1)
    ax.set_xlim(0, data.hyp.max()*1.1)
    ax.set_ylabel('temps')
    
    ax.set_xlabel('f(x) / ' + hyp_str)

    slope, intercept, r_value, p_value, std_err = linregress(data.hyp, data.temps)
    print(str(algo) + ' : slope = ' + str(slope))
    print(str(algo) + ' : intercept = ' + str(intercept))
    print(str(algo) + ' : r_value = ' + str(r_value))
    print(str(algo) + ' : r_squared = ' + str(r_value**2))
    print(str(algo) + ' : p_value = ' + str(p_value))
    print(str(algo) + ' : std_err = ' + str(std_err))
    
plt.tight_layout()
plt.savefig('test_constantes')
plt.show()
