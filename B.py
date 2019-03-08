'''file = open("C:\Users\Andrey\Documents\CPP_TP\TP-CPP-exB\TP-CPP-exB\input.txt", "r")
# states: val=0, var=1
state=val

for line in file:
    '''
opers = ["and", "or", "xor", "not", "(", ")"]
opersDict = {"?":-1, "*": 2, "+":1, "^":2, "~":3, "(":0, ")":5}
X = ["True", "False"]
reservedWords = opers + X
def eatChars(index, maxIndex, s, food):
    while index < maxIndex and s[index] == ' ': index = index + 1
    return index
def parseManifestation(s, indexEq):
    name = ""
    val = 0
    mode = 0
    j=0
    j = eatChars(j, indexEq, s, ' ')
    while j < indexEq and s[j].isalpha()==True:
        name = name + s[j]
        j = j + 1        
    if len(name) == 0:
        print("Error! Zero-length var name")
        return 0
    if name in reservedWords:
        print("Error! var name is a reserved word!")
        return 0
    j = eatChars(j, indexEq, s, ' ')
    if j == indexEq:
        j = j + 1 #пропускаем =
    else:
        print("Error! Wrong var name!")
        return 0
    j = eatChars(j, len(s), s, ' ')
    valName = ""
    while j < len(s) and s[j].isalpha() == True:
        valName = valName + s[j]
        j = j + 1
    if valName == X[0]: val = 1
    elif valName == X[1]: val = 0
    else:
        print("Error! Wrong var value: "+valName)
        return 0
    j = eatChars(j, len(s), s, ' ')
    if j<len(s) and s[j] == ";":
        return (name, val)
    else:
        print("Error! Wrong format")
        return 0
def parseExpression(st, varNames, varValues, varCount):
    st = st + " "
    stack = ['?']
    output = []
    val = ''
    fl=1    
    j = 0
    j = eatChars(j, len(s), s, ' ')
    for i in st:
        if i.isalpha():
            val = val + i
            fl = 1
        else:
            fl=0
            if val!='':
                #что за слово?
                if val in varNames:
                    index = varNames.index(val)
                    output.append(str(varValues[index]))
                elif val == "True": output.append('1')
                elif val=="False": output.append('0')
                elif val in opers:
                    temp = stack.pop()
                    if val=="and": t = '*'
                    elif val=="or": t = '+'
                    elif val=="xor": t = '^'
                    elif val=="not": t = '~'
                    else:
                        print("Error! Unknown operator")
                        return 0
                    while opersDict[t] <= opersDict[temp]:
                        output.append(temp)
                        temp = stack.pop()
                    stack.append(temp)
                    stack.append(t)                     
                else:
                    print("Error! Unknown word: " + val)
                    return 0
                val = ''
                
            #на какой символ наткнулись?
            if i==' ': pass
            elif i=='(':
                stack.append('(')
            elif i==')':
                temp = stack.pop()
                while temp!='(':
                    output.append(temp)
                    temp = stack.pop()                    
            else:
                print("Error! Unknown symbol: "+i)
                
    '''if fl == 1:
        output.append(int(val))'''
    while len(stack)>1:
        temp = stack.pop()
        if temp == '(' or temp == ')':
            print("Error! Wrong bracket succession!")
            return 0
        output.append(temp) 
    return output

def calculateExpression(s):
    stack = []
    for i in s:
        if i.isdigit():
            stack.append(i)
        else:
            if len(stack)==0:
                print("Error! Wrong expression!")
                return -1
            a = int(stack.pop())
            if i=='~':
                stack.append(str(int(not a)))
            else:
                if len(stack)==0:
                    print("Error! Wrong expression!")
                    return -1
                b = int(stack.pop())
                if i=='+':
                    stack.append(str(a or b))
                elif i=='*':
                    stack.append(str(a and b))
                elif i=='^':
                    stack.append(str(a^b))
                else:
                    print("Error! Unknown operator")
                    return -1
    t = stack.pop()
    if len(stack) != 0 or t.isdigit()!=True:
        print("Error! Invalid expression!")
        return -1
    return t
        
#s = "12+56*2-6/2="
s = [" ab   =  False   ;", "iscat=True;", "isdog=False;", "iscat = False;"," not ab or (False and True) and True", "(not iscat and isdog) or True"]
varNames = []
varValues = []
varCount = 0
for i in s:
    indexEq = -1
    for j in range(len(i)):
        if i[j] == "=": indexEq = j
    if indexEq >= 0:    #если строка верна, в ней - определение переменной
        ans = parseManifestation(i, indexEq)
        if ans != 0:
            if ans[0] in varNames:
                ind = varNames.index(ans[0])
                if ind >= 0:
                    varValues[ind] = ans[1]
            else:
                varNames.append(ans[0])
                varValues.append(ans[1])
                varCount = varCount + 1
            print(ans)
        else:
            print("Error during ")
    else:               #строка с выражением, либо ошибочная
        '''for j in range(varCount):
            i = i.replace(varNames[j], str(varValues[j]))
        print(i)'''
        ans = parseExpression(i, varNames, varValues, varCount)
        if ans==0:
            print("Error during parsing an expression!")
            exit(1)
        print(ans)
        #parseExpression() не обрабатывает логические ошибки. Это отсеивает вычислитель
        ans = calculateExpression(ans)
        if ans < 0 :
            print("Error! Cannot calculate an expression")
            exit(1)
        print("ANS= ", str(ans))
print(varNames)
print(varValues)

#s = "(1 + (2 + (3 + 8*2)))"
oper = {"?":-1,"+":1, "-":1, "*":2, "/":2, "(":0, ")":4}
stack = ['?']
output = []
val = ''
fl=1
# +*-
for st in s:
    for i in st:
        if True:
            if i.isdigit()==True:
                val = val + i
                fl = 1
            elif i in oper:
                if True:
                    fl=0
                    if val != '':
                        output.append(int(val))
                    val = ''
                    if i=='(':
                        stack.append(i)
                    elif i==')':
                        temp = stack.pop()
                        while temp!='(':
                            output.append(temp)
                            temp = stack.pop()
                    else:
                        temp = stack.pop()
                        while oper[i] <= oper[temp]:
                            output.append(temp)
                            temp = stack.pop()
                        stack.append(temp)
                        stack.append(i)
            
    if fl == 1:
        output.append(int(val))
    while len(stack)>1:
        output.append(stack.pop())
    print(output)
    print(stack)
            
            
    
