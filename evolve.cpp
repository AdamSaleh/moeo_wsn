/*
* <Sch1.cpp>
* Copyright (C) DOLPHIN Project-Team, INRIA Futurs, 2006-2007
* (C) OPAC Team, LIFL, 2002-2007
*
* Abdelhakim Deneche, Arnaud Liefooghe
*
* This software is governed by the CeCILL license under French law and
* abiding by the rules of distribution of free software.  You can  use,
* modify and/ or redistribute the software under the terms of the CeCILL
* license as circulated by CEA, CNRS and INRIA at the following URL
* "http://www.cecill.info".
*
* As a counterpart to the access to the source code and  rights to copy,
* modify and redistribute granted by the license, users are provided only
* with a limited warranty  and the software's author,  the holder of the
* economic rights,  and the successive licensors  have only  limited liability.
*
* In this respect, the user's attention is drawn to the risks associated
* with loading,  using,  modifying and/or developing or reproducing the
* software by the user in light of its specific status of free software,
* that may mean  that it is complicated to manipulate,  and  that  also
* therefore means  that it is reserved for developers  and  experienced
* professionals having in-depth computer knowledge. Users are therefore
* encouraged to load and test the software's suitability as regards their
* requirements in conditions enabling the security of their systems and/or
* data to be ensured and,  more generally, to use and operate it in the
* same conditions as regards security.
* The fact that you are presently reading this means that you have had
* knowledge of the CeCILL license and that you accept its terms.
*
* ParadisEO WebSite : http://paradiseo.gforge.inria.fr
* Contact: paradiseo-help@lists.gforge.inria.fr
*
*/
//-----------------------------------------------------------------------------

#include <Python.h>
#include <stdio.h>
#include <sstream>
#include <moeo>
#include <es/eoRealInitBounded.h>
#include <es/eoRealOp.h>

using namespace std;

PyObject *pModule, *pFuncMin, *pFuncMax,*pFuncNo,
*pFuncMinBounds, *pFuncMaxBounds,*pFuncNoParams,*pFuncEval;

bool py_bool_function(PyObject *fn,int index){
    bool out = false;	
    PyObject *pArgs, *pValue;
    if (fn && PyCallable_Check(fn)) {
        pArgs = PyTuple_New(1);
	pValue = PyInt_FromLong(index);
	PyTuple_SetItem(pArgs, 0, pValue);
	pValue = PyObject_CallObject(fn, pArgs);
	Py_DECREF(pArgs);
	if (pValue != NULL) {
		out= (Py_True == pValue);
	}
	else {
		PyErr_Print();
	}
     }
     Py_DECREF(pValue);

     return out;
}

int py_int_function(PyObject *fn)
{
    long out = 0;	
        PyObject *pArgs, *pValue;
	if (fn && PyCallable_Check(fn)) {
		pValue = PyObject_CallObject(fn, NULL);
		if (pValue != NULL) {
			out= PyInt_AsLong(pValue);
		}
		else {
			PyErr_Print();
		}
	}
	Py_DECREF(pValue);
    
        return out;
}

double py_double_function(PyObject *fn,int index){
    double out = 0;	
    PyObject *pArgs, *pValue;
    if (fn && PyCallable_Check(fn)) {
	pArgs = PyTuple_New(1);
	pValue = PyInt_FromLong(index);
	PyTuple_SetItem(pArgs, 0, pValue);
	pValue = PyObject_CallObject(fn, pArgs);
	Py_DECREF(pArgs);
	if (pValue != NULL) {
		out= PyFloat_AsDouble(pValue);
	}
	else {
		PyErr_Print();
	}
    }
    Py_DECREF(pValue);
    return out;
}

bool py_minimizing (int index)
{
    return py_bool_function(pFuncMin,index);
}

bool py_maximizing (int index)
{
    return py_bool_function(pFuncMax,index);
}

int py_nObjectives ()
{
    return py_int_function(pFuncNo); 
}

double py_minimalBounds(int index)
{
    return py_double_function(pFuncMinBounds,index); 
}

double py_maximalBounds(int index)
{
    return py_double_function(pFuncMaxBounds,index);
}

int py_noParams ()
{
    return py_int_function(pFuncNoParams); 
}

// the moeoObjectiveVectorTraits : minimizing 2 objectives
class OVT : public moeoObjectiveVectorTraits
{
public:
 static bool minimizing (int index)
    {						
        bool out =	py_minimizing(index);
        return out;
    }
    static bool maximizing (int index)
    {
		bool out =	py_maximizing(index);
        return out;
       
    }
    static unsigned int nObjectives ()
    {

		int out =	py_nObjectives();
        return out;
    }
};


// objective vector of real values
typedef moeoRealObjectiveVector < OVT > EvolveObjectiveVector;


// multi-objective evolving object for the Evolve problem
class Evolve : public moeoRealVector < EvolveObjectiveVector >
{
public:
    Evolve() : moeoRealVector < EvolveObjectiveVector > (1)
    {}
};

std::vector<double> py_eval (std::vector<double> params)
    {

    std::vector<double>  out(py_nObjectives());	
        PyObject *pArgs, *pValue,*pReturnTuple;
	if (pFuncMax && PyCallable_Check(pFuncMax)) {
		pArgs = PyTuple_New(params.size());
                for(int i=0;i<params.size();i++){
		  pValue = PyFloat_FromDouble(params[i]);
		  PyTuple_SetItem(pArgs, i, pValue);
		}
		pReturnTuple = PyObject_CallObject(pFuncEval, pArgs);
		Py_DECREF(pArgs);
		if (pReturnTuple != NULL) {

		   for(int i=0;i<py_nObjectives();i++){
		     pValue = PyTuple_GetItem(pReturnTuple,i);	
		     double result_i = PyFloat_AsDouble(pValue);
		     out[i]=result_i;
		   }

		}
		else {
			PyErr_Print();
		}
	}
	
	Py_DECREF(pValue);
	
	Py_DECREF(pReturnTuple);

        return out;
    }

