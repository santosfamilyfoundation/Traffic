#! /usr/bin/env python
# -*- coding: utf-8 -*-
''' Generic utilities.'''

import matplotlib.pyplot as plt
from datetime import time, datetime
from math import sqrt, ceil, floor
from scipy.stats import kruskal, shapiro
from scipy.spatial import distance
from numpy import zeros, array, exp, sum as npsum, int as npint, arange, cumsum, median, isnan, ones, convolve,  dtype, isnan, NaN, mean, ma, isinf


datetimeFormat = "%Y-%m-%d %H:%M:%S"

#########################
# Strings
#########################

def upperCaseFirstLetter(s):
    words = s.split(' ')
    lowerWords = [w[0].upper()+w[1:].lower() for w in words]
    return ' '.join(lowerWords)

#########################
# Enumerations
#########################

def inverseEnumeration(l):
    'Returns the dictionary that provides for each element in the input list its index in the input list'
    result = {}
    for i,x in enumerate(l):
        result[x] = i
    return result

#########################
# Simple statistics
#########################

def logNormalMeanVar(loc, scale):
    '''location and scale are respectively the mean and standard deviation of the normal in the log-normal distribution
    https://en.wikipedia.org/wiki/Log-normal_distribution'''
    mean = exp(loc+(scale**2)/2)
    var = (exp(loc**2)-1)*exp(2*loc+scale**2)
    return mean, var

def sampleSize(stdev, tolerance, percentConfidence, printLatex = False):
    from scipy.stats.distributions import norm
    k = round(norm.ppf(0.5+percentConfidence/200., 0, 1)*100)/100. # 1.-(100-percentConfidence)/200.
    if printLatex:
        print('${0}^2\\frac{{{1}^2}}{{{2}^2}}$'.format(k, stdev, tolerance))
    return (k*stdev/tolerance)**2

def confidenceInterval(mean, stdev, nSamples, percentConfidence, trueStd = True, printLatex = False):
    '''if trueStd, use normal distribution, otherwise, Student

    Use otherwise t.interval or norm.interval
    ex: norm.interval(0.95, loc = 0., scale = 2.3/sqrt(11))
    t.interval(0.95, 10, loc=1.2, scale = 2.3/sqrt(nSamples))
    loc is mean, scale is sigma/sqrt(n) (for Student, 10 is df)'''
    from scipy.stats.distributions import norm, t
    if trueStd:
        k = round(norm.ppf(0.5+percentConfidence/200., 0, 1)*100)/100. # 1.-(100-percentConfidence)/200.
    else: # use Student
         k = round(t.ppf(0.5+percentConfidence/200., nSamples-1)*100)/100.
    e = k*stdev/sqrt(nSamples)
    if printLatex:
        print('${0} \pm {1}\\frac{{{2}}}{{\sqrt{{{3}}}}}$'.format(mean, k, stdev, nSamples))
    return mean-e, mean+e

def computeChi2(expected, observed):
    '''Returns the Chi2 statistics'''
    result = 0.
    for e, o in zip(expected, observed):
        result += ((e-o)*(e-o))/e
    return result

class DistributionSample(object):
    def nSamples(self):
        return sum(self.counts)

def cumulativeDensityFunction(sample, normalized = False):
    '''Returns the cumulative density function of the sample of a random variable'''
    xaxis = sorted(sample)
    counts = arange(1,len(sample)+1) # dtype = float
    if normalized:
        counts /= float(len(sample))
    return xaxis, counts

class DiscreteDistributionSample(DistributionSample):
    '''Class to represent a sample of a distribution for a discrete random variable'''
    def __init__(self, categories, counts):
        self.categories = categories
        self.counts = counts

    def mean(self):
        result = [float(x*y) for x,y in zip(self.categories, self.counts)]
        return npsum(result)/self.nSamples()

    def var(self, mean = None):
        if not mean:
            m = self.mean()
        else:
            m = mean
        result = 0.
        squares = [float((x-m)*(x-m)*y) for x,y in zip(self.categories, self.counts)]
        return npsum(squares)/(self.nSamples()-1)

    def referenceCounts(self, probability):
        '''probability is a function that returns the probability of the random variable for the category values'''
        refProba = [probability(c) for c in self.categories]
        refProba[-1] = 1-npsum(refProba[:-1])
        refCounts = [r*self.nSamples() for r in refProba]
        return refCounts, refProba

class ContinuousDistributionSample(DistributionSample):
    '''Class to represent a sample of a distribution for a continuous random variable
    with the number of observations for each interval
    intervals (categories variable) are defined by their left limits, the last one being the right limit
    categories contain therefore one more element than the counts'''
    def __init__(self, categories, counts):
        # todo add samples for initialization and everything to None? (or setSamples?)
        self.categories = categories
        self.counts = counts

    @staticmethod
    def generate(sample, categories):
        if min(sample) < min(categories):
            print('Sample has lower min than proposed categories ({}, {})'.format(min(sample), min(categories)))
        if max(sample) > max(categories):
            print('Sample has higher max than proposed categories ({}, {})'.format(max(sample), max(categories)))
        dist = ContinuousDistributionSample(sorted(categories), [0]*(len(categories)-1))
        for s in sample:
            i = 0
            while  i<len(dist.categories) and dist.categories[i] <= s:
                i += 1
            if i <= len(dist.counts):
                dist.counts[i-1] += 1
                #print('{} in {} {}'.format(s, dist.categories[i-1], dist.categories[i]))
            else:
                print('Element {} is not in the categories'.format(s))
        return dist

    def mean(self):
        result = 0.
        for i in range(len(self.counts)-1):
            result += self.counts[i]*(self.categories[i]+self.categories[i+1])/2
        return result/self.nSamples()

    def var(self, mean = None):
        if not mean:
            m = self.mean()
        else:
            m = mean
        result = 0.
        for i in range(len(self.counts)-1):
            mid = (self.categories[i]+self.categories[i+1])/2
            result += self.counts[i]*(mid - m)*(mid - m)
        return result/(self.nSamples()-1)

    def referenceCounts(self, cdf):
        '''cdf is a cumulative distribution function
        returning the probability of the variable being less that x'''
        # refCumulativeCounts = [0]#[cdf(self.categories[0][0])]
