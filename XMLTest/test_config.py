import os
import sys

def readConfigurationList() -> (str, str):
    configFile = "./Config.txt"
    input_dir, output_dir = "", ""
    if not os.path.exists(configFile):
        return
    with open(configFile, 'r') as f:
        line = f.readlines()
    for i in range(len(line)):
        _line = line[i].strip()
        ele = _line.split()
        if len(ele) > 0 and ele[0] == "END":
            break
        elif len(ele) > 0 and ele[0] == "INPUT_DIRECTORY":
            input_dir += ele[1] + "\\"
            continue
        elif len(ele) > 0 and ele[0] == "OUTPUT_DIRECTORY":
            output_dir += ele[1] + "\\"
            continue
        if i + 1 == 2:
            break
    return input_dir, output_dir

if __name__ == "__main__":
    input, output = readConfigurationList()
    print("input: ", input)
    print("output: ", output)
