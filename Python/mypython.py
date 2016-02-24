#!/usr/bin/env python3

#Assignment 5
#Author: Sean Mulholland
#Date: 7/8/15
#email: mulholls@onid.oregonstate.edu

import sys
import random
import string

#Function: createFile(string)
#Description: Creates a file named after the passed string. If it can't,
#  error is written to console and the program exits.
#Pre-conditions: Files in directory with names passed to function must not exist
#   or are ok to be overwritten. If files exist, must have read/write permission
#Post-conditions: File is created/opened and closed
def createFile(fName):
   try:
      file = open(fName, 'w')
      file.close()
   except IOError:
      print("Creating " + fName + " didn't work!")
      sys.exit(1)

#Function: tenRandChar(string)
#Description: Opens and writes 10 random characters to a file named "string"
#Pre-conditions: File named "string" SHOULD exist already with read/write
#  permissions. All file contents should be ok to be overwritten.
#Post-conditions: Ten random lowercase letters are written into file "string"
def tenRandChar(fName):
   try:
      file = open(fName, 'w')
      for i in range(10):
         randLetter = random.choice(string.ascii_lowercase)
         file.write(randLetter)
      file.close()
   except IOError:
      print("Writing the random letters to " + fName + " didn't work!")
      sys.exit(1)

#Function: printFile(string)
#Description: Prints the first line of a file named "string"
#Pre-Conditions: File named "string" should exist with read/write permissions
#Post-Conditions: None
def printFile(fName):
   try:
      file = open(fName, 'r')
      str_to_write = file.readline()
      print(str_to_write, "\n")
      file.close()
   except IOError:
      print("Printing " + fName + " didn't work!\n")
      sys.exit(1)

#Create the 3 necessary files
createFile("file_one")
createFile("file_two")
createFile("file_three")

#Fill file_one with contents and print contents
print("file_one contents:\n")
tenRandChar("file_one")
printFile("file_one")
print

#Fill file_two with contents and print contents
print("file_two contents:\n")
tenRandChar("file_two")
printFile("file_two")
print

#Fill file_three with contents and print contents
print("file_three contents\n")
tenRandChar("file_three")
printFile("file_three")
print

#Generate two random integers
randInt1 = random.randint(1,42)
randInt2 = random.randint(1,42)

#Print the random integers
print("randInt1 = ", randInt1)
print("randInt2 = ", randInt2)

#Solve for and print the product of the random integers
product = randInt1 * randInt2
print("Product  = ", product)
