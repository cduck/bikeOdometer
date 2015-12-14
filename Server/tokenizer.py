# coding: utf-8
import string

queries = [
    'Did my average speed exceed 2 m/s?', #F(average > 5)
    'Did I bike greater than 100 feet this week?', #F(tot_dist > 100)
    'Did I stay above an altitude of 50 meters?', #G(attitude > 50)
    'Did my altitude ever exceed 200 foot?', #F(attitude > 200)
    'Was I always faster than 10 m/s?', #G(speed > 10)
    'Did my speed exceed 5 m/s and my altitude always stayed above 50 m?', # F(speed > 5) && G(altitude > 50)
    'Did I bike more than 250 meters and go faster than 5 m/s on average?', # F(average > 5) && F(distance > 50)
    'Did I go slower than 5 m/s or bike more than 251 m?', # F(speed < 5) || F(distance > 251)
    'Did I maintain a speed of greater than 5 m/s over 1 minute?', # FG_(0,60) (speed > 5)
    'Did I keep an altitude of less than 20 meters for 1 hour?' # FG_(0,3600) (altitude > 20)
#     'After biking 500 meters, did I exceed speed of 15 m/s?'
#     'Did I exceed speed of 15 m/s after biking 500 meters?'
]


triggerPhrases = [
    ['did i', 'do i'],
    ['how many', 'how much']
]
# Should contain all //recognizable// words. 
synonyms = [
    ['maintain','keep'], #G_(t1,t2)
    ['stay','always'], #G
    ['did','ever'], #F
    ['greater','exceed','more','faster','above'], # >
    ['less','below','slower'], # <
    ['after'],
    ['until'],
    ['speed', 'velocity'],
    ['distance'],
    ['incline', 'hill'],
    ['altitude', 'elevation', 'height'],
    ['degree', 'º', '°', 'deg'],
    ['meters per second','m/s','ms','mph', 'miles per hour'],
    ['minute'],
    ['second'],
    ['hour'],
    ['meter','m'],
    ['foot','feet'],
    ['of'],
    ['average','mean'],
    ['bike','travel'],
    ['and','both'],
    ['or']
]
unitWords = [  # Omit s if can be plural
    'degree', 'mph', 'second', 'minute', 'hour', 'meter', 'foot', 'meters per second'
]
modifierWords = [  # Words that modify units
    'average','altitude','bike'
]
connectingWords = [
    'of','and','or'
]
operatorWords = [ # Order matters in this array
    'after', 'until', 'greater', 'less', 'maintain', 'stay','did'
]
variableWords = [
    'speed', 'altitude', 'distance', 'incline'
]
distanceUnits = [  # First unit is default supported
    'meter', 'foot'
]
timeUnits = [  
    'second', 'minute', 'hour'
]
speedUnits = [  
    'meters per second', 'mph'
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
    if word[-2:] == 'ed':
        word = word[:-2]
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
    MODIFIER = 7
    def __init__(self, word, left=None, right=None):
        self.origWord = word
        self.word = wordToStandard(word)
        self.left = wordToStandard(left)
        self.right = wordToStandard(right)
        if self.word in operatorWords:
            self.type = Token.OPERATOR
        elif self.word in modifierWords:
            self.type = Token.MODIFIER
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
                   'NUMBER', 'UNIT', 'VARIABLE','MODIFIER'][self.type]
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

# Determine if query has a F or G operator and returns them if they exist
def determineFGOperator(query):
    tokens = tokenizeQuery(query)
    splitToks = splitTokensByType(tokens)
    if 'OPERATOR' in splitToks:
        if 'did' in splitToks['OPERATOR']:
            return 'did'
        elif 'stay' in splitToks['OPERATOR']:
            return 'stay'
        else:
            return ''
    else:
        return ''
    
