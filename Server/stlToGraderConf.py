
# Input STL formula as a tree formed with tuples e.g.
# ('F', ('and', ('G', ('>', 'speed', 5)), ('G', ('<', 'incline', 3))))


template = """# signals measured by Tellaride
signal alt,inc,spe,dis

# Time horizon
param tmax = %s

# Formulas
%s

# Define the test
test formula_check: "%s"
{
  Result()
  {
    varTop,
    "THE_FORMULA_IS_TRUE",
    "THE_FORMULA_IS_FALSE",
    false
  }
}
"""

variables = ['alt', 'inc', 'spe', 'dis']  # first 3 letters of variables


def stlToGraderConf(stl, traceFileName):
  tmax = 60*60*24  # One day max trace length
  formulas = stlFormula(stl)
  return template % (tmax, formulas, traceFileName)

def stlFormula(stl):
  output, varName, nextVarIndex = stlFormulaHelp(stl, 0, 'varTop')
  return str(output)

def stlFormulaHelp(stl, varIndex, varName=None):
  # Base case: stl is a string or number
  if not hasattr(stl, '__iter__'):
    return '', stl, varIndex
  # Check that there is an operator
  assert len(stl) >= 1
  # Split into operator and args
  op, args = stl[0], list(stl[1:])
  # Recurse on each argument
  outputTotal = ''  # Keep track of all ouput lines so far
  for i in xrange(len(args)):
    output, varNameArg, nextVarIndex = stlFormulaHelp(args[i], varIndex)
    outputTotal += output  # Keep track of all ouput lines so far
    args[i] = varNameArg  # Replace the argument which could consist of more tuples with the variable name
    varIndex = nextVarIndex  # Keep track of next unused variable index
  # Pick a variable name to use that hasn't already been used
  varName = 'var%d'%varIndex if varName is None else varName
  varIndex += 1
  outputLine = formatLine(varName, op, args)
  outputTotal += outputLine
  return outputTotal, varName, varIndex


def formatLine(varName, op, argsFlat):
  return '{0} := {1}\n'.format(varName, formatOperator(op, argsFlat))

def formatOperator(op, args):
  # Normalize variable names and add [t] index
  op = op.strip().upper()
  args = map(lambda a: str(a).lower(), args)
  args = [a[:3]+'[t]' if a[:3] in variables else a for a in args]

  # Format string based on the operator
  if op == 'G':
    assert len(args) == 1, 'Bad args for {0} operator'.format(op)
    return 'alw_[0, tmax] {0}'.format(args[0])
  elif op == 'F':
    assert len(args) == 1, 'Bad args for {0} operator'.format(op)
    return 'ev_[0, tmax] {0}'.format(args[0])
  elif op == '>' or op == '<' or op == '<=' or op == '>=':
    assert len(args) == 2, 'Bad args for {0} operator'.format(op)
    return '{0} {1} {2}'.format(args[0], op, args[1])
  elif op == 'AND' or op == 'OR':
    op = op.lower()
    assert len(args) == 2, 'Bad args for {0} operator'.format(op)
    return '{0} {1} {2}'.format(args[0], op, args[1])
  else:
    assert False, 'Unkown operator {0}'.format(op)
    return None



if __name__ == '__main__':
  stl = ('F', ('and', ('G', ('>', 'speed', 5)), ('G', ('<', 'incline', 3))))
  print '# Sample output:'
  print
  print stlToGraderConf(stl, 'traceFile.txt')
  print
  print
  print 'Helper functions:'
  print formatOperator('>', ('speed',5))
  print formatLine('var1', '>', ('speed',5))

