import timeit, random, datetime, base64, lzma, time

a = int(input())
print(a)
if a == 0:
    print(random.randint(1,22))
    print(timeit.timeit('"-".join(str(n) for n in range(100))', number=10000))
    print(datetime.date.today())
    print(base64.b85encode(lzma.compress(b'aqwrevqwercrewtqwercqervedcaewfeqrcewrawercqewrvqewr')))
elif a == 1:
    raise IndexError()

