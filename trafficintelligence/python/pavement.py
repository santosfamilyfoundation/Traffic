#! /usr/bin/env python
'''Tools for processing and analyzing pavement marking data'''

import utils

import numpy as np

paintTypes = {
    0: "Non-existant",  # None/non-existent paint
    1: "Eau",  # Water-based paint
    2: "Epoxy",  # Epoxy-based paint
    3: "Alkyde",  # Oil/solvent-based alkyd paint
    4: "Autre"  # Other types of paint
}

durabilities = {  # Define a scale of durability
    1: 98,  # 96 to 100
    2: 85,  # 75 to 96
    3: 62,  # 50 to 75
    4: 32,  # 15 to 50
    5: 7  # 0 to 15
}

roadFunctionalClasses = {
    40: "Collectrice",  # Collector road
    20: "Nationale",  # National road
    30: "Regionale",  # Regional road
    10: "Autoroute",  # Highway
    60: "Acces ressources",  # Resource access road (?)
    51: "Local 1",
    52: "Local 2",
    53: "Local 3",
    15: "Aut (PRN)",  # Highway ?
    25: "Nat (PRN)",  # National ?
    70: "Acces isolees",  # Isolated access road
    99: "Autres"  # Other types of road
}


def caracteristiques(rtss, maintenanceLevel, rtssWeatherStation, fmr, paintType):
    '''Computes characteristic data for the RTSS (class rtss)
    maintenanceLevel = pylab.csv2rec('C:\Users\Alexandre\Desktop\Projet_maitrise_recherche\BDD_access\\analyse_donnees_deneigement\\exigence_circuits.txt', delimiter = ';')
    rtssWeatherStation = pylab.csv2rec('C:\Users\Alexandre\Desktop\Projet_maitrise_recherche\stations_environnement_canada\\rtssWeatherStation\juste_pour_rtss_avec_donnees_entretien_hiv\\rtssWeatherStation_EC3.txt', delimiter = ',')
    fmr = pylab.csv2rec('C:\Users\Alexandre\Desktop\Projet_maitrise_recherche\BDD_access\\analyse_donnees_deneigement\\fmr.txt', delimiter = ';')
    paintType = pylab.csv2rec('C:\Users\Alexandre\Desktop\Projet_maitrise_recherche\BDD_access\\analyse_donnees_deneigement\\type_peinture.txt', delimiter = ';')
    '''
    # determination exigence deneigement
    if rtss.id in maintenanceLevel['rtss_debut']:
        for i in range(len(maintenanceLevel)):
            if maintenanceLevel['rtss_debut'][i] == rtss.id:
                exigence = maintenanceLevel['exigence'][i]
    else:
        exigence = ''

    # determination x/y
    if rtss.id in rtssWeatherStation['rtss']:
        for i in range(len(rtssWeatherStation)):
            if rtssWeatherStation['rtss'][i] == rtss.id:
                x_moy = rtssWeatherStation['x_moy'][i]
                y_moy = rtssWeatherStation['y_moy'][i]
    else:
        x_moy, y_moy = '', ''

    # determination info fmr
    age_revtm, classe_fonct, type_revtm, milieu, djma, pourc_camions, vit_max = [], [], [], [], [], [], []
    if rtss.id in fmr['rtss_debut']:
        for i in range(len(fmr)):
            if fmr['rtss_debut'][i] == rtss.id:
                age_revtm.append(fmr['age_revtm'][i])
                classe_fonct.append(fmr['des_clasf_fonct'][i])
                type_revtm.append(fmr['des_type_revtm'][i])
                milieu.append(fmr['des_cod_mil'][i])
                djma.append(fmr['val_djma'][i])
                pourc_camions.append(fmr['val_pourc_camns'][i])
                vit_max.append(fmr['val_limt_vitss'][i])
        age_revtm = utils.mostCommon(age_revtm)  # mostCommon
        classe_fonct = utils.mostCommon(classe_fonct)
        type_revtm = utils.mostCommon(type_revtm)
        milieu = utils.mostCommon(milieu)
        djma = utils.mostCommon(djma)
        vit_max = utils.mostCommon(vit_max)
        if vit_max < 0:
            vit_max = ''
        pourc_camions = utils.mostCommon(pourc_camions)
        if pourc_camions == "" or pourc_camions < 0:
            djma_camions = ""
        else:
            djma_camions = pourc_camions * djma / 100
    else:
        age_revtm, classe_fonct, type_revtm, milieu, djma, djma_camions, vit_max = '', '', '', '', '', '', ''

    # determination type peinture
    peinture_rd, peinture_rg, peinture_cl = [], [], []
    peinture_lrd, peinture_lrg, peinture_lc = 0, 0, 0
    if rtss.id in paintType['rtss_debut_orig']:
        for i in range(len(paintType)):
            if paintType['rtss_debut_orig'][i] == rtss.id:
                peinture_rd.append((paintType['peinture_rd'][i]))
                peinture_rg.append((paintType['peinture_rg'][i]))
                peinture_cl.append((paintType['peinture_cl'][i]))
        peinture_lrd = utils.mostCommon(peinture_rd)
        peinture_lrg = utils.mostCommon(peinture_rg)
        peinture_lc = utils.mostCommon(peinture_cl)
    else:
        peinture_lrd, peinture_lrg, peinture_lc = '', '', ''

    return (
        exigence, x_moy, y_moy, age_revtm, classe_fonct, type_revtm, milieu, djma, djma_camions, vit_max, peinture_lrd,
        peinture_lrg, peinture_lc)