#         for inter in self.categories:
#             refCumulativeCounts.append(cdf(inter[1]))
        refCumulativeCounts = [cdf(x) for x in self.categories[1:-1]]

        refProba = [refCumulativeCounts[0]]
        for i in xrange(1,len(refCumulativeCounts)):
            refProba.append(refCumulativeCounts[i]-refCumulativeCounts[i-1])
        refProba.append(1-refCumulativeCounts[-1])
        refCounts = [p*self.nSamples() for p in refProba]
        
        return refCounts, refProba

    def printReferenceCounts(self, refCounts=None):
        if refCounts:
            ref = refCounts
        else:
            ref = self.referenceCounts
        for i in xrange(len(ref[0])):
            print('{0}-{1} & {2:0.3} & {3:0.3} \\\\'.format(self.categories[i],self.categories[i+1],ref[1][i], ref[0][i]))


#########################
# maths section
#########################

# def kernelSmoothing(sampleX, X, Y, weightFunc, halfwidth):
#     '''Returns a smoothed weighted version of Y at the predefined values of sampleX
#     Sum_x weight(sample_x,x) * y(x)'''
#     from numpy import zeros, array
#     smoothed = zeros(len(sampleX))
#     for i,x in enumerate(sampleX):
#         weights = array([weightFunc(x,xx, halfwidth) for xx in X])
#         if sum(weights)>0:
#             smoothed[i] = sum(weights*Y)/sum(weights)
#         else:
#             smoothed[i] = 0
#     return smoothed

def kernelSmoothing(x, X, Y, weightFunc, halfwidth):
    '''Returns the smoothed estimate of (X,Y) at x
    Sum_x weight(sample_x,x) * y(x)'''
    weights = array([weightFunc(x,observedx, halfwidth) for observedx in X])
    if sum(weights)>0:
        return sum(weights*Y)/sum(weights)
    else:
        return 0

def uniform(center, x, halfwidth):
    if abs(center-x)<halfwidth:
        return 1.
    else:
        return 0.

def gaussian(center, x, halfwidth):
    return exp(-((center-x)/halfwidth)**2/2)

def epanechnikov(center, x, halfwidth):
    diff = abs(center-x)
    if diff<halfwidth:
        return 1.-(diff/halfwidth)**2
    else:
        return 0.
    
def triangular(center, x, halfwidth):
    diff = abs(center-x)
    if diff<halfwidth:
        return 1.-abs(diff/halfwidth)
    else:
        return 0.

def medianSmoothing(x, X, Y, halfwidth):
    '''Returns the media of Y's corresponding to X's in the interval [x-halfwidth, x+halfwidth]'''
    return median([y for observedx, y in zip(X,Y) if abs(x-observedx)<halfwidth])

def argmaxDict(d):
    return max(d, key=d.get)

def framesToTime(nFrames, frameRate, initialTime = time()):
    '''returns a datetime.time for the time in hour, minutes and seconds
    initialTime is a datetime.time'''
    seconds = int(floor(float(nFrames)/float(frameRate))+initialTime.hour*3600+initialTime.minute*60+initialTime.second)
    h = int(floor(seconds/3600.))
    seconds = seconds - h*3600
    m = int(floor(seconds/60))
    seconds = seconds - m*60
    return time(h, m, seconds)

def timeToFrames(t, frameRate):
    return frameRate*(t.hour*3600+t.minute*60+t.second)

def sortXY(X,Y):
    'returns the sorted (x, Y(x)) sorted on X'
    D = {}
    for x, y in zip(X,Y):
        D[x]=y
    xsorted = sorted(D.keys())
    return xsorted, [D[x] for x in xsorted]

def compareLengthForSort(i, j):
    if len(i) < len(j):
        return -1
    elif len(i) == len(j):
        return 0
    else:
        return 1

def sortByLength(instances, reverse = False):
    '''Returns a new list with the instances sorted by length (method __len__)
    reverse is passed to sorted'''
    return sorted(instances, cmp = compareLengthForSort, reverse = reverse)

def ceilDecimals(v, nDecimals):
    '''Rounds the number at the nth decimal
    eg 1.23 at 0 decimal is 2, at 1 decimal is 1.3'''
    tens = 10**nDecimals
    return ceil(v*tens)/tens

def inBetween(bound1, bound2, x):
    'useful if one does not know the order of bound1/bound2'
    return bound1 <= x <= bound2 or bound2 <= x <= bound1

