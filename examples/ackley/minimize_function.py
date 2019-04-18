#!/usr/bin/env python
import sys
sys.path.append('/home/martiser/skorali/korali/lib')
import libkorali as korali

p = korali.Engine()
p["Pep"]["ParameterString"] = "String";
p["Pep"]["ParameterDouble"] = 3.5;
p["Pep"]["ParameterInt"] = 3;
p["Pep"]["ParameterBool"] = True;
p["ParameterString"] = "String";
p["ParameterDouble"] = 3.5;
p["ParameterInt"] = 3;
p["ParameterBool"] = True;