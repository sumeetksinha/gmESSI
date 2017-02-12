import gmessi

Example_5 = gmessi.gmESSIPython()

Example_5.loadMshFile("Example_5.msh")

Inputfile = open('Example_5.gmessi', 'r')

for line in Inputfile:
	line = line.partition('//')[0]
	line = line.rstrip()
	if (line):
		Example_5.Convert(line)
