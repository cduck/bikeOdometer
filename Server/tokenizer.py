# coding: utf-8
import string

queries = [
    'Did my average speed exceed 5 m/s?', #F(average > 5)
    'Did I bike more than 500 meters this week?', #F(tot_dist > 500)
    'Did I always stay above an altitude of 50 meters?', #G(attitude > 50)
    'Did my speed ever exceed 10 m/s?', #F(attitude > 50)
    'Was I always faster than 10 m/s?', #G(speed > 10)
    'Did my speed exceed 5 m/s and my altitude always stayed above 50 m?', # F(speed > 5) && G(altitude > 50)
    'Did I bike more than 250 meters and go faster than 5 m/s?', # F(speed > 5) && F(distance > 50)
    'Did I go slower than 5 m/s or bike more than 251 m?' # F(speed > 5) && G(altitude > 50)
]


triggerPhrases = [
    ['did i', 'do i'],
    ['how many', 'how much']
]
# Should contain all //recognizable// words. 
synonyms = [
    ['maintain'], #GF or FG?
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
    ['mph', 'miles per hour'],
    ['meters per second','m/s','ms'],
    ['second'],
    ['minute'],
    ['hour'],
    ['meter','m'],
    ['foot'],
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
    'average','attitude','bike'
]
connectingWords = [
    'of','and','or'
]
operatorWords = [ # Order matters in this array
    'maintain', 'after', 'until', 'greater', 'less', 'did','stay'
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

class TokenGroup:
    '''A group of tokens that mean somthing as a group'''
    # Group types
    UNKOWN = 100
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
    def __str__(self):
        typeStr = ['UNKNOWN', 'VAL', 'EXPRESSION'][self.type-100]
        if self.type == TokenGroup.UNKNOWN:
            return '<TokenGroup UNKNOWN>'
        if self.type == TokenGroup.VALUE:
            return '<VAL %s %s>' 
        if self.type == TokenGroup.BOOL_EXPR:
            return '<EXPR>'
        if self.type == TokenGroup.REAL_EXPR:
            return '<EXPR>'
            #return '<%s, %s>' % (repr(self.word), typeStr)
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
        elif token.type == UNIT:
            splitToks['UNIT'] = token.word
        elif token.type == VARIABLE:
            splitToks['VARIABLE'] = token.word
        elif token.type == MODIFIER:
            if 'MODIFIER' in splitToks:
                splitToks['MODIFIER'].append(token.word)
            else:
                splitToks['MODIFIER'] = [token.word]
        elif token.type == NUMBER:
            splitToks['NUMBER'] = token.word
        else:
            pass
#             print token.type
#             print 'Error'
    return splitToks

# Determine from tokens which variable you are interested in
def determineVariable(splitToks):
    if 'VARIABLE' in splitToks: #easy way out
        return splitToks['VARIABLE']
    else:
        variable = ''
        if 'MODIFIER' in splitToks:
            if splitToks['MODIFIER'] == 'average':
                variable = 'average'
        if 'UNIT' in splitToks:
            if splitToks['UNIT'] == 'meters per second':
                variable = variable + ' speed'
            elif splitToks['UNIT'] == 'meter':
                variable = variable + 'distance'
        return variable

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
def queryToSTL(query):
    #Assume only one variable per query        
    tokens = tokenizeQuery(query)
    splitToks = splitTokensByType(tokens)
    numOperators = len(splitToks['OPERATOR'])
    for i in xrange(numOperators):        
        min_power = 100
        min_operator = ''
        variable = determineVariable(splitToks)
        #Choose the most bad ass operator
        for j in xrange(len(splitToks['OPERATOR'])):            
            operator = splitToks['OPERATOR'][j]
            power = operatorWords.index(operator)
            if power < min_power:
                min_power = power
                min_operator = operator
        #Apply the bad ass operator
        if variable == '':
            if min_operator == 'did':
                STL = ('F',STL)
            elif min_operator == 'stay':
                STL = ('G',STL)            
        else:
            if min_operator == 'greater':
                STL = ('>',variable,splitToks['NUMBER'])
            elif min_operator == 'less':
                STL = ('<',variable,splitToks['NUMBER'])
        rem_tokens = []
        if variable != '':            
            for token in tokens:
                if token.word == variable:
                    pass
                elif token.word == splitToks['NUMBER']:
                    pass
                elif token.word == splitToks['UNIT']:
                    pass
                elif token.word == min_operator:
                    pass
                else:
                    rem_tokens.append(token)
        else:
            if token.word == min_operator:
                pass
            else:
                rem_tokens.append(token)
        splitToks = splitTokensByType(rem_tokens)
    return STL

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
    print result[2]
    STL = '' 
    for i in xrange(numQueries):
        partialSTL = queryToSTL(result[2][i])
        if i > 0:
            STL = (operator,partialSTL,STL)
        else:
            STL = partialSTL       
    return STL

