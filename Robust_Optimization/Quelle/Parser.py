from xml.dom.minidom import parse
import sys

ERHALTEN_ALPHA = 1

def erhaltenALPHA(solution_path):
	domTree = parse(solution_path)
	rootNode = domTree.documentElement
	# print(rootNode.nodeName)

	variables =  rootNode.getElementsByTagName("variable")
	# print(variables)
	with open("../Datei/Parser_Temperatur.txt", "w") as outfile: 
		for variable in variables:
			if(variable.hasAttribute("name")):
				if(variable.getAttribute("name")=="ALPHA"):
					outfile.write(variable.getAttribute("value"))


if __name__ == '__main__':
	solution_path = ""
	if(int(sys.argv[1])==ERHALTEN_ALPHA):
		solution_path = "../Lösung/Haupt_"+str(sys.argv[2])+".sol"
		erhaltenALPHA(solution_path)
