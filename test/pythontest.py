#/usr/bin/python
#run with python, not python3
from datetime import datetime
current_milli_time = lambda: datetime.now().microsecond
start = current_milli_time()
for x in range(0,100000):
	pass
time = float((current_milli_time() - start))
print("Done in %d milliseconds." % time)
