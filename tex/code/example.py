# Function signatures consist only of one keyword (def)
def fizzbuzz(start, end):
    # Nested function definition
    def fizzbuff_from_int(i):
        entry = ''
        if i%3 == 0:
            entry += "Fizz"
        if i%5 == 0:
            entry += "Buzz"
        # empty string evaluates to false (useable in conditions)
        if not entry
            entry = str(i)
        return entry
    # List comprehensions are the pythonic way of composing lists
    return [int_to_fizzbuzz(i) for i in range(start, end+1)]
