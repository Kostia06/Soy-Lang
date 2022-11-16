# Soy-Lang
The Soy-Lang the interpreted programming language

|Programming Languages|JS|PY|Lua|SL|
|---|---|---|---|---|
|time/1million actios|2.56s|1.30s|0.37s|2.43s|

The langauge it self is really JavaScript syntax based langauge. The purpose of the langauge is to created small things for computer. For example, desktop cleaner, calculator and etc...
# Variable declaration
Variable can be any type
String, Integer, Array, Object, Action, Operator, Function, Argument, Module and etc...
```
obj Person{ name, age, country }  ~Object~

name = "Kostia" + ` ` + 'Ilnytkyi' ~String~

add = +  ~Action~

age = 10 add 6 ~Integer~

countries = ["Ukraine", `Malaysia` , 'United Stated'] add "Switerland" ~Array~

me = Person{
  name,
  age,
  countries
}
```
# Operators
|Operator|Types that work with|
|---|---|
|+= or +|Any|
|-= or -|Any|
|*= or *|String,Integer|
|/= or /|Integer|
|^= or ^|Integer|
|%= or %|Integer|
# Small things
To print out things use keyword `out` and ot get input use `input`.
The keyword `end` will print the next line, only works with `out` and `input`
```
age = 16
out("Kostia", " ", age, end)
```
`#` helps to get number of things. For example number of items in array
```
num = [1,2,3,4,5,6,7]#

out(num,end) ~will print out 7~
```
works with Integers, Strings and Arrays
`?` helps to get type of things
```
type = "Hello World"?
out(type, end) ~will printf out STRING~
```
# Objects
Delcare object with `obj` following name of obejct and items that it can hold
```
obj Person{last, age}
Kostia = Person{"Ilnytskyi", 16}
```
to access the item use the `.` and name of the item
```
last_name = Kosita.last
```
to change it
```
Kostia.age = 17
```
# Array
To add item use `+` or `+=` and item
```
names = ["Kostia", "Jeff", "John"] + "Ronin"
```
To remove item use `-` or `-=` and index
```
names -= 1
out(names, end) ~Output: Kostia John Ronin~
```
To change array item
```
obj Person{name, age}

index = 0

list = ["Apple", 10, 11,Person{"Kos", 16}]

list[index] = "Orange"

list[list#-1].name = "Kostia"
```
# Block
Every block has it's on scope meaning, that every new variable, object and function are automatically in local scope. Block starts with  `{` and ends with `}`
## Function declaration
To declare function you use keyword `fn`.
```
fn add(first_number, second_number){
  return first_number + second_number
}

result = add(21, 69)
```
If function doesn't return anything it will automatically return `0`
## If statements
There are two keywords `if` and `elif`.
```
age = 16
if(age < 21){
  out("Adult")
}
elif(age < 13){
  out("Teenager")
}
elif(1){
  out("Child")
}
```
Keyword `elif` serves as a `else if` and `else`. To use `else if` you give it a conditon, to use `else` you just put `1` in a condition.
## Loops
Loops have two modes, `for` and `while`. The langauge will automatically detect which one is `for` and `while`.
To declare it use keyword `do`.
```
num = 0
do(1<5){    ~while loop~
  num += 1
}

do(5){      ~for loop~
  num+= 1 
}
```
In `for` you can put String, Integers and Arrays to loop through them.
To break a loop use keyword `stop` and to skip to next loop use keyword `skip`
## Public Scope 
To create public variable, function or object use keyword `pub`
```
pub name = "Kostia Ilnytskyi"

pub obj Person{name, age}

me = Person{name, 16}

```
# Import 
To import items use keyword `use` and name of the file
```
use "/file1.txt", "/file2.txt"
```
# Modules
Use `@` and name of the module and give arguments
```
positive = @abs(-10)
```
|Module Name|Number of Arguments|Type of Arguments|Return|
|---|---|---|---|
|system|1|String|Any|
|exit|0|None|None|
|execute|1|String|None|
|sleep|1|Integer|1|
|time|0|None|Integer|
|cos|1|Integer|Integer|
|sin|1|Integer|Integer|
|tan|1|Integer|Integer|
|acos|1|Integer|Integer|
|asin|1|Integer|Integer|
|atan|1|Integer|Integer|
|atan2|2|Integer,Integer|Integer|
|sqrt|1|Integer|Integer|
|hypot|2|Integer,Integer|Integer|
|floor|1|Integer|Integer|
|random|2|Integer,Integer|Integer|
|abs|1|Integer|Integer|
|min|2|Integer,Integer|Integer|
|max|2|Integer,Integer|Integer|