def winterMaintenanceIndicators(data, startDate, endDate, circuitReference, snowThreshold):
    '''Computes several winter maintenance indicators
    data = entretien_hivernal = pylab.csv2rec('C:\Users\Alexandre\Documents\Cours\Poly\Projet\mesures_entretien_hivernal\mesures_deneigement.txt', delimiter = ',')'''
    import datetime
    # Initialize running sums
    somme_eau, somme_neige, somme_abrasif, somme_sel, somme_lc, somme_lrg, somme_lrd, compteur_premiere_neige, compteur_somme_abrasif = 0, 0, 0, 0, 0, 0, 0, 0, 0

    if circuitReference in data['ref_circuit']:
        for i in range(len(data)):
            if data['ref_circuit'][i] == circuitReference and (
                        data['date'][i] + datetime.timedelta(days=6)) <= endDate and (
                        data['date'][i] + datetime.timedelta(days=6)) > startDate:
                compteur_premiere_neige += float(data['premiere_neige'][i])
                somme_neige += float(data['neige'][i])
                somme_eau += float(data['eau'][i])
                somme_abrasif += float(data['abrasif'][i])
                somme_sel += float(data['sel'][i])
                somme_lc += float(data['lc'][i])
                somme_lrg += float(data['lrg'][i])
                somme_lrd += float(data['lrd'][i])
                compteur_somme_abrasif += float(
                    data['autre_abrasif_binaire'][i])
        if compteur_premiere_neige >= 1:  # If the first snow counter >=1,
            premiere_neige = 1  # First snow is True
        else:
            premiere_neige = 0
        if compteur_somme_abrasif >= 1:  # If the abrasive sum counter >=1,
            autres_abrasifs = 1  # Other abrasives is True
        else:
            autres_abrasifs = 0
        if somme_neige < snowThreshold:  # If sum of snow < snowThreshold,
            neigeMTQ_sup_seuil = 0  # The Quebec Transport. Ministry snow threshold is False
        else:
            neigeMTQ_sup_seuil = 1
    else:
        somme_eau, somme_neige, somme_abrasif, somme_sel, somme_lc, somme_lrg, somme_lrd, premiere_neige, autres_abrasifs, neigeMTQ_sup_seuil = '', '', '', '', '', '', '', '', '', ''

    return (somme_eau, somme_neige, neigeMTQ_sup_seuil, somme_abrasif, somme_sel, somme_lc, somme_lrg, somme_lrd,
            premiere_neige, autres_abrasifs)


