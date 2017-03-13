#######################################################################
# Software License Agreement (AGPL-3 License)
#
# CertiViBE Test Software
# Based on OpenViBE V1.1.0, Copyright (C) Inria, 2006-2015
# Copyright (C) Inria, 2015-2017,V1.0
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License version 3,
# as published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.
# If not, see <http://www.gnu.org/licenses/>.
#######################################################################

#######################################################################
# Script description
# Goal: check if the number of stimuations present after classification
# is identical to a reference result.
#
# Step 1: Define time windows between 2 stimulation id in the file before the classification,
# the first stimulation ID will be the reference stimulation ID.
# Step 2: Count how many time you get the reference stimulation ID in the time windows.
# Step 3: Compute the number of success and provide a ratio.
# Step 4: Compare the ratio to a reference value compute from previous test.
#######################################################################

import csv
import sys


try:
    # assign arguments to variables
    #arg 1 csv file recorded from ov file before the classification
    #arg 2 csv file recorded after the classification
    #arg 3 reference classification result
    # open files associated with the varaibles
    fileReferenceData = open(sys.argv[1], 'r')
    fileTestData = open(sys.argv[2], 'r')
    fileClassificationReference = open(sys.argv[3], 'r')


    # use csv object to access the csv files
    readerReferenceData = csv.reader(fileReferenceData, delimiter=';')
    readerTestData = csv.reader(fileTestData, delimiter=';')

    # Define all necessary list
    inputData = []
    outputData = []
    referenceData = []
    workingData = []
    classificationReferenceData = []

    # Convert classification reference file into list
    # Iterator in python can't be reset,
    # thus csv objects need to be converted to allow multiple iterations over the data
    for fileIndex in fileClassificationReference:
        classificationReferenceData.append(fileIndex)

    # Convert input data before classification into list
    # Iterator in python can't be reset,
    # thus csv objects need to be converted to allow multiple iterations over the data
    for rowReference in readerReferenceData:
        inputData.append(rowReference)

    # Convert output data after classification into list
    # Iterator in python can't be reseted,
    # csv object need to be converted to be iterate several times
    # Use next() to start after the header of the csv file
    next(readerTestData)
    for rowReference in readerTestData:
        outputData.append(rowReference)
    # Check if outputdata lenght has a value
    if len(outputData) <= 0:
        print('The lenght of the test data: %s is equal to 0 or lower'%sys.argv[2])
        sys.exit(109)
    indexInputFile = 0

    # Trick to add in the input file at the end of the list a timestamp reference
    # to create the last time window
    # Check if readerReferenceData line number has a value
    if readerReferenceData.line_num <= 0:
        print('The lenght of the file reference data: %s is equal to 0 or lower'%sys.argv[1])
        sys.exit(109)
    lastInputLine = inputData[readerReferenceData.line_num-1]

    # Select into input data the signal with good stimulation marker
    for rowReference in inputData:
        if rowReference[1] == '33026' or rowReference[1] == '33027' or rowReference[1] == '33025':
            referenceData.append(rowReference)
    referenceData.append(lastInputLine)
    # Check if the referenceData lenght has a value
    if len(referenceData) <= 0:
        print('The lenght of the reference data: %s is equal to 0 or lower'%sys.argv[2])
        sys.exit(109)
    inputlength = len(referenceData)

    #  Check in a time window define by two timestamp the number of stimuation
    while indexInputFile < (inputlength-1):
        # Create the first timestamp referenc
        firstLineReference = referenceData[indexInputFile]
        stimulationReference = firstLineReference[1]
        firstTimeReference = float(firstLineReference[0])
        # Create the second timestamp reference
        secondLineReference = referenceData[indexInputFile+1]
        secondTimeReference = float(secondLineReference[0])
        stimulationCount = 0
        numberOfStimulation = 0

        # Create the time window between the first and second time stamp reference
        #Count the reference stimulation ID
        for index in outputData:

            currentTimeReference = float(index[0])
            if (currentTimeReference > firstTimeReference) \
                and (currentTimeReference < secondTimeReference):
                stimulationCount = stimulationCount + index.count(stimulationReference)
                numberOfStimulation = numberOfStimulation + 1
            elif currentTimeReference > secondTimeReference:
                break
        #Compute the percentage of success
        classificationEvaluation = (stimulationCount/numberOfStimulation)*100
        workingData = []

        # Compare to the reference value
        if abs(classificationEvaluation - float(classificationReferenceData[indexInputFile])) \
            > sys.float_info.epsilon:

            print('Classification refference value:%s'%classificationReferenceData[indexInputFile])
            print('Classification evaluation value:%s'%classificationEvaluation)
            sys.exit(108)
        indexInputFile = indexInputFile+1

# exception raised if missing file
except FileNotFoundError as error:
    print('Missing file:%s'%error.filename)

# exception raise if output file empty
except StopIteration:
    print('error in output file:%s'%sys.argv[2])
    sys.exit(110)

except:
    raise

finally:
    try:
        fileReferenceData.close()
        fileTestData.close()
        fileClassificationReference.close()
    # catch exception when close fail due to missing file
    except NameError:
        sys.exit(110)

sys.exit()