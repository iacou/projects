#! /usr/bin/env python

import yaml
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('-n', '--number', metavar='number of hosts', type=int, required=True,
                    help='number of hosts to deploy')

args = parser.parse_args()

doc = {}
doc["version"] = '2'
doc["services"] = ""
for i in range(3):
    doc['services']['server'] = {
            "image": "debian-ed",
            "ports": [
                "200" + str(i) + ":22"
            ]
        },

with open(r'Docker-compose.yaml', 'w') as compose:
    documents = yaml.dump(doc, compose)