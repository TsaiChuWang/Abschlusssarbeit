## @file alpha_psd_extractor.py
#  @brief This script extracts specific values (ALPHA or PSD fractions) from a solution file.
#  It parses an XML document and writes the extracted data to a specified output file.

from xml.dom.minidom import parse
import sys

ERHALTEN_ALPHA 		= 1
ERHALTEN_PSD_BRUCH 	= 2

TEMPERATUR_WEG = "../Datei/Parser_Temperatur.txt"

## @brief Extracts the value of the variable "ALPHA" from the solution XML file and writes it to a file.
#  @param solution_path The path to the solution XML file.
def erhaltenALPHA(solution_path):
	domTree = parse(solution_path)
	rootNode = domTree.documentElement
	# print(rootNode.nodeName)

	variables =  rootNode.getElementsByTagName("variable")
	# print(variables)
	with open(TEMPERATUR_WEG, "w") as outfile: 
		for variable in variables:
			if(variable.hasAttribute("name")):
				if(variable.getAttribute("name")=="ALPHA"):
					outfile.write(variable.getAttribute("value"))

## @brief Extracts variables whose names start with "f_" from the solution XML file and writes their names and values to a file.
#  @param solution_path The path to the solution XML file.
def erhaltenPSDBruch(solution_path):
	domTree = parse(solution_path)
	rootNode = domTree.documentElement
	variables =  rootNode.getElementsByTagName("variable")
	# print(variables)
	with open(TEMPERATUR_WEG, "w") as outfile: 
		for variable in variables:
			if(variable.hasAttribute("name")):
				if(variable.getAttribute("name")[:2]=="f_"):
					outfile.write(variable.getAttribute("name")+":"+variable.getAttribute("value")+"\n")

## @brief Main function to execute the appropriate extraction function based on the input argument.
#  Takes a command-line argument to determine which function to run (ALPHA or PSD fraction extraction).
if __name__ == '__main__':
	solution_path = ""
	if(int(sys.argv[1])==ERHALTEN_ALPHA):
		solution_path = "../Lösung/Haupt_"+str(sys.argv[2])+".sol"
		erhaltenALPHA(solution_path)

	if(int(sys.argv[1])==ERHALTEN_PSD_BRUCH):
		solution_path = "../Lösung/Haupt_"+str(sys.argv[2])+".sol"
		erhaltenPSDBruch(solution_path)