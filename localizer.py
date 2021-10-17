#!/usr/bin/env python3

import json
import argparse


class Colors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

    def color_string(self, message):
        return f"{self.OKCYAN}{message}{self.ENDC}"


class Localizer:
    def __init__(self, reference_path, working_path):
        self.reference_dict = dict()
        self.working_dict = dict()
        self.out_dict = dict()
        self.reference_path = reference_path
        self.working_path = working_path
        self.init_dicts()

    def init_dicts(self):
        with open(self.reference_path) as reference_file:
            self.reference_dict = json.load(reference_file)
        with open(self.working_path) as working_file:
            self.working_dict = json.load(working_file)

    def get_new_dict(self):
        self.out_dict = self.update_values(self.reference_dict, self.working_dict)[0]

    def update_values(self, reference_dict, working_dict):
        colors = Colors()
        done = False
        for key, value in reference_dict.items():
            if done:
                return (working_dict, True)
            if isinstance(value, dict):
                if key not in working_dict:
                    working_dict[key] = dict()
                working_dict[key], done = self.update_values(
                    value, working_dict[key])
            elif key not in working_dict:
                working_val = input(
                    f"Translation for key: [{key}] was not found.\nType in the translation in the field below.\nType \"_exit\" to save & quit and \"_skip\" to go to skip to the next key.\n<<< {colors.color_string(repr(value)[1:-1])}\n>>> ")
                if working_val == "_exit":
                    return (working_dict, True)
                elif working_val == "_skip":
                    continue
                working_dict[key] = working_val
        return (working_dict, False)

    def update_file(self):
        self.get_new_dict()
        with open(self.working_path, 'w') as working_file:
            json.dump(self.out_dict, working_file, indent=2, ensure_ascii=False)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description="Compare and update JSON translation files")
    required = parser.add_argument_group('Required arguments')
    required.add_argument('-r', '--reference',
                          help='reference file', required=True)
    required.add_argument(
        '-w', '--working', help='working file, will be edited', required=True)
    args = parser.parse_args()

    loc = Localizer(args.reference, args.working)
    loc.update_file()
