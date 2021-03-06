#!/usr/bin/env python

import sys, BaseHTTPServer, urlparse, socket, posixpath, urllib, json
import serverLogic


class MyHttpHandler(BaseHTTPServer.BaseHTTPRequestHandler):
  def do_GET(self):
    self.do_GETPOST()
  def do_POST(self):
    self.do_GETPOST()
  def do_GETPOST(self):
    parsedPath = urlparse.urlparse(self.path)
    parsedPathComponents = self.pathParse(parsedPath.path)
    parsedQueries = urlparse.parse_qs(parsedPath.query, keep_blank_values=True)
    parsedQueries = {k:v[-1] for k,v in parsedQueries.iteritems()}
    print ' * REQUEST:', parsedPathComponents, parsedQueries

    if len(parsedPathComponents) == 1 and parsedPathComponents[0] == 'favicon.ico':
      self.send_response(404)
      self.end_headers()
      return

    message = serverLogic.handleRequest(path=parsedPathComponents, params=parsedQueries)
    response = json.dumps({'path': parsedPathComponents,
                           'query': parsedQueries,
                           'result': message})
    print ' * RESPONSE:', response
    self.send_response(200)
    self.end_headers()
    self.wfile.write(response)
    return
  def pathParse(self, rawPathStr, normalize=True, module=posixpath):
    result = []
    if normalize:
      tmp = module.normpath(rawPathStr)
    else:
      tmp = rawPathStr
    while tmp != '/':
      tmp, item = module.split(tmp)
      result.insert(0, urllib.unquote(item))
    return result


def runServer(port, verbose=False):
  ip = socket.gethostbyname(socket.getfqdn())
  print 'Serving at IP {}, port {}'.format(ip, port)
  print 'Communicate with this server by entering "http://{}:{}/some/path?a=query1&xyz=789" in a browser.'.format(ip, port)
  print 'Communicate with this server by entering "http://{}:{}/stl?question=Did%20my%20average%20speed%20exceed%2010%20m%2Fs&i=45" in a browser.'.format(ip, port)
  sys.stdout.flush()

  server_address = ('', port)
  httpd = BaseHTTPServer.HTTPServer(server_address, MyHttpHandler)
  while True:
    httpd.handle_request()

if __name__ == '__main__':
  import argparse
  parser = argparse.ArgumentParser()
  parser.add_argument('-v', action='store_true', help='Verbose')
  parser.add_argument('-p', type=int, default=8080, help='Port number')
  args = parser.parse_args()

  runServer(args.p, verbose=args.v)