def pointDistanceL2(x1,y1,x2,y2):
    ''' Compute point-to-point distance (L2 norm, ie Euclidean distance)'''
    return sqrt((x2-x1)**2+(y2-y1)**2)

def crossProduct(l1, l2):
    return l1[0]*l2[1]-l1[1]*l2[0]

def cat_mvgavg(cat_list, halfWidth):
    ''' Return a list of categories/values smoothed according to a window. 
        halfWidth is the search radius on either side'''
    from copy import deepcopy
    smoothed = deepcopy(cat_list)
    for point in range(len(cat_list)):
        lower_bound_check = max(0,point-halfWidth)
        upper_bound_check = min(len(cat_list)-1,point+halfWidth+1)
        window_values = cat_list[lower_bound_check:upper_bound_check]
        smoothed[point] = max(set(window_values), key=window_values.count)
    return smoothed

def filterMovingWindow(inputSignal, halfWidth):
    '''Returns an array obtained after the smoothing of the input by a moving average
    The first and last points are copied from the original.'''
    width = float(halfWidth*2+1)
    win = ones(width,'d')
    result = convolve(win/width,array(inputSignal),'same')
    result[:halfWidth] = inputSignal[:halfWidth]
    result[-halfWidth:] = inputSignal[-halfWidth:]
    return result

def linearRegression(x, y, deg = 1, plotData = False):
    '''returns the least square estimation of the linear regression of y = ax+b
    as well as the plot'''
    from numpy.lib.polynomial import polyfit
    from numpy.core.multiarray import arange
    coef = polyfit(x, y, deg)
    if plotData:
        def poly(x):
            result = 0
            for i in range(len(coef)):
                result += coef[i]*x**(len(coef)-i-1)
            return result
        plt.plot(x, y, 'x')
        xx = arange(min(x), max(x),(max(x)-min(x))/1000)
        plt.plot(xx, [poly(z) for z in xx])
    return coef

def correlation(data, correlationMethod = 'pearson', plotFigure = False, displayNames = None, figureFilename = None):
    '''Computes (and displays) the correlation matrix for a pandas DataFrame'''
    columns = data.columns.tolist()
    for var in data.columns:
        uniqueValues = data[var].unique()
        if len(uniqueValues) == 1 or data.dtypes[var] == dtype('O') or (len(uniqueValues) == 2 and len(data.loc[~isnan(data[var]), var].unique()) == 1): # last condition: only one other value than nan
            columns.remove(var)
    c=data[columns].corr(correlationMethod)
    if plotFigure:
        fig = plt.figure(figsize=(4+0.4*c.shape[0], 0.4*c.shape[0]))
        fig.add_subplot(1,1,1)
        #plt.imshow(np.fabs(c), interpolation='none')
        plt.imshow(c, vmin=-1., vmax = 1., interpolation='none', cmap = 'RdYlBu_r') # coolwarm
        colnames = [displayNames.get(s.strip(), s.strip()) for s in columns]
        #correlation.plot_corr(c, xnames = colnames, normcolor=True, title = filename)
        plt.xticks(range(len(colnames)), colnames, rotation=90)
        plt.yticks(range(len(colnames)), colnames)
        plt.tick_params('both', length=0)
        plt.subplots_adjust(bottom = 0.29)
        plt.colorbar()
        plt.title('Correlation ({})'.format(correlationMethod))
        plt.tight_layout()
        if len(colnames) > 50:
            plt.subplots_adjust(left=.06)
        if figureFilename is not None:
            plt.savefig(figureFilename, dpi = 150, transparent = True)
    return c

def addDummies(data, variables, allVariables = True):
    '''Add binary dummy variables for each value of a nominal variable 
    in a pandas DataFrame'''
    newVariables = []
    for var in variables:
        if var in data.columns and data.dtypes[var] == dtype('O') and len(data[var].unique()) > 2:
            values = data[var].unique()
            if not allVariables:
                values = values[:-1]
            for val in values:
                if val is not NaN:
                    newVariable = (var+'_{}'.format(val)).replace('.','').replace(' ','').replace('-','')
                    data[newVariable] = (data[var] == val)
                    newVariables.append(newVariable)
    return newVariables

