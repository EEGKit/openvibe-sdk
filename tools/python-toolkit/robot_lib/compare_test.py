#!usr/bin/env python
# ###################################################################
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


class CompareLibrary(object):
    def __init__(self):
        pass

    def should_be_higher_or_equal(self, first_value, second_value):
        """This creates a keyword named "Should be higher or equal"


        :param first_value:
        :param second_value:
        :raise AssertionError: exception if first_value is lower than second_value

        Examples:

        | Should be higher or equal | 1 | 2 | # Result is an exception The comparison fail 1 is lower thant 2
        | Should be higher or equal | 2 | 1 |

        """

        if first_value < second_value:
            raise AssertionError('The comparison fails %d is lower than %d' % (first_value, second_value))
