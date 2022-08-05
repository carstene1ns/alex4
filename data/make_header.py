#!/bin/env/python3

import sys
import os
from os.path import basename, join, splitext

##############################################################################
# helpers
##############################################################################

# gets all files in a dict with enum key
def list_files(dir):
	dict_files = dict()
	for root, dirs, files in os.walk(dir):
		for name in files:
			ID = splitext(name)[0].upper()
			dict_files[ID] = join(basename(dir), name)
	return dict_files

def print_enum(dic, name, prefix, file):
	print("enum " + name + " {", file=file)
	line = "\t"
	for i, ID in enumerate(sorted(dic, key=dic.get)):
		add = prefix + ID
		if i == 0:
			add += " = 0, "
		else:
			add += ", "

		if len(line) + len(add) > 77:
			print(line[:-1], file=file)
			line = ""
			line += "\t" + add
		else:	
			line += add
	print(line + "\n\t// end marker\n\t" + prefix + "MAX\n};\n", file=file)

def print_mapping(dic, name, prefix, file):
	print("const char *" + name + "[" + prefix + "MAX] = {", file=file)
	size = len(dic)
	line = "\t"
	for i, ID in enumerate(sorted(dic, key=dic.get)):
		add = "\"" + dic[ID] + "\""
		if i != size-1:
			add += ", "

		if len(line) + len(add) > 77:
			print(line[:-1], file=file)
			line = ""
			line += "\t" + add
		else:	
			line += add
	print(line + "\n};\n", file=file)

header = """
/*****************************************
 * data_files.h - Part of alex4 project. *
 *                                       *
 * GENERATED header, do not edit!        *
 *****************************************/

#ifndef DATA_FILES_H
#define DATA_FILES_H

// PUBLIC enums
"""
split = """// PRIVATE mappings

#ifdef INSIDE_DATA
"""
footer = """#endif // INSIDE_DATA

#endif // DATA_FILES_H
"""

##############################################################################
# main
##############################################################################

# check params
args = len(sys.argv)
if args < 2 or args > 3:
	print("ERROR: provide data directory and optionally a header file!", file=sys.stderr)
	exit(1)

data_dir = sys.argv[1]

# to stdout or file
out = sys.stdout
if args == 3:
	out = open(sys.argv[2], 'w')

# get files
images = list_files(join(data_dir, "images"))
sounds = list_files(join(data_dir, "sfx_22"))
music = list_files(join(data_dir, "music"))
maps = list_files(join(data_dir, "maps"))
fonts = list_files(join(data_dir, "font"))

if not (images or sounds or music or maps or fonts):
	print("ERROR: Could not find data files!", file=sys.stderr)
	exit(1)

# generate header file
print(header, file=out)
print_enum(images, "_images", "I_", file=out)
print_enum(sounds, "_sounds", "S_", file=out)
print_enum(music, "_music", "MSC_", file=out)
print_enum(maps, "_maps", "MAP_", file=out)
print_enum(fonts, "_fonts", "F_", file=out)
print(split, file=out)
print_mapping(images, "_image_mapping", "I_", file=out)
print_mapping(sounds, "_sound_mapping", "S_", file=out)
print_mapping(music, "_music_mapping", "MSC_", file=out)
print_mapping(maps, "_map_mapping", "MAP_", file=out)
print_mapping(fonts, "_font_mapping", "F_", file=out)
print(footer, file=out)

# close file
if args == 3:
	out.close()
