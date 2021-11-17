# -*- coding: utf-8 -*-
"""
Created on Mon Sep 13 15:27:04 2021

@author: breno
"""

import matplotlib.pyplot as plt
import numpy as np
import math

n0_theta, f1 = plt.subplots()

A_f = 0.833072684
W_a = 1.804
f = 0.6365
p = 0.83
a = 0.94
t = 0.95
y = 0.9136

theta = np.linspace(0, math.pi, 50)
n0 = np.zeros(len(theta))
fuso = 3
SL = fuso*15 #oeste de GreenWich
DS = 0 #daylight saving
LL = 48.785496858 #longitude ariranha
L = -21.185999256 #latitude - ao sul do equador = negativo

for i in range(0, len(theta)):
    n0[i] = (p*t*a*y)*(math.cos(theta[i])-(A_f*math.sin(theta[i])))

f1.plot(theta*(180/math.pi), n0, 'r', label='$\eta_0 (theta)$', linewidth='1.6')
f1.set_ylabel('$\eta_0$')
f1.set_xlabel('Ângulo de incidência (rad)')
f1.legend()

B = np.zeros(3)
delta = np.zeros(3)
AST = np.zeros(3)
LST = np.zeros(3)
h = np.zeros(3)
ET = np.zeros(3)
alpha = np.zeros(3)
theta = np.zeros(3)

for dia in range(0,3):
    if dia == 0:
        n = 310
    if dia == 1:
        n = 311
    if dia == 2:
        n = 312
    delta[dia] = 23.45 * math.sin(math.radians((360*(284+n))/365)) #eq 2.5 kalogirou
    B[dia] = math.radians((360*(n-81))/364) #eq 2.2 kalogirou
    ET[dia] = 9.87*math.sin(2*B[dia]) - 7.53*math.cos(B[dia]) -1.5*math.sin(B[dia]) #eq 2.1 kalogirou
    LST[dia] = 12 #12h em min
    AST[dia] = LST[dia] + ET[dia]/60 + 4*(SL-LL)/60 - DS/60  #eq 2.3 kalogirou
    h[dia] = (AST[dia]-12)*15 #eq 2.9 kalogirou
    sen_alpha = math.sin(math.radians(L))*math.sin(delta[dia]) + math.cos(math.radians(L))*math.cos(delta[dia])*math.cos(math.radians(h[dia]))
    alpha[dia] = math.asin(sen_alpha)
    cos_theta = math.sqrt(sen_alpha**2 + math.cos(delta[dia])*math.cos(delta[dia])*math.sin(math.radians(h[dia]))*math.sin(math.radians(h[dia])))
    theta[dia] = (math.acos(cos_theta))*(180/math.pi)