def weatherIndicators(data, startDate, endDate, snowThreshold, weatherDatatype, minProportionMeasures=0.):
    """

    Args:
        data:
        startDate:
        endDate:
        snowThreshold:
        weatherDatatype:
        minProportionMeasures:

    Returns:

    """
    '''Computes the indicators from Environment Canada files
    (loaded as a recarray using csv2rec in data),
    between start and end dates (datetime.datetime objects)

    weatherDataType is to indicate Environnement Canada data ('ec') or else MTQ
    minProportionMeasures is proportion of measures necessary to consider the indicators'''
    from matplotlib.mlab import find
    nbre_jours_T_negatif, nbre_jours_gel_degel, pluie_tot, neige_tot, ecart_type_T = 0, 0, 0, 0, 0
    compteur, nbre_jours_gel_consecutifs = 0, 0
    tmoys = []
    seuils_T = [20, 15, 10, 5]
    deltas_T = [0, 0, 0, 0]
    startIndex = find(data['date'] == startDate)
    nDays = int((endDate - startDate).days) + 1
    if len(startIndex) > 0 and startIndex + nDays <= len(data):
        startIndex = startIndex[0]
        for i in range(startIndex, startIndex + nDays):
            if not np.isnan(data['tmax'][i]):
                tmax = data['tmax'][i]
            else:
                tmax = None
            if not np.isnan(data['tmin'][i]):
                tmin = data['tmin'][i]
            else:
                tmin = None
            if weatherDatatype == 'ec':
                if data['pluie_tot'][i] is not None and not np.isnan(data['pluie_tot'][i]):
                    pluie_tot += data['pluie_tot'][i]
                if data['neige_tot'][i] is not None and not np.isnan(data['neige_tot'][i]):
                    neige_tot += data['neige_tot'][i]
            if tmax is not None:
                if tmax < 0:
                    nbre_jours_T_negatif += 1
            if tmax is not None and tmin is not None:
                if tmax > 0 and tmin < 0:
                    nbre_jours_gel_degel += 1
                for l in range(len(seuils_T)):
                    if tmax - tmin >= seuils_T[l]:
                        deltas_T[l] += 1
            if not np.isnan(data['tmoy'][i]):
                tmoys.append(data['tmoy'][i])
            if tmax is not None:
                if tmax < 0:
                    compteur += 1
                elif tmax >= 0 and compteur >= nbre_jours_gel_consecutifs:
                    nbre_jours_gel_consecutifs = compteur
                    compteur = 0
                else:
                    compteur = 0
            nbre_jours_gel_consecutifs = max(
                nbre_jours_gel_consecutifs, compteur)
    if len(tmoys) > 0 and float(len(tmoys)) / nDays >= minProportionMeasures:
        if tmoys != []:
            ecart_type_T = np.std(tmoys)
        else:
            ecart_type = None
        if neige_tot < snowThreshold:
            neigeEC_sup_seuil = 0
        else:
            neigeEC_sup_seuil = 1
        return (nbre_jours_T_negatif, nbre_jours_gel_degel, deltas_T, nbre_jours_gel_consecutifs, pluie_tot, neige_tot,
                neigeEC_sup_seuil, ecart_type_T)
    else:
        return [None] * 2 + [[None] * len(seuils_T)] + [None] * 5


def mtqWeatherIndicators(data, startDate, endDate, tmax, tmin, tmoy):
    print("Deprecated, use weatherIndicators")
    from matplotlib.mlab import find
    nbre_jours_T_negatif, nbre_jours_gel_degel, ecart_type_T = 0, 0, 0
    compteur, nbre_jours_gel_consecutifs = 0, 0
    tmoys = []
    seuils_T = [20, 15, 10, 5]
    deltas_T = [0, 0, 0, 0]
    startIndex = find(data['date'] == startDate)
    nDays = (endDate - startDate).days + 1
    for i in range(startIndex, startIndex + nDays):
        if tmax[i] < 0:
            nbre_jours_T_negatif += 1
        if tmax[i] > 0 and tmin[i] < 0:
            nbre_jours_gel_degel += 1
        for l in range(len(seuils_T)):
            if tmax[i] - tmin[i] >= seuils_T[l]:
                deltas_T[l] += 1
        tmoys.append(tmoy[i])
        if tmax[i] < 0:
            compteur += 1
        elif tmax[i] >= 0 and compteur >= nbre_jours_gel_consecutifs:
            nbre_jours_gel_consecutifs = compteur
            compteur = 0
        else:
            compteur = 0
        nbre_jours_gel_consecutifs = max(nbre_jours_gel_consecutifs, compteur)
        if tmoys != []:
            ecart_type_T = np.std(tmoys)
        else:
            ecart_type = None

    return (nbre_jours_T_negatif, nbre_jours_gel_degel, deltas_T, nbre_jours_gel_consecutifs, ecart_type_T)


