
# Global state variables
counter = 0


def handleRequest(path="", params={}):
  # Declare globals
  global counter

  # Do things
  counter += 1

  return {'result': 'string #%d'%counter, 'info': 'abc', 'count': counter}