/*

1) generovat config-jedinec
2) ohodnotit


*/


// evaluation of objective functions
class EvolveEval : public moeoEvalFunc < Evolve >
{
public:
    void operator () (Evolve & _sch1)
    {
        if (_sch1.invalidObjectiveVector())
        {

	    EvolveObjectiveVector objVec;

	    std::vector<double> params(py_noParams());
	   for(int i=0;i<py_noParams();i++){
	      params[i]=_sch1[i];
	    }

	    std::vector<double> objectives =py_eval(params);

	    for(int i=0;i<objectives.size();i++){
	      objVec[i] = objectives[i];
	    }
	    
	    _sch1.objectiveVector(objVec);
        }
    }
};

// main
int main (int argc, char *argv[])
{
    eoParser parser(argc, argv);  // for user-parameter reading
    eoState state;                // to keep all things allocated

    // parameters
    unsigned int POP_SIZE = parser.createParam((unsigned int)(100), "popSize", "Population size",'P',"Param").value();
    unsigned int MAX_GEN = parser.createParam((unsigned int)(100), "maxGen", "Maximum number of generations",'G',"Param").value();
    double M_EPSILON = parser.createParam(0.01, "mutEpsilon", "epsilon for mutation",'e',"Param").value();
    double P_CROSS = parser.createParam(0.25, "pCross", "Crossover probability",'C',"Param").value();
    double P_MUT = parser.createParam(0.35, "pMut", "Mutation probability",'M',"Param").value();
    string P_PATH = parser.createParam(string("/tmp/evolve"),"pPath","Python path",'t').value(); 
    string P_MODULE = parser.createParam(string("ObjectiveVectorTraits"),"pModule","Python module",'m').value(); 
    string P_ALGO = parser.createParam(string("nsgaII"),"pAlgo","Evolution algorithm",'a').value(); 
   cout << P_PATH << P_MODULE; 
    Py_Initialize();              // will use python for glue
    // Add PROJECT to PATH
    stringstream path; 
    cout << "Archive" << endl;
    //path << "/home/asaleh/Diplomka/moeo_wsn/evolve";
    path << P_PATH;
    stringstream pyrun;
    pyrun << "import os, sys, threading\nsys.path.append('" << path.str() <<  "')\nos.chdir('" << path.str() << "')\n";
    PyRun_SimpleString(pyrun.str().c_str() ); 
    cout << pyrun.str() << endl;
	  
   if (pModule == NULL) {        
		PyObject *pName;
        	pName = PyString_FromString(P_MODULE.c_str());
        	/* Error checking of pName left out */
        	pModule = PyImport_Import(pName);
    cout << "Archive" << endl;
		Py_DECREF(pName);
		if (pModule != NULL) {
			pFuncMin = PyObject_GetAttrString(pModule, "minimizing");
			pFuncMax = PyObject_GetAttrString(pModule, "maximizing");
			pFuncNo = PyObject_GetAttrString(pModule, "nObjectives");
			pFuncMinBounds = PyObject_GetAttrString(pModule, "minimumBounds");
			pFuncMaxBounds = PyObject_GetAttrString(pModule, "maximumBounds");
			pFuncNoParams = PyObject_GetAttrString(pModule, "nParams");
			pFuncEval = PyObject_GetAttrString(pModule, "evaluate");
		}else {
			PyErr_Print();
	    	}
	}

    // objective functions evaluation
    EvolveEval eval;

    // crossover and mutation
    eoQuadCloneOp < Evolve > xover;
    eoUniformMutation < Evolve > mutation (M_EPSILON);

    // generate initial population
    int noParams = py_noParams();
    std::vector<double> minBounds(noParams);
    std::vector<double> maxBounds(noParams);

    for(int i=0;i<noParams;i++){
      minBounds[i]=py_minimalBounds(i);
      maxBounds[i]=py_maximalBounds(i);
    }
    eoRealVectorBounds bounds (minBounds, maxBounds);

    eoRealInitBounded < Evolve> init (bounds);
    eoPop < Evolve > pop (POP_SIZE, init);

   // help ?
   make_help(parser);
   
   if(P_ALGO=="spea2"){
    	// build spea2 
    	moeoSPEA2Archive<Evolve> arch_spea(POP_SIZE);
    	moeoSPEA2<Evolve > spea2 (MAX_GEN, eval, xover, P_CROSS, mutation, P_MUT,arch_spea);

    	// run the algo
    	spea2 (pop);

    	// extract first front of the final population using an moeoArchive (this is the output of nsgaII)
    	moeoUnboundedArchive < Evolve > arch;
    	arch(pop);

    	// printing of the final archive
    	cout << "Final Spea2 Archive" << endl;
    	arch.sortedPrintOn (cout);
    	cout << endl;
    }else{
    	// build NSGA-II
    	moeoNSGAII<Evolve > nsgaII (MAX_GEN, eval, xover, P_CROSS, mutation, P_MUT);

    	// run the algo
    	nsgaII (pop);

    	// extract first front of the final population using an moeoArchive (this is the output of nsgaII)
    	moeoUnboundedArchive < Evolve > arch;
    	arch(pop);

    	// printing of the final archive
    	cout << "Final NSGA-II Archive" << endl;
    	arch.sortedPrintOn (cout);
    	cout << endl;
    }	
  Py_XDECREF(pFuncMin);
  Py_XDECREF(pFuncMax);
  Py_XDECREF(pFuncNo);
  Py_DECREF(pModule);
 Py_Finalize(); // finish python
    return EXIT_SUCCESS;
}