def kruskalWallis(data, dependentVariable, independentVariable, plotFigure = False, filenamePrefix = None, figureFileType = 'pdf', saveLatex = False, renameVariables = lambda s: s, kwCaption = u''):
    '''Studies the influence of (nominal) independent variable over the dependent variable

    Makes tests if the conditional distributions are normal
    using the Shapiro-Wilk test (in which case ANOVA could be used)
    Implements uses the non-parametric Kruskal Wallis test'''
    tmp = data[data[independentVariable].notnull()]
    independentVariableValues = sorted(tmp[independentVariable].unique().tolist())
    if len(independentVariableValues) >= 2:
        if saveLatex:
            from storage import openCheck
            out = openCheck(filenamePrefix+'-{}-{}.tex'.format(dependentVariable, independentVariable), 'w')
        for x in independentVariableValues:
            print('Shapiro-Wilk normality test for {} when {}={}: {} obs'.format(dependentVariable,independentVariable, x, len(tmp.loc[tmp[independentVariable] == x, dependentVariable])))
            if len(tmp.loc[tmp[independentVariable] == x, dependentVariable]) >= 3:
                print shapiro(tmp.loc[tmp[independentVariable] == x, dependentVariable])
        if plotFigure:
            plt.figure()
            plt.boxplot([tmp.loc[tmp[independentVariable] == x, dependentVariable] for x in independentVariableValues])
            #q25, q75 = tmp[dependentVariable].quantile([.25, .75])
            #plt.ylim(ymax = q75+1.5*(q75-q25))
            plt.xticks(range(1,len(independentVariableValues)+1), independentVariableValues)
            plt.title('{} vs {}'.format(dependentVariable, independentVariable))
            if filenamePrefix is not None:
                plt.savefig(filenamePrefix+'-{}-{}.{}'.format(dependentVariable, independentVariable, figureFileType))
        table = tmp.groupby([independentVariable])[dependentVariable].describe().unstack().sort(['50%'], ascending = False)
        table['count'] = table['count'].astype(int)
        testResult = kruskal(*[tmp.loc[tmp[independentVariable] == x, dependentVariable] for x in independentVariableValues])
        if saveLatex:
            out.write('\\begin{minipage}{\\linewidth}\n'
                      +'\\centering\n'
                      +'\\captionof{table}{'+(kwCaption.format(dependentVariable, independentVariable, *testResult))+'}\n'
                      +table.to_latex(float_format = lambda x: '{:.3f}'.format(x)).encode('ascii')+'\n'
                      +'\\end{minipage}\n'
                      +'\\ \\vspace{0.5cm}\n')
        else:
            print table
        return testResult
    else:
        return None

def prepareRegression(data, dependentVariable, independentVariables, maxCorrelationThreshold, correlations, maxCorrelationP, correlationFunc, stdoutText = ['Removing {} (constant: {})', 'Removing {} (correlation {} with {})', 'Removing {} (no correlation: {}, p={})'], saveFiles = False, filenamePrefix = None, latexHeader = '', latexTable = None, latexFooter=''):
    '''Removes variables from candidate independent variables if
    - if two independent variables are correlated (> maxCorrelationThreshold), one is removed
    - if an independent variable is not correlated with the dependent variable (p>maxCorrelationP)
    Returns the remaining non-correlated variables, correlated with the dependent variable

    correlationFunc is spearmanr or pearsonr from scipy.stats
    text is the template to display for the two types of printout (see default): 3 elements if no saving to latex file, 8 otherwise

    TODO: pass the dummies for nominal variables and remove if all dummies are correlated, or none is correlated with the dependentvariable'''    
    from copy import copy
    from pandas import DataFrame
    result = copy(independentVariables)
    table1 = ''
    table2 = {}
    # constant variables
    for var in independentVariables:
        uniqueValues = data[var].unique()
        if (len(uniqueValues) == 1) or (len(uniqueValues) == 2 and uniqueValues.dtype != dtype('O') and len(data.loc[~isnan(data[var]), var].unique()) == 1):
            print(stdoutText[0].format(var, uniqueValues))
            if saveFiles:
                table1 += latexTable[0].format(var, *uniqueValues)
            result.remove(var)
    # correlated variables
    for v1 in copy(result):
        if v1 in correlations.index:
            for v2 in copy(result):
                if v2 != v1 and v2 in correlations.index:
                    if abs(correlations.loc[v1, v2]) > maxCorrelationThreshold:
                        if v1 in result and v2 in result:
                            if saveFiles:
                                table1 += latexTable[1].format(v2, v1, correlations.loc[v1, v2])
                            print(stdoutText[1].format(v2, v1, correlations.loc[v1, v2]))
                            result.remove(v2)
    # not correlated with dependent variable
    table2['Correlations'] = []
    table2['Valeurs p'] = []
    for var in copy(result):
        if data.dtypes[var] != dtype('O'):
            cor, p = correlationFunc(data[dependentVariable], data[var])
            if p > maxCorrelationP:
                if saveFiles:
                    table1 += latexTable[2].format(var, cor, p)
                print(stdoutText[2].format(var, cor, p))
                result.remove(var)
            else:
                table2['Correlations'].append(cor)
                table2['Valeurs p'].append(p)

    if saveFiles:
        from storage import openCheck
        out = openCheck(filenamePrefix+'-removed-variables.tex', 'w')
        out.write(latexHeader)
        out.write(table1)
        out.write(latexFooter)
        out.close()
        out = openCheck(filenamePrefix+'-correlations.html', 'w')
        table2['Variables'] = [var for var in result if data.dtypes[var] != dtype('O')]
        out.write(DataFrame(table2)[['Variables', 'Correlations', 'Valeurs p']].to_html(formatters = {'Correlations': lambda x: '{:.2f}'.format(x), 'Valeurs p': lambda x: '{:.3f}'.format(x)}, index = False))
        out.close()
    return result


#########################
# regression analysis using statsmodels (and pandas)
#########################

# TODO make class for experiments?
# TODO add tests with public dataset downloaded from Internet (IRIS et al)
def modelString(experiment, dependentVariable, independentVariables):
    return dependentVariable+' ~ '+' + '.join([independentVariable for independentVariable in independentVariables if experiment[independentVariable]])

def runModel(experiment, data, dependentVariable, independentVariables, regressionType = 'ols'):
    import statsmodels.formula.api as smf
    modelStr = modelString(experiment, dependentVariable, independentVariables)
    if regressionType == 'ols':
        model = smf.ols(modelStr, data = data)
    elif regressionType == 'gls':
        model = smf.gls(modelStr, data = data)
    elif regressionType == 'rlm':
        model = smf.rlm(modelStr, data = data)
    else:
        print('Unknown regression type {}. Exiting'.format(regressionType))
        import sys
        sys.exit()
    return model.fit()

