
# Global state variables
counter = 0


def handleRequest(path="", params={}):
  # Declare globals
  global counter

  # Do things
  counter += 1

  return ('Result string #%d\n\n' +
          'Path: %s\n' +
          'Params: %s\n') % (counter, path, params)

