#! /usr/bin/env python

import timeit

vectorLength = 10
number = 10

print('Default Python implementation with lambda')
print timeit.timeit('lcss.compute(random_sample(({},2)), random_sample(({}, 2)))'.format(vectorLength, vectorLength*2), setup = 'from utils import LCSS; from numpy.random import random_sample; lcss = LCSS(similarityFunc = lambda x,y: (abs(x[0]-y[0]) <= 0.1) and (abs(x[1]-y[1]) <= 0.1));', number = number)

print('Using scipy distance.cdist')
print timeit.timeit('lcss.compute(random_sample(({},2)), random_sample(({}, 2)))'.format(vectorLength, vectorLength*2), setup = 'from utils import LCSS; from numpy.random import random_sample; lcss = LCSS(metric = "cityblock", epsilon = 0.1);', number = number)