def runModels(experiments, data, dependentVariable, independentVariables, regressionType = 'ols'):
    '''Runs several models and stores 3 statistics
    adjusted R2, condition number (should be small, eg < 1000)
    and p-value for Shapiro-Wilk test of residual normality'''
    for i,experiment in experiments.iterrows():
        if experiment[independentVariables].any():
            results = runModel(experiment, data, dependentVariable, independentVariables, regressionType = 'ols')
            experiments.loc[i,'r2adj'] = results.rsquared_adj
            experiments.loc[i,'condNum'] = results.condition_number
            experiments.loc[i, 'shapiroP'] = shapiro(results.resid)[1]
            experiments.loc[i,'nobs'] = int(results.nobs)
    return experiments

def generateExperiments(independentVariables):
    '''Generates all possible models for including or not each independent variable'''
    from pandas import DataFrame
    experiments = {}
    nIndependentVariables = len(independentVariables)
    if nIndependentVariables != len(set(independentVariables)):
        print("Duplicate variables. Exiting")
        import sys
        sys.exit()
    nModels = 2**nIndependentVariables
    for i,var in enumerate(independentVariables):
        pattern = [False]*(2**i)+[True]*(2**i)
        experiments[var] = pattern*(2**(nIndependentVariables-i-1))
    experiments = DataFrame(experiments)
    experiments['r2adj'] = 0.
    experiments['condNum'] = NaN
    experiments['shapiroP'] = -1
    experiments['nobs'] = -1
    return experiments

def findBestModel(data, dependentVariable, independentVariables, regressionType = 'ols', nProcesses = 1):
    '''Generates all possible model with the independentVariables
    and runs them, saving the results in experiments
    with multiprocess option'''
    from pandas import concat
    from multiprocessing import Pool
    experiments = generateExperiments(independentVariables)
    nModels = len(experiments)
    print("Running {} models with {} processes".format(nModels, nProcesses))
    print("IndependentVariables: {}".format(independentVariables))
    if nProcesses == 1:
        return runModels(experiments, data, dependentVariable, independentVariables, regressionType)
    else:
        pool = Pool(processes = nProcesses)
        chunkSize = int(ceil(nModels/nProcesses))
        jobs = [pool.apply_async(runModels, args = (experiments[i*chunkSize:(i+1)*chunkSize], data, dependentVariable, independentVariables, regressionType)) for i in range(nProcesses)]
        return concat([job.get() for job in jobs])

def findBestModelFwd(data, dependentVariable, independentVariables, modelFunc, experiments = None):
    '''Forward search for best model (based on adjusted R2)
    Randomly starting with one variable and adding randomly variables 
    if they improve the model
    
    The results are added to experiments if provided as argument
    Storing in experiment relies on the index being the number equal 
    to the binary code derived from the independent variables'''
    from numpy.random import permutation as nppermutation
    if experiments is None:
        experiments = generateExperiments(independentVariables)
    nIndependentVariables = len(independentVariables)
    permutation = nppermutation(range(nIndependentVariables)).tolist()
    variableMapping = {j: independentVariables[i] for i,j in enumerate(permutation)}
    print('Tested variables '+', '.join([variableMapping[i] for i in xrange(nIndependentVariables)]))
    bestModel = [False]*nIndependentVariables
    currentVarNum = 0
    currentR2Adj = 0.
    for currentVarNum in xrange(nIndependentVariables):
        currentModel = [i for i in bestModel]
        currentModel[currentVarNum] = True
        rowIdx = sum([0]+[2**i for i in xrange(nIndependentVariables) if currentModel[permutation[i]]])
        #print currentVarNum, sum(currentModel), ', '.join([independentVariables[i] for i in xrange(nIndependentVariables) if currentModel[permutation[i]]])
        if experiments.loc[rowIdx, 'shapiroP'] < 0:
            modelStr = modelString(experiments.loc[rowIdx], dependentVariable, independentVariables)
            model = modelFunc(modelStr, data = data)
            results = model.fit()
            experiments.loc[rowIdx, 'r2adj'] = results.rsquared_adj
            experiments.loc[rowIdx, 'condNum'] = results.condition_number
            experiments.loc[rowIdx, 'shapiroP'] = shapiro(results.resid)[1]
            experiments.loc[rowIdx, 'nobs'] = int(results.nobs)
        if currentR2Adj < experiments.loc[rowIdx, 'r2adj']:
            currentR2Adj = experiments.loc[rowIdx, 'r2adj']
            bestModel[currentVarNum] = True
    return experiments

