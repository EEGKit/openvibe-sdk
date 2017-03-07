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

import numpy as np
import pandas as pd
from csv_test import CsvLibrary
from mensiasigpro.compare import compare_two_arrays
from mensiasigpro.epoching import stimulation_based_epoching


class StimulationBasedEpoching(object):
    def __init__(self):
        pass

    def epoching_reference(self, input_data_csv, output_data_csv, interstimulation_interval,
                                              epoch_duration, epoch_offset, sampling_rate, test_temporary_dir,
                                              test_name):
        """This creates a keyword named "Epoching Reference"
        The goal is to compare the result of box stimulation based epoching and result computed with the keyword

        :param input_data_csv: csv data recorded before stimulation based epoching
        :param output_data_csv: csv data recorded after stimulation based epoching
        :param interstimulation_interval: interstimulation interval set (ie clock stimulator box) : integer
        :param epoch_duration: epoch duration set in the stimulation based epoching box : integer
        :param epoch_offset: epoch offset set in the stimulation based epoching box : integer
        :param sampling_rate: sampling rate set signal generator (ie time signal box)
        :param test_temporary_dir: directory with all files generated during the test
        :param test_name: test name
        :return: path of output csv file (epoching signal)

        :raise AssertionError: raise an exception if there is a difference between the box result and the computation result.
        This also generates, two output csv files with computation results and inputs for investigations.

        Example:

        input_data_csv: input-stim-based-epoch-test-sampling-rate-512.csv
        output_data_csv: output-stim-base-epoch-test-sampling-rate-512.csv
        interstimulation_interval: 2
        epoch_duration: 1
        epoch_offset: 1
        sampling_rate: 512
        test_temporary_dir: /home/certivibe/testoutput
        test_name: test-sampling-rate-512


        Case 1: The box stimulation based epoching result is good, the keyword won't raise anything.

        | Epoching results should be successful | input-stim-based-epoch-test-sampling-rate-512.csv |
        output-stim-base-epoch-test-sampling-rate-512.csv | 2 | 1 | 1 | 512 | /home/certivibe/testoutput |test-sampling-rate-512 |

        Case 2: The box stimulation based epoching result is wrong, the keyword will raise an exception
        and create 2 csv files:
        "test-sampling-rate-512-input.csv"
        "test-sampling-rate-512-output.csv"
        in the test temporary directory: /home/certivibe/testoutput.

        | Epoching results should be successful | input-stim-based-epoch-test-sampling-rate-512.csv |
        output-stim-base-epoch-test-sampling-rate-512.csv | 2 | 1 | 1 | 512 | /home/certivibe/testoutput |test-sampling-rate-512 |

        """
        sampling_rate = int(sampling_rate)
        epoch_duration = float(epoch_duration)
        epoch_offset = float(epoch_offset)
        interstimulation_interval = float(interstimulation_interval)

        # Convert files to data frames and remove the last columns that is not useful
        input = CsvLibrary.read_openvibe_csv_file(input_data_csv, [-1])
        output_box = CsvLibrary.read_openvibe_csv_file(output_data_csv, [-1])
        timer_max = len(output_box)/sampling_rate

        # Provide a data frame with the computed value
        epoch_reference = stimulation_based_epoching(input, epoch_duration, epoch_offset,
                                                     interstimulation_interval, sampling_rate, timer_max)

        # Check the value of both epoch are close enough
        path_output_reference = test_temporary_dir + test_name + '-input.csv'
        output_box.to_csv(test_temporary_dir + test_name + '-output.csv', sep=';')
        epoch_reference.to_csv(path_output_reference, sep=';')

        return path_output_reference
