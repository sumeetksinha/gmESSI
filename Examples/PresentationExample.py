import gmessi

PresentationExample = gmessi.gmESSIPython()

PresentationExample.loadMshFile("PresentationExample.msh")

Inputfile = open('PresentationExample.gmessi', 'r')

for line in Inputfile:
	line = line.partition('//')[0]
	line = line.rstrip()
	if (line):
		PresentationExample.Convert(line)
