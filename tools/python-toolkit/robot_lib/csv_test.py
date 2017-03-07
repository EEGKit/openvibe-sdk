#!usr/bin/env python
# -*- coding: utf-8 -*-
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

import csv
import pandas as pd


class CsvLibrary(object):
    def __init__(self):
        pass

    def read_csv_file(self, filename):
        """This creates a keyword named "Read CSV File"

         This keyword takes one argument, which is a path to a .csv file. It
         returns a list of rows, with each row being a list of the data in
         each column.
        :param filename: csv file with ; delimiter
        :return: list of row

         """
        data = []
        with open(filename, 'r') as csvfile:
            reader = csv.reader(csvfile, delimiter=';')
            for row in reader:
                data.append(row)
        return data

    def read_openvibe_csv_file(csv_openvibe_signal_file,columns=[-1,0]):
        """This creates a keyword named "Read Openvibe Csv File:

         This keyword takes one argument, which is a path to a .csv file. It
         returns a data frame but without the first and last columns of the csv file.

        :param filename: csv file with ";" delimiter
        :param columns: columns to remove
        :return: Data frame

         """
        data = pd.read_csv(csv_openvibe_signal_file, sep=';')
        data.drop(data.columns[columns], axis=1, inplace=True)
        return data