# Splits a query based on OR and AND, assuming only one of the two is being used
def divideQueries(queries):
    splitAnd = queries.split(' and ')
    splitOr = queries.split(' or ')
    andLength = len(splitAnd)
    orLength = len(splitOr)
    if andLength > orLength:
        return (andLength,'AND',splitAnd)
    elif orLength > andLength:
        return (orLength,'OR',splitOr)
    elif (orLength == 1 and andLength == 1):
        return (1,'',[queries])
    else:
        print 'Improperly formatted string'
        return ''  

def queriesToSTL(queries):
    result = divideQueries(queries)
    numQueries = result[0]
    operator = result[1]
    extraTok = ''
    for i in xrange(numQueries):
        token_word = determineFGOperator(result[2][i])
        if len(token_word) > 0 and not extraTok: # F or G operator found
            extraTok = token_word
            for j in xrange(1,numQueries):
                result[2][j] = token_word + ' ' + result[2][j]
    STL = '' 
    for i in xrange(numQueries):
        partialSTL = queryToSTL(result[2][i])
        if i > 0:
            STL = (operator,partialSTL,STL)
        else:
            STL = partialSTL       
    return STL
    
# This will tell you what type each splitted query is
def tokenizeQuery(query):
    split = splitQuery(query)
    tokens = []
    for i in xrange(len(split)):
        left = None if i-1 < 0 else split[i-1]
        right = None if i+1 >= len(split) else split[i+1]
        t = Token(split[i], left, right)
        if t.type != Token.UNKNOWN and t.type != Token.NONE:
            if t.type == Token.NUMBER:
                groupT = TokenGroup(TokenGroup.VALUE,[t, Token(split[i+1], left, right)])
                tokens.append(groupT)
            elif t.type == Token.UNIT:
                continue 
            else:
                tokens.append(t)
    return tokens

# This will take an array of tokens and then output a dictionary index by Token Types
def splitTokensByType(tokens):
    # Token types, copied from Token definition
    UNKNOWN = 0
    NONE = 1
    OPERATOR = 2
    CONNECTOR = 3
    NUMBER = 4
    UNIT = 5
    VARIABLE = 6
    MODIFIER = 7

    splitToks = {}
    #Separate out tokens by type
    for token in tokens:
        if token.type == OPERATOR:
            if 'OPERATOR' in splitToks and (token.word not in splitToks['OPERATOR']):
                splitToks['OPERATOR'].append(token.word)
            else:
                splitToks['OPERATOR'] = [token.word]
        elif token.type == TokenGroup.VALUE:
            if 'VALUE' in splitToks and (token.word not in splitToks['VALUE']):
                splitToks['VALUE'].append(token.word)
            else:
                splitToks['VALUE'] = [token.word]
        elif token.type == VARIABLE:
            splitToks['VARIABLE'] = token.word
        elif token.type == MODIFIER:
            if 'MODIFIER' in splitToks:
                splitToks['MODIFIER'].append(token.word)
            else:
                splitToks['MODIFIER'] = [token.word]
        else:
            pass
    return splitToks

def parseValue(val):
    parts = val.split(' ',1)
    num = parts[0]
    unit = parts[1]
    return num,unit
    
def queryToSTL(query):
    #Assume only one variable per query        
    tokens = tokenizeQuery(query)
    splitToks = splitTokensByType(tokens)
    tLim = determineInterval(splitToks)
    numOperators = len(splitToks['OPERATOR'])
    for i in xrange(numOperators):        
        min_power = 100
        min_operator = ''
        variable,qty = determineVariable(splitToks)
        #Choose the most bad ass operator
        for j in xrange(len(splitToks['OPERATOR'])):
            operator = splitToks['OPERATOR'][j]
            power = operatorWords.index(operator)
            if power < min_power:
                min_power = power
                min_operator = operator
        #Apply the bad ass operator