def displayModelResults(results, model = None, plotFigures = True, filenamePrefix = None, figureFileType = 'pdf', text = {'title-shapiro': 'Shapiro-Wilk normality test for residuals: {:.2f} (p={:.3f})', 'true-predicted.xlabel': 'Predicted values', 'true-predicted.ylabel': 'True values', 'residuals-predicted.xlabel': 'Predicted values', 'residuals-predicted.ylabel': 'Residuals'}):
    import statsmodels.api as sm
    '''Displays some model results

    3 graphics, true-predicted, residuals-predicted, '''
    print(results.summary())
    shapiroResult = shapiro(results.resid)
    print(shapiroResult)
    if plotFigures:
        fig = plt.figure(figsize=(7,6.3*(2+int(model is not None))))
        if model is not None:
            ax = fig.add_subplot(3,1,1)
            plt.plot(results.predict(), model.endog, 'x')
            x=plt.xlim()
            y=plt.ylim()
            plt.plot([max(x[0], y[0]), min(x[1], y[1])], [max(x[0], y[0]), min(x[1], y[1])], 'r')
            #plt.axis('equal')
            if text is not None:
                plt.title(text['title-shapiro'].format(*shapiroResult))
                #plt.title(text['true-predicted.title'])
                plt.xlabel(text['true-predicted.xlabel'])
                plt.ylabel(text['true-predicted.ylabel'])
            fig.add_subplot(3,1,2, sharex = ax)
            plt.plot(results.predict(), results.resid, 'x')
            nextSubplotNum = 3
        else:
            fig.add_subplot(2,1,1)
            plt.plot(results.predict(), results.resid, 'x')
            nextSubplotNum = 2
        if text is not None:
            if model is None:
                plt.title(text['title-shapiro'].format(*shapiroResult))
            plt.xlabel(text['residuals-predicted.xlabel'])
            plt.ylabel(text['residuals-predicted.ylabel'])
        qqAx = fig.add_subplot(nextSubplotNum,1,nextSubplotNum)
        sm.qqplot(results.resid, fit = True, line = '45', ax = qqAx)
        plt.axis('equal')
        if text is not None and 'qqplot.xlabel' in text:
            plt.xlabel(text['qqplot.xlabel'])
            plt.ylabel(text['qqplot.ylabel'])
        plt.tight_layout()
        if filenamePrefix is not None:
            from storage import openCheck
            out = openCheck(filenamePrefix+'-coefficients.html', 'w')
            out.write(results.summary().as_html())
            plt.savefig(filenamePrefix+'-model-results.'+figureFileType)

#########################
# iterable section
#########################

def mostCommon(L):
    '''Returns the most frequent element in a iterable

    taken from http://stackoverflow.com/questions/1518522/python-most-common-element-in-a-list'''
    from itertools import groupby
    from operator import itemgetter
    # get an iterable of (item, iterable) pairs
    SL = sorted((x, i) for i, x in enumerate(L))
    # print 'SL:', SL
    groups = groupby(SL, key=itemgetter(0))
    # auxiliary function to get "quality" for an item
    def _auxfun(g):
        item, iterable = g
        count = 0
        min_index = len(L)
        for _, where in iterable:
            count += 1
            min_index = min(min_index, where)
            # print 'item %r, count %r, minind %r' % (item, count, min_index)
        return count, -min_index
    # pick the highest-count/earliest item
    return max(groups, key=_auxfun)[0]

#########################
# sequence section
#########################

