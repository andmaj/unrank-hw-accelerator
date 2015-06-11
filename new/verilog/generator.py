MAXBITS = 32

def factorial(n, stop=0):
	o = 1
	while n > stop:
		o *= n
		n -= 1
	return o

def choose(n, k):
	if n < k:
		return 0
	return factorial(n, stop=k) / factorial(n - k)

for col in range(1,MAXBITS+1):
	for row in range(1,MAXBITS+1):
		print('assign binom_table[%d][%d] = %d;' % (col, row, choose(row,col)))
	print('')

for col in range(1,MAXBITS+1):
	for row in range(1,MAXBITS+1):
		print('assign bits[%d][%d] = num > binom_table[%d][%d];' % (col, row, col, row))
	print('')

for row in range(2,MAXBITS+1):
	print('assign xorbits[%d] = colbits[%d]^colbits[%d];' % (row, row-1, row))