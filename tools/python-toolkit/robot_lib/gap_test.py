#!usr/bin/env python
####################################################################
# INRIA and MENSIA TECHNOLOGIES SA, CONFIDENTIAL
#
# CertiViBE Test Software
# Copyright Inria and Mensia Technologies SA, 2015-2017
# All Rights Reserved
# 
# Author
# 
# NOTICE: All information contained herein is, and remains
# the property of Mensia Technologies SA and Inria.
# The intellectual and technical concepts contained
# herein are proprietary to Mensia Technologies SA and Inria,
# and are covered copyright law.
# Dissemination of this information or reproduction of this material
# is strictly forbidden unless prior written permission is obtained
# from Mensia Technologies SA and Inria.
####################################################################


class GapLibrary(object):
    def __init__(self):
        pass

    def gap_listing(self, input_list):
        """'This creates a keyword named "gap listing"

        :param input_list: list of rows
        :return: list of the first element of each rows

        The goal is to get all times values
        Examples:
        | ${list} =  | ['1.0000000000e+00', '32777', '0.0000000000e+00'],
        ['2.0000000000e+00', '32777', '0.0000000000e+00'],
        ['3.0000000000e+00', '32777', '0.0000000000e+00']  |
        | ${result} = | gap listing |  ${list} | # Result is ['1.0000000000e+00','2.0000000000e+00','3.0000000000e+00']

        """

        return [row[0] for row in input_list]

    def gap_computing(self, input_list):
        """This creates a keyword named "gap computing"

        :param input_list: list of float
        :return: list of float, the gap between each element of the list.

        Examples:
        | ${list} = | [1,2,3,4,5,6] |
        | ${result} = | gap computing | ${list} | # Result is [1.0,1.0,1.0,1.0,1.0]

        """

        stimulation_gap = []
        for index in range(0, len(input_list)-1):
            gap = float(input_list[index + 1]) - float(input_list[index])
            stimulation_gap.append(gap)
        return stimulation_gap

    def gap_comparison(self, input_list, stimulation_interval, epsilon):

        """This creates a keyword named "gap comparison"

        :param input_list: list of float, gap already computed
        :param stimulation_interval: stimulation interval set, float
        :param epsilon: the minimal error expected, float
        :raise AssertionError: when the difference between the stimulation interval and the computed gap is too high.

        """
        for gap_computed in input_list:
            if abs(float(stimulation_interval) - float(gap_computed)) > float(epsilon):
                raise AssertionError('gap between stimulation interval:%d and measured stimulation interval:%d too big'
                                     % (stimulation_interval, gap_computed))