class LCSS(object):
    '''Class that keeps the LCSS parameters
    and puts together the various computations

    the methods with names starting with _ are not to be shadowed
    in child classes, who will shadow the other methods, 
    ie compute and computeXX methods'''
    def __init__(self, similarityFunc = None, metric = None, epsilon = None, delta = float('inf'), aligned = False, lengthFunc = min):
        '''One should provide either a similarity function
        that indicates (return bool) whether elements in the compares lists are similar

        eg distance(p1, p2) < epsilon
        
        or a type of metric usable in scipy.spatial.distance.cdist with an epsilon'''
        if similarityFunc is None and metric is None:
            print("No way to compute LCSS, similarityFunc and metric are None. Exiting")
            import sys
            sys.exit()
        elif metric is not None and epsilon is None:
            print("Please provide a value for epsilon if using a cdist metric. Exiting")
            import sys
            sys.exit()
        else:
            if similarityFunc is None and metric is not None and not isinf(delta):
                print('Warning: you are using a cdist metric and a finite delta, which will make probably computation slower than using the equivalent similarityFunc (since all pairwise distances will be computed by cdist).')
            self.similarityFunc = similarityFunc
            self.metric = metric
            self.epsilon = epsilon
            self.aligned = aligned
            self.delta = delta
            self.lengthFunc = lengthFunc
            self.subSequenceIndices = [(0,0)]

    def similarities(self, l1, l2, jshift=0):
        n1 = len(l1)
        n2 = len(l2)
        self.similarityTable = zeros((n1+1,n2+1), dtype = npint)
        if self.similarityFunc is not None:
            for i in xrange(1,n1+1):
                for j in xrange(max(1,i-jshift-self.delta),min(n2,i-jshift+self.delta)+1):
                    if self.similarityFunc(l1[i-1], l2[j-1]):
                        self.similarityTable[i,j] = self.similarityTable[i-1,j-1]+1
                    else:
                        self.similarityTable[i,j] = max(self.similarityTable[i-1,j], self.similarityTable[i,j-1])
        elif self.metric is not None:
            similarElements = distance.cdist(l1, l2, self.metric) <= self.epsilon
            for i in xrange(1,n1+1):
                for j in xrange(max(1,i-jshift-self.delta),min(n2,i-jshift+self.delta)+1):
                    if similarElements[i-1, j-1]:
                        self.similarityTable[i,j] = self.similarityTable[i-1,j-1]+1
                    else:
                        self.similarityTable[i,j] = max(self.similarityTable[i-1,j], self.similarityTable[i,j-1])
            

    def subSequence(self, i, j):
        '''Returns the subsequence of two sequences
        http://en.wikipedia.org/wiki/Longest_common_subsequence_problem'''
        if i == 0 or j == 0:
            return []
        elif self.similarityTable[i][j] == self.similarityTable[i][j-1]:
            return self.subSequence(i, j-1)
        elif self.similarityTable[i][j] == self.similarityTable[i-1][j]:
            return self.subSequence(i-1, j)
        else:
            return self.subSequence(i-1, j-1) + [(i-1,j-1)]

    def _compute(self, _l1, _l2, computeSubSequence = False):
        '''returns the longest common subsequence similarity
        l1 and l2 should be the right format
        eg list of tuple points for cdist 
        or elements that can be compare using similarityFunc

        if aligned, returns the best matching if using a finite delta by shifting the series alignments
        '''
        if len(_l2) < len(_l1): # l1 is the shortest
            l1 = _l2
            l2 = _l1
            revertIndices = True
        else:
            l1 = _l1
            l2 = _l2
            revertIndices = False
        n1 = len(l1)
        n2 = len(l2)

        if self.aligned:
            lcssValues = {}
            similarityTables = {}
            for i in xrange(-n2-self.delta+1, n1+self.delta): # interval such that [i-shift-delta, i-shift+delta] is never empty, which happens when i-shift+delta < 1 or when i-shift-delta > n2
                self.similarities(l1, l2, i)
                lcssValues[i] = self.similarityTable.max()
                similarityTables[i] = self.similarityTable
                #print self.similarityTable
            alignmentShift = argmaxDict(lcssValues) # ideally get the medium alignment shift, the one that minimizes distance
            self.similarityTable = similarityTables[alignmentShift]
        else:
            alignmentShift = 0
            self.similarities(l1, l2)

        # threshold values for the useful part of the similarity table are n2-n1-delta and n1-n2-delta
        self.similarityTable = self.similarityTable[:min(n1, n2+alignmentShift+self.delta)+1, :min(n2, n1-alignmentShift+self.delta)+1]

        if computeSubSequence:
            self.subSequenceIndices = self.subSequence(self.similarityTable.shape[0]-1, self.similarityTable.shape[1]-1)
            if revertIndices:
                self.subSequenceIndices = [(j,i) for i,j in self.subSequenceIndices]
        return self.similarityTable[-1,-1]

    def compute(self, l1, l2, computeSubSequence = False):
        '''get methods are to be shadowed in child classes '''
        return self._compute(l1, l2, computeSubSequence)

    def computeAlignment(self):
        return mean([j-i for i,j in self.subSequenceIndices])

    def _computeNormalized(self, l1, l2, computeSubSequence = False):
        ''' compute the normalized LCSS
        ie, the LCSS divided by the min or mean of the indicator lengths (using lengthFunc)
        lengthFunc = lambda x,y:float(x,y)/2'''
        return float(self._compute(l1, l2, computeSubSequence))/self.lengthFunc(len(l1), len(l2))

    def computeNormalized(self, l1, l2, computeSubSequence = False):
        return self._computeNormalized(l1, l2, computeSubSequence)

    def _computeDistance(self, l1, l2, computeSubSequence = False):
        ''' compute the LCSS distance'''
        return 1-self._computeNormalized(l1, l2, computeSubSequence)

    def computeDistance(self, l1, l2, computeSubSequence = False):
        return self._computeDistance(l1, l2, computeSubSequence)
    
#########################
# plotting section
#########################

def plotPolygon(poly, options = ''):
    'Plots shapely polygon poly'
    from numpy.core.multiarray import array
    from matplotlib.pyplot import plot
    from shapely.geometry import Polygon

    tmp = array(poly.exterior)
    plot(tmp[:,0], tmp[:,1], options)

def stepPlot(X, firstX, lastX, initialCount = 0, increment = 1):
    '''for each value in X, increment by increment the initial count
    returns the lists that can be plotted 
    to obtain a step plot increasing by one for each value in x, from first to last value
    firstX and lastX should be respectively smaller and larger than all elements in X'''
    
    sortedX = []
    counts = [initialCount]
    for x in sorted(X):
        sortedX += [x,x]
        counts.append(counts[-1])
        counts.append(counts[-1]+increment)
    counts.append(counts[-1])
    return [firstX]+sortedX+[lastX], counts

class PlottingPropertyValues(object):
    def __init__(self, values):
        self.values = values

    def __getitem__(self, i):
        return self.values[i%len(self.values)]

markers = PlottingPropertyValues(['+', '*', ',', '.', 'x', 'D', 's', 'o'])
scatterMarkers = PlottingPropertyValues(['s','o','^','>','v','<','d','p','h','8','+','x'])

linestyles = PlottingPropertyValues(['-', '--', '-.', ':'])

colors = PlottingPropertyValues('brgmyck') # 'w'

def plotIndicatorMap(indicatorMap, squareSize, masked = True, defaultValue=-1):
    from matplotlib.pyplot import pcolor
    coords = array(indicatorMap.keys())
    minX = min(coords[:,0])
    minY = min(coords[:,1])
    X = arange(minX, max(coords[:,0])+1.1)*squareSize
    Y = arange(minY, max(coords[:,1])+1.1)*squareSize
    C = defaultValue*ones((len(Y), len(X)))
    for k,v in indicatorMap.iteritems():
        C[k[1]-minY,k[0]-minX] = v
    if masked:
        pcolor(X, Y, ma.masked_where(C==defaultValue,C))
    else:
        pcolor(X, Y, C)

