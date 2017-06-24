# 1. Line type (0-4) is in 'description' element
# 0, 1 for separators; 2 for partitions; 3 for starting point; 4 for ending point.
# If description is not given, then it takes default (0).

import xml.etree.ElementTree as ET

in_directory = "../data/in/"
kml_directory = "../data/kml/"
file_name = "map_all"
input_file = kml_directory + file_name + ".kml"
output_file = in_directory + file_name + ".in"


def convert_kml():
    outfile = open(output_file, 'w')
    tree = ET.parse(input_file)
    root = tree.getroot()
    xmlns = '{' + root.tag.split('{')[1].split('}')[0] + '}'
    for elem_iter in root.iter(xmlns + 'Placemark'):
        # print elem_iter.find(xmlns + 'name').text
        description = elem_iter.find(xmlns + 'description')
        if description == None:
            description = '0'                   # A valid default.
        else:
            description = description.text
        line_string = elem_iter.find(xmlns + 'LineString')
        if line_string is not None:
            coordinates = line_string.find(xmlns + 'coordinates').text
            # print coordinates
            num_of_lines = coordinates.count('\n') - 1
            coordinates_list = coordinates.split('\n')
            outfile.write(str(num_of_lines) + ' ' + description + '\n')
            for coordinate in coordinates_list[1:-1]:              # Get rid of redundant empty lines.
                outfile.write(coordinate.strip(' ') + '\n')
        point = elem_iter.find(xmlns + 'Point')
        if point is not None:
            coordinate = point.find(xmlns + 'coordinates').text.split('\n')[1].strip(' ')
            outfile.write('1 ' + description + '\n')
            outfile.write(coordinate + '\n')
    outfile.close()


if __name__ == "__main__":
    convert_kml()
