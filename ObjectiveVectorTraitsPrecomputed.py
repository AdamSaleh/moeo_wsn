# -*- coding: utf-8 -*-
import os, shutil, errno,math,sqlite3,csv
import logging
logging.basicConfig(filename='evolve.log',level=logging.DEBUG)

def minimizing(i):
	return True

def maximizing(i):
	return False

def nObjectives():
	return 3
 
def minimumBounds(i):
	if i == 0:
		return 0
	return 0.01
def maximumBounds(i):
	if i == 0:
		return 54
	return 0.99
 
def nParams():
	return 2

def get_csv_database(filename):
  db = sqlite3.connect(":memory:")
  cur = db.cursor()
  cur.execute('CREATE TABLE IF NOT EXISTS precomputed (p1 REAL,p4 REAL,fp REAL,fn REAL,memory REAL)')
  reader = csv.reader(open(filename, "rb"))
  for x,p2,p3,p1,p4,fp,fn,tp,tn,Accuracy,Memory,Fitness in reader:
    cur.execute('INSERT OR IGNORE INTO precomputed (p1, p4,fp,fn,memory) VALUES (?,?,?,?,?)', (int(p1), float(p4), int(fp),int(fn),float(Memory)))
  return db

db = get_csv_database("Exhaustive_search.csv")

def dominates(test,tested):
  for i in range(0,len(test)):
    if maximizing(i):
      if test[i] < tested[i]:
	return False
    else:
      if test[i] > tested[i]:
	return False      
  return True

def get_nondominated(filename):
  reader = csv.reader(open(filename, "rb"))
  nondominated = set()
  for x,p2,p3,p1,p4,fp,fn,tp,tn,Accuracy,Memory,Fitness in reader:
    nw = (int(fp),int(fn),float(Memory))
    dominated = False 
    for test in nondominated:
      if dominates(test,nw):
	dominated = True

    if dominated == False:
      toremove = set()
      for tested in nondominated:
	if dominates(nw,test):
	  toremove.add(test) 
      nondominated.difference_update(toremove)
      nondominated.add(nw)
  return nondominated

entry_set = set()

def evaluate(p1,p4):
	p1 = math.floor(p1)
	p4 = math.floor(p4*100)/100
	entry_set.add((p1,p4))
	if p4 > 0.99:
	    p4 = 0.99
        cur = db.cursor()
	logging.info("Genome: "+ str(p1) +","+str(p4))
	cur.execute("Select fp,fn,memory from precomputed where p1=? and p4=?", (p1,p4))
	result = cur.fetchall();
	if len(result)>0:
	  logging.info("Result: "+ str(result[0]))  
	logging.info("Entry-set: "+ str(len(entry_set)))  
	return result[0];
