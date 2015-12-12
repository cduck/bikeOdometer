# coding: utf-8
import string

queries = [
    'Did my average speed exceed 5 m/s?'
]

triggerPhrases = [
    ['did i', 'do i'],
    ['how many', 'how much']
]
# Should contain all //recognizable// words. 
synonyms = [
    ['maintain'],
    ['after'],
    ['until'],
    ['greater'],
    ['less'],
    ['speed', 'velocity'],
    ['distance'],
    ['incline', 'hill'],
    ['altitude', 'elevation', 'height'],
    ['degree', 'º', '°', 'deg'],
    ['mph', 'miles per hour'],
    ['meters per second','m/s','ms'],
    ['second'],
    ['minute'],
    ['hour'],
    ['meter'],
    ['foot'],
    ['of'],
    ['did']
]
unitWords = [  # Omit s if can be plural
    'degree', 'mph', 'second', 'minute', 'hour', 'meter', 'foot', 'meters per second'
]
connectingWords = [
    'of'
]
operatorWords = [
    'maintain', 'after', 'until', 'greater', 'less', 'did'
]
variableWords = [
    'speed', 'altitude', 'distance', 'incline'
]
operatorForms = {
    'maintain': [
        ['maintain', 'BOOL_EXPR', 'VALUE']
        ###['maintain', 'BOOL_EXPR', 'until']
    ],
    'after': [
        ['BOOL_EXPR', 'after', 'BOOL_EXPR']
    ],
    #'until': []
    'greater': [
        ['REAL_EXPR', 'greater', 'REAL_EXPR']
    ],
    'less': [
        ['REAL_EXPR', 'less', 'REAL_EXPR']
    ]
    # ...
}

#######TOKENIZER CODE##################################################################

# Create synonymDict mapping all synonyms to their canonical word
synonymDict = {}
for group in synonyms:
    standardWord = group[0]
    for word in group:
        synonymDict[word] = standardWord

all = string.maketrans('','')
noDigs = all.translate(all, string.digits)
noLetters = all.translate(all, string.letters)
allowedLetters = ' abcdefghijklmnopqrstuvwxyz0123456789%.º°'
notAllowed = all.translate(all, allowedLetters)

def cleanQuery(query):
    return query.lower().translate(all, notAllowed)

# Attempts to return the canonical words in your query
def wordToStandard(word):
    if word is None:
        return None
    word = word.lower()  # Lower case
    if len(word) == 0:
        return None
    if word[0] in '-0123456789':
        #TODO: Maybe more processing of numbers
        return word
    if word in synonymDict:
        return synonymDict[word]
    word = word.translate(all, noLetters)  # Keep only letters
    if word in synonymDict:
        return synonymDict[word]
    if word[-1] == 's':
        word = word[:-1]
        if word in synonymDict:
            return synonymDict[word]
    return None

class Token:
    '''A word associated with its meaning'''
    # Token types
    UNKNOWN = 0
    NONE = 1
    OPERATOR = 2
    CONNECTOR = 3
    NUMBER = 4
    UNIT = 5
    VARIABLE = 6
    def __init__(self, word, left=None, right=None):
        self.origWord = word
        self.word = wordToStandard(word)
        self.left = wordToStandard(left)
        self.right = wordToStandard(right)
        if self.word in operatorWords:
            self.type = Token.OPERATOR
        elif self.word in variableWords:
            self.type = Token.VARIABLE
        elif self.word in unitWords:
            self.type = Token.UNIT
        elif self.word in connectingWords:
            self.type = Token.CONNECTOR
        elif len(self.word) > 0 and self.word[0] in '-0123456789':
            self.type = Token.NUMBER
        else:
            self.type = Token.NONE
            if self.word is not None:
                print repr(self.origWord), repr(self.word), 'not understood.'
    def __str__(self):
        typeStr = ['UNKNOWN', 'NONE', 'OPERATOR', 'CONNECTOR',
                   'NUMBER', 'UNIT', 'VARIABLE'][self.type]
        return '<%s, %s>' % (self.word, typeStr)
    def __repr__(self):
        return self.__str__()

# This gives you the recognized canonical words
def splitQuery(query):
    query = cleanQuery(query)
    l = query.split()
    tokens = []
    for word in l:
        standard = wordToStandard(word)
        if standard is not None:
            tokens.append(standard)
    return tokens

splitQuery(queries[0])

# This will tell you what type each splitted query is
def tokenizeQuery(query):
    split = splitQuery(query)
    tokens = []
    for i in xrange(len(split)):
        left = None if i-1 < 0 else split[i-1]
        right = None if i+1 >= len(split) else split[i+1]
        t = Token(split[i], left, right)
        if t.type != Token.UNKNOWN and t.type != Token.NONE:
            tokens.append(t)
    return tokens




