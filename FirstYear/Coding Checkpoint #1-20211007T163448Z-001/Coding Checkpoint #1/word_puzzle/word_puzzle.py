# -*- coding: utf-8 -*-
"""word_puzzle.ipynb

Automatically generated by Colaboratory.

Original file is located at
    https://colab.research.google.com/drive/1Jmixg9EA2jy6NMPefzsvMqAtf0XqX7D0
"""

# Project #1, Anmol Ratol #1587571
# program for a hangman-esque word game

import random as rand

# opening up ins as instructions.txt in read mode
ins = open("/content/drive/MyDrive/Fall 2021/word_puzzle/wp_instructions.txt", mode = 'r')

# creating a string equivalent to what is in the opened text file 
message = ins.read()

#closing said file
ins.close()

# printing the instructions
print(message)

# list of words to use for the game
word_list = ['apple', 'banana', 'watermelon', 'kiwi', 'pineapple', 'mango']

# variables for the word for a specific game, a word of equal length made up of just dashes, and the number of wrong letters the player has guessed 
selected_word = ''
blank_array = ''
wrong_guess_count = 0



# function to randomly pick an element from a list
def select_elem(list1, emp_list):
  elem = rand.randint(0, len(list1)-1)
  emp_list = list1[elem]
  return emp_list



# function to make a word of n length of just dashes
def make_dashed(word, blank):
  blank = ''
  for i in range(0, len(word)):
    blank = blank + ('-')
  return blank



# function for checking if entered letter matches the one in the array
def check_letter(letter, word, dash):
  # creating a placeholder array 
  placeholder = []
  # check all the letters and if the guessed letter is correct, change the dash to the guessed letter
  for i in range(0, len(word)):
    placeholder.append(dash[i])
    if letter == word[i]:
        placeholder[i] = (word[i])
  dash = ''
  # reconstruct the dashes with the new letters
  for i in range(0, len(word)):
    dash = dash + placeholder[i]
  return dash



# picking a word and creating it's dashed word of equivalent length 
selected_word = select_elem(word_list, selected_word)
blank_array = make_dashed(selected_word, blank_array)

# variable to keep the loop going
loop = True



while loop == True:


  # defining a variable for what the guesses from the previous returns look like 
  prev_blank = blank_array


  # player guesses a letter
  print("The answer so far is " + blank_array+".")
  guess = input("Guess a letter (" + str(4-wrong_guess_count)+ " guesses remaining): ")


  # checking if letter fits in the word
  blank_array = check_letter(guess, selected_word, blank_array)


  # if nothing changes, the player must have guessed incorrectly and the wrong guess count must be updated
  if blank_array == prev_blank:
    wrong_guess_count = wrong_guess_count + 1


  # if the player has guessed all the letters in the word, the player has won
  if blank_array == selected_word:
    print("Good job! You found the word "+ selected_word + "!")
    enter = input("Press enter to end the game. ")
    if enter == "":
      loop = False
    else:
      loop = False


  # if the player has had 4 wrong guesses, the player has lost
  if wrong_guess_count == 4:
    print("Not quite. The correct word was "+selected_word+". Better luck next time.")
    enter = input("Press enter to end the game. ")
    if enter == "":
      loop = False
    else:
      loop = False