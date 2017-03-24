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


#The goal of this function is to compare values of cell from two files

def compareCells (file1,file2):

    # select the ligne for both csv file
    for inputLigne, outputLigne in zip(file1,file2):

        #check the file size of input and output files 
        if len(inputLigne)!=len(outputLigne):
            print('input and output cells size are different')
            print('error on ligne %s'%(inputData.index(inputLigne)+2))
            print('input cells: %s'%len(inputLigne))
            print('output cells: %s'%len(outputLigne))
            sys.exit(107)



        #Select the cell for both csv file
        for inputCell, outputCell in zip(inputLigne,outputLigne):
             #check if the cell wasn't empty
             if inputCell !="":
                         
                #Convert cell value in float and check if the values weren't close enough
                if abs(float(outputCell)-float(inputCell)) > sys.float_info.epsilon:
                                   
                    print('error on ligne %s at ligne %s and column %s'%(inputLigne, (inputData.index(inputLigne)+2),(inputLigne.index(inputCell)+1)))
                    print( 'the input cell %s is different from output cell %s'%(inputCell,outputCell))

                    sys.exit(107)



try:

    #open files associated with the varaibles
    fileReferenceData= open(referenceData, 'r')
    fileTestData= open(testData, 'r')

    #use csv object to access the csv files
    readerReferenceData = csv.reader(fileReferenceData, delimiter=';')
    readerTestData = csv.reader(fileTestData, delimiter=';')
    inputData=[]
    outputData=[]

    #check the file size of input and output files 
    if len(open(referenceData).readlines())!=len(open(testData).readlines()):
        print('input and output files size are different') 
        print('input lines: %s'%len(open(referenceData).readlines()))
        print('output lines: %s'%len(open(testData).readlines()))  
        sys.exit(107)

    ## convert csv object into list
    for rowref, rowtest in zip(readerReferenceData,readerTestData):
        inputData.append(rowref)
        outputData.append(rowtest)

    # remove the first line with the header of the csv file
    inputData.pop(0)
    outputData.pop(0)
    
    #perform the cell comparison
    compareCells(inputData,outputData)

    print('All values are identiqual')



except IOError as err:
        print('missing input or ouput file')    
        print('missing file: %s'%(err.filename))
        sys.exit(106)
       

except:
    raise

sys.exit()