import re

class Parser:

    start = "^[ \t]*TESTIDS_START"
    end = "^[ \t]*TESTIDS_END"
    keyValue = '^[ \t]*(?P<key>[A-Za-z0-9_]+) *: *"(?P<value>[A-Za-z0-9_]+)",'

    def __init__ (self):
        self.parsing = False
        pass

    def parseLine (self, line, idMap):

        if re.search (self.start, line):
            self.parsing = True

        elif re.search (self.end, line):
            self.parsing = False

        elif self.parsing:
            m = re.search (self.keyValue, line)
            key = m.group ("key")
            value = m.group ("value")
            idMap[key] = value
            print "    testIds ['{0}'] = {1}".format (key, value)

    def parseFile (self, filename):

        self.parsing = False

        #print "def getTestIds():\n"
        #print "    testIds = dict()\n"
        
        self.testIDs = dict()
        for l in open (filename):

            self.parseLine (l, self.testIDs)
        print ("\n    return testIds")
        return self.testIDs

if __name__ == "__main__":

    import os
    import sys

    if len(sys.argv) < 2:
        print "*** usage: python parseTestIds.py FILENAME"
        exit (1)

    filename = sys.argv[1]
    if not os.path.exists (filename):
        print "*** No such file: ", filename
        exit (1)

    p=Parser()
    p.parseFile (filename)

            
        



