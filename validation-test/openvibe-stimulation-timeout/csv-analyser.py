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

import csv
import sys

# test script parameter arg1: reference file csv 
# test script parameter arg2: test file csv

# assign arguments to variable
referenceData=sys.argv[1]
testData=sys.argv[2]

#open files associated with the varaibles
fileReferenceData= open(referenceData, 'r')
fileTestData= open(testData, 'r')



try:
    #use csv object to access the csv files
    readerReferenceData = csv.reader(fileReferenceData, delimiter=';')
    readerTestData = csv.reader(fileTestData, delimiter=';')

  

    #parse the csv file to find the appropriate line
    for rowReference in readerReferenceData:
        #check if the csv is the expeted one
        if rowReference[0] == 'Time (s)':
            print('csv file correct:%s'%referenceData)

        #search the value 2 second in the 1st column
        elif float(rowReference[0]) == 2:
            referenceLine = rowReference
            break
    
    for rowTest in readerTestData:
        #check if the csv is the expeted one
        if rowTest[0] == 'Time (s)':
            print('csv file correct:%s'%testData)

        #search the value 2 second in the 1st column
        elif float(rowTest[0]) == 2:
            testLine = rowTest
            break

    
finally:
    fileReferenceData.close()
    fileTestData.close()

#Remove the last item of the list.
#This item isn't relevant because openvibe add the sampling rate only on the first line of the csv output. This could trigger a non relevant error
referenceLine.pop()
testLine.pop()

#Check if each value in each list are the same

# remove the first line with string
if len(referenceLine)==len(testLine):
    
    #check with each line each value of the line 
    for index, ref in enumerate(referenceLine, start=0):
        if abs(float(referenceLine[index])-float(testLine[index])) > 0.000001:
            print('Fail to compare at index %d' %(index))
            print('Reference line is: %r'%referenceLine)
            print('Test line is: %r' %testLine)
            sys.exit(100)
else:
    print ('list size are different')
    print ('Reference line is : %r'%referenceLine)
    print ('Reference line is size: %r'%len(referenceLine))
    print ('Test line is: %r'%testLine)
    print ('Test line is size: %r'%len(testLine))
    sys.exit(101)
sys.exit()

