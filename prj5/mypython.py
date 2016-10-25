################
# Eric Stevens #
# CS 344 - OS  #
# Program 5    #
################

import string # work with strings 
import random # generate random values
import os     # used to clear terminal


# This function will return a 10 lowercase char string
def RandStr10():
	myString = random.choice(string.ascii_lowercase) # initalize
	for x in range(0,9): #loop the rest of the chars
		myString = myString + random.choice(string.ascii_lowercase)
	myString = myString + '\n' # add new line char
	return myString #retrun string

# This function will write the string it is handed to the filename its handed
def CreateFile(fileName, inputString):
	fo = open(fileName, "wb") # open file object
	fo.write(inputString) # write to file
	fo.close #close file io

##############################################3

# generate random strings
string1 = RandStr10()
string2 = RandStr10()
string3 = RandStr10()

# create files and place random strings in them
CreateFile("file1", string1)
CreateFile("file2", string2)
CreateFile("file3", string3)

os.system('clear') # clear terminal

##########################################
# terminal output #
print '\n\nThe contents of the files are\n'
print 'file 1: ' + string1
print 'file 2: ' + string2
print 'file 3: ' + string3

# generate random number and multiply them
number1 = random.randint(0,42)
number2 = random.randint(0,42)
product = number1 * number2

print '\n\nHere are some random numbers\n'
print 'number 1: ' + str(number1) + '\n'
print 'number 2: ' + str(number2) + '\n'
print 'product: ' + str(product) + '\n\n\n'
###########################################




