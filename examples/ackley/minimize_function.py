#!/usr/bin/env python
import sys
sys.path.append('/home/martiser/skorali/korali/lib')
import libkorali as korali

p = korali.Pet('Molly')
print(p)
print(p.getName())
p.setName('Charly')
print(p.getName())
