#!/usr/bin/env python

import argparse
import json


def main():
  commands = argparse.ArgumentParser(description = 'Prettifies JSON files.')
  commands.add_argument('json_file', help = 'the json file to prettify')
  arguments = commands.parse_args()
  with open(arguments.json_file) as json_file:
    json_object = json.load(json_file)
  with open(arguments.json_file, 'w') as json_file:
    json_file.write(json.dumps(json_object, indent = 2, sort_keys = True))
    json_file.write('\n')


if '__main__' == __name__:
  main()