#########################
# Data download
#########################

def downloadECWeather(stationID, years, months = [], outputDirectoryname = '.', english = True):
    '''Downloads monthly weather data from Environment Canada
    If month is provided (number 1 to 12), it means hourly data for the whole month
    Otherwise, means the data for each day, for the whole year

    Example: MONTREAL MCTAVISH	10761
             MONTREALPIERRE ELLIOTT TRUDEAU INTL A	5415

    To get daily data for 2010 and 2011, downloadECWeather(10761, [2010,2011], [], '/tmp')
    To get hourly data for 2009 and 2012, January, March and October, downloadECWeather(10761, [2009,2012], [1,3,10], '/tmp')'''
    import urllib2
    if english:
        language = 'e'
    else:
        language = 'f'
    if len(months) == 0:
        timeFrame = 2
        months = [1]
    else:
        timeFrame = 1

    for year in years:
        for month in months:
            url = urllib2.urlopen('http://climat.meteo.gc.ca/climateData/bulkdata_{}.html?format=csv&stationID={}&Year={}&Month={}&Day=1&timeframe={}&submit=++T%C3%A9l%C3%A9charger+%0D%0Ades+donn%C3%A9es'.format(language, stationID, year, month, timeFrame))
            data = url.read()
            outFilename = '{}/{}-{}'.format(outputDirectoryname, stationID, year)
            if timeFrame == 1:
                outFilename += '-{}-hourly'.format(month)
            else:
                outFilename += '-daily'
            outFilename += '.csv'
            out = open(outFilename, 'w')
            out.write(data)
            out.close()

#########################
# File I/O
#########################

def removeExtension(filename, delimiter = '.'):
    '''Returns the filename minus the extension (all characters after last .)'''
    i = filename.rfind(delimiter)
    if i>0:
        return filename[:i]
    else:
        return filename

def cleanFilename(s):
    'cleans filenames obtained when contatenating figure characteristics'
    return s.replace(' ','-').replace('.','').replace('/','-').replace(',','')

def listfiles(dirname, extension, remove = False):
    '''Returns the list of files with the extension in the directory dirname
    If remove is True, the filenames are stripped from the extension'''
    from os import listdir
    tmp = [f for f in listdir(dirname) if f.endswith(extension)]
    tmp.sort()
    if remove:
        return [removeExtension(f, extension) for f in tmp]
    else:
        return tmp

def mkdir(dirname):
    'Creates a directory if it does not exist'
    import os
    if not os.path.exists(dirname):
        os.mkdir(dirname)
    else:
        print(dirname+' already exists')

def removeFile(filename):
    '''Deletes the file while avoiding raising an error 
    if the file does not exist'''
    import os
    if (os.path.exists(filename)):
        os.remove(filename)
    else:
        print(filename+' does not exist')

def line2Floats(l, separator=' '):
    '''Returns the list of floats corresponding to the string'''
    return [float(x) for x in l.split(separator)]

def line2Ints(l, separator=' '):
    '''Returns the list of ints corresponding to the string'''
    return [int(x) for x in l.split(separator)]

#########################
# CLI utils
#########################

def parseCLIOptions(helpMessage, options, cliArgs, optionalOptions=[]):
    ''' Simple function to handle similar argument parsing
    Returns the dictionary of options and their values

    * cliArgs are most likely directly sys.argv 
    (only the elements after the first one are considered)
    
    * options should be a list of strings for getopt options, 
    eg ['frame=','correspondences=','video=']
    A value must be provided for each option, or the program quits'''
    import sys, getopt
    from numpy.core.fromnumeric import all
    optionValues, args = getopt.getopt(cliArgs[1:], 'h', ['help']+options+optionalOptions)
    optionValues = dict(optionValues)

    if '--help' in optionValues.keys() or '-h' in optionValues.keys():
        print(helpMessage+
              '\n - Compulsory options: '+' '.join([opt.replace('=','') for opt in options])+
              '\n - Non-compulsory options: '+' '.join([opt.replace('=','') for opt in optionalOptions]))
        sys.exit()

    missingArgument = [('--'+opt.replace('=','') in optionValues.keys()) for opt in options]
    if not all(missingArgument):
        print('Missing argument')
        print(optionValues)
        sys.exit()

    return optionValues


#########################
# Profiling
#########################

def analyzeProfile(profileFilename, stripDirs = True):
    '''Analyze the file produced by cProfile 

    obtained by for example: 
    - call in script (for main() function in script)
    import cProfile, os
    cProfile.run('main()', os.path.join(os.getcwd(),'main.profile'))

    - or on the command line:
    python -m cProfile [-o profile.bin] [-s sort] scriptfile [arg]'''
    import pstats, os
    p = pstats.Stats(os.path.join(os.pardir, profileFilename))
    if stripDirs:
        p.strip_dirs()
    p.sort_stats('time')
    p.print_stats(.2)
    #p.sort_stats('time')
    # p.print_callees(.1, 'int_prediction.py:')
    return p

#########################
# running tests
#########################

if __name__ == "__main__":
    import doctest
    import unittest
    suite = doctest.DocFileSuite('tests/utils.txt')
    #suite = doctest.DocTestSuite()
    unittest.TextTestRunner().run(suite)
    #doctest.testmod()
    #doctest.testfile("example.txt")