class RTSS(object):
    '''class for data related to a RTSS:
    - agregating pavement marking measurements
    - RTSS characteristics from FMR: pavement type, age, AADT, truck AADT
    - winter maintenance level from V155

    If divided highway, the RTSS ends with G or D and are distinct: there is no ambiguity
    - retroreflectivity types: there are CB, RJ and RB
    If undivided, ending with C
    - durability is fine: ETAT_MARQG_RG ETAT_MARQG_CL ETAT_MARQG_RD (+SG/SD, but recent)
    - retroreflectivity: CJ is center line, RB and SB are left/right if DEBUT-FIN>0 or <0
    '''

    def __init__(self, _id, name, data):
        self.id = _id
        self.name = name
        self.data = data


class MarkingTest(object):
    '''class for a test site for a given product

    including the series of measurements over the years'''

    def __init__(self, _id, paintingDate, paintingType, color, data):
        self.id = _id
        self.paintingDate = paintingDate
        self.paintingType = paintingType
        self.color = color
        self.data = data
        self.nMeasures = len(data)

    def getSite(self):
        return int(self.id[:2])

    def getTestAttributes(self):
        return [self.paintingType, self.color, self.paintingDate.year]

    def plot(self, measure, options='o', dayRatio=1., **kwargs):
        from matplotlib.pyplot import plot
        plot(self.data['jours'] / float(dayRatio),
             self.data[measure], options, **kwargs)

    def getMarkingMeasures(self, dataLabel):
        nonZeroIndices = ~np.isnan(self.data[dataLabel])
        return self.data[nonZeroIndices]['jours'], self.data[nonZeroIndices][dataLabel]

    def plotMarkingMeasures(self, measure, options='o', dayRatio=1., **kwargs):
        for i in range(1, 7):
            self.plot('{}_{}'.format(measure, i), options, dayRatio, **kwargs)

    def computeMarkingMeasureVariations(self, dataLabel, lanePositions, weatherData, snowThreshold,
                                        weatherDataType='ec', minProportionMeasures=0.):
        '''Computes for each successive measurement
        lanePositions = None
        measure variation, initial measure, time duration, weather indicators

        TODO if measurements per lane, add a variable for lane position (position1 to 6)
        lanePositions = list of integers (range(1,7))
        measure variation, initial measure, time duration, lane position1, weather indicators
        measure variation, initial measure, time duration, lane position2, weather indicators
        ...'''
        variationData = []
        if lanePositions is None:
            nonZeroIndices = ~np.isnan(self.data[dataLabel])
            days = self.data[nonZeroIndices]['jours']
            dates = self.data[nonZeroIndices]['date_mesure']
            measures = self.data[nonZeroIndices][dataLabel]
            for i in range(1, len(dates)):
                nDaysTNegative, nDaysThawFreeze, deltaTemp, nConsecutiveFrozenDays, totalRain, totalSnow, snowAboveThreshold, stdevTemp = weatherIndicators(
                    weatherData, dates[i - 1], dates[i], snowThreshold, weatherDataType, minProportionMeasures)
                if dates[i - 1].year + 1 == dates[i].year:
                    winter = 1
                    if days[i - 1] < 365:
                        firstWinter = 1
                else:
                    winter = 0
                    firstWinter = 0
                variationData.append(
                    [measures[i - 1] - measures[i], measures[i - 1], days[i] - days[i - 1], days[i - 1], winter,
                     firstWinter,
                     nDaysTNegative, nDaysThawFreeze] + deltaTemp + [nConsecutiveFrozenDays, totalRain, totalSnow,
                                                                     snowAboveThreshold, stdevTemp])
        return variationData
