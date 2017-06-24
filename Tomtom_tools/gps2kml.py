import xml.etree.ElementTree as ET
import random
from matplotlib import pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

mapname = 'xmlnds'
xmlfile_path = "/home/genmaoshi/truenas/tomtom_data_sample/" + "xmlnds" + "/uca/"

# roaddna_file_path = "/home/genmaoshi/Desktop/"
# mapname = "test_ramp"

xmlfile = xmlfile_path + mapname + ".xml"
kmlfile = mapname + str(999) + ".kml"
# tree = ET.parse(xmlfile)
# root = tree.getroot()
# root = root[1]


tree = ET.parse(xmlfile)
#
# s = set()
# for elem in tree.iter(tag='material'):
#     s.add(elem.text)
#
# print s

lon = []
lat = []
alt = []

with open(kmlfile, 'w') as outfile:
    outfile.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"
                  "<Document>\n<name>test3</name>\n<description/>\n<Folder>\n<name>" + mapname + "</name>\n")
line_count = 0
for elem in tree.iter(tag='wktLineString3d'):
    contain = False
    l = elem.text.split('(')[1].split(')')[0]
    for coor in l.split(', '):
        values = coor.split(' ')
        if abs(float(values[0]) - (-117.1333)) < 10e-5 and abs(float(values[1]) - 32.77274) < 10e-5:
            print float(values[0])
            contain = True
            break
    if ~contain:
        continue
    line_count += 1
    with open(kmlfile, 'a') as outfile:
        outfile.write("<Placemark>\n<name>Line " + str(line_count)
                      + "</name>\n<description>" + str(line_count)
                      + "</description>\n<styleUrl>#line-0288D1-1</styleUrl>\n<LineString>\n<tessellate>"
                      + str(line_count) + "</tessellate>\n<coordinates>\n")

    # if line_count > 125:
    #     # Maximum size of google map kml file is 5MB
    #     break
    # l = elem.text.split('(')[1].split(')')[0]
    for coor in l.split(', '):
        values = coor.split(' ')
        # lon.append(float(values[0]))
        # lat.append(float(values[1]))
        # alt.append(float(values[2]))

        with open(kmlfile, 'a') as outfile:
            outfile.write(str(values[0])+','+str(values[1])+',0')
    with open(kmlfile, 'a') as outfile:
        outfile.write("</coordinates>\n</LineString>\n</Placemark>\n")
with open(kmlfile, 'a') as outfile:
    outfile.write("</Folder>\n</Document>\n</kml>\n")

"""
fig = plt.figure(1)
ax = fig.add_subplot(111, projection='3d')
ax.scatter(lon, lat, alt)
ax.set_xlabel('longitude')
ax.set_ylabel('latitude')
ax.set_zlabel('altitude')
"""