#         print 'var:  ',variable
#         print 'min_op:  ',min_operator
        if variable == '':
            if min_operator == 'did':
                STL = ('F',STL)
            elif min_operator == 'stay':
                STL = ('G',STL)
            elif min_operator == 'maintain':
                STL = ('G_(0,%s)'%tLim,STL)
        else:
            if min_operator == 'greater':
                STL = ('>',variable,qty)
            elif min_operator == 'less':
                STL = ('<',variable,qty)       
        rem_tokens = []
        if variable != '':            
            for token in tokens:
                if token.word in variable.split():
                    pass
                elif token.word in splitToks['VALUE']:
                    pass
                elif token.word == min_operator:
                    pass
                else:
                    rem_tokens.append(token)
        else:
            for token in tokens:
                if token.word == min_operator:
                    pass
                else:
                    rem_tokens.append(token)
        splitToks = splitTokensByType(rem_tokens)
        tokens = rem_tokens
    return STL

    
# Determine if query has time interval information
def determineInterval(splitToks):    
    if 'VALUE' in splitToks:
        for val in splitToks['VALUE']:
            num, unit = parseValue(val)
            if unit == timeUnits[0]:
                return num
            else:
                continue
        return 'tmax'
    else:
        return 'tmax'
    
# Determine from tokens which variable you are interested in and how much of it
def determineVariable(splitToks):
    variable = '',''
    if 'VALUE' in splitToks:
        for val in splitToks['VALUE']:
            num,unit = parseValue(val)
            if 'meters per second' == unit:
                if 'MODIFIER' in splitToks:
                    if splitToks['MODIFIER'] == ['average']:
                        variable = 'average speed',num
                else:
                    variable = 'speed',num
            elif 'meter'== unit:
                variable = 'distance',num
                if 'MODIFIER' in splitToks:
                    if splitToks['MODIFIER'] == ['altitude']:
                        variable = 'altitude',num
            if variable != ('',''):
                break
    return variable


class TokenGroup:
    '''A group of tokens that mean somthing as a group'''
    # Group types
    UNKNOWN = 100
    VALUE = 101  # Number with unit
    BOOL_EXPR = 102  # Logical or temporal
    REAL_EXPR = 103
    tokenPatterns = {
        VALUE: [
            [Token.NUMBER, Token.UNIT],
            [Token.NUMBER]
        ],
        BOOL_EXPR: [
            # Equality operator
            [Token.VARIABLE, Token.CONNECTOR, VALUE],
            [VALUE, Token.VARIABLE],
            []
        ]
    }
    def __init__(self, type, tokens):
        self.type = type
        self.tokens = tokens
        if tokens[1].origWord not in [distanceUnits[0], speedUnits[0], timeUnits[0]]:
            notSupportedUnit = tokens[1].origWord
            if notSupportedUnit in speedUnits:
                number = 'unsupported unit'
                unit = speedUnits[0]
                if notSupportedUnit == 'mph':
                    number = str(float(tokens[0].word)*0.44704)                                    
            elif notSupportedUnit in timeUnits:
                number = 'unsupported unit'
                unit = timeUnits[0]
                if notSupportedUnit == 'minute':
                    number = str(float(tokens[0].word)*60)
                if notSupportedUnit == 'hour':                    
                    number = str(float(tokens[0].word)*60*60)
            elif notSupportedUnit in distanceUnits:
                number = 'unsupported unit'
                unit = distanceUnits[0]
                if notSupportedUnit == 'foot':
                    number = str(float(tokens[0].word)*0.3048)                
            self.word = '%s %s'%(number,unit) 
        else:
            self.word = '%s %s'%(self.tokens[0].origWord,self.tokens[1].origWord) 
    def __str__(self):
        typeStr = ['UNKNOWN', 'VAL', 'EXPRESSION'][self.type-100]
        if self.type == TokenGroup.UNKNOWN:
            return '<TokenGroup UNKNOWN>'
        if self.type == TokenGroup.VALUE:
            return '<%s, VALUE>'%self.word 
        if self.type == TokenGroup.BOOL_EXPR:
            return '<EXPR>'
        if self.type == TokenGroup.REAL_EXPR:
            return '<EXPR>'
            #return '<%s, %s>' % (repr(self.word), typeStr)
    def __repr__(self):
        return self.__str__()


