import os
""" 
Below are defined by user.
"""
file_path = "/mnt/truenas/scratch/data/map/hdmap/raw"
map_name = "40_miles_forward"
down_sample_rate = 6
"""
Above are defined by user.
"""


txt_file_name = map_name + ".txt"
kml_file_name = map_name + ".kml"
full_filename = os.path.join(file_path, str(txt_file_name))

# os.remove(kml_file_name)
with open(kml_file_name, 'w') as outfile:
    outfile.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"
                  "<Document>\n<name>" + map_name +
                  "</name>\n<description/>\n<Folder>\n<name>"+map_name+"</name>\n")
with open(kml_file_name, 'a') as outfile:
    outfile.write("<Style id=\"line-E65100-5456\">\n<LineStyle>\n<color>ff0051e6</color>\n<width>5.456</width>\n</LineStyle>\n</Style>")
point_count = 0
line_count = 1

with open(full_filename, 'r') as infile:
    with open(kml_file_name, 'a') as outfile:
        for line in infile:
            fields = line.split(',')
            if len(fields) == 3:  # If with precision field, CHANGE THIS VALUE TO 4.
                point_count += 1
                if point_count % down_sample_rate == 0:
                    lat = fields[0]
                    lon = fields[1]
                    alt = fields[2]
                    outfile.write(str(lon) + ',' + str(lat) + ',' + str(alt))
                    # will automatically add "\n" at the end.
            else:
                fields = line.split(' ')
                if len(fields) == 3:
                    point_count = 0
                    if line_count == 1:
                        # If first line
                        outfile.write("<Placemark>\n<name>Line " + str(line_count) + "</name>\n"
                                      "<description>" + str(line_count) + "</description>\n"
                                      "<styleUrl>#line-E65100-5456</styleUrl>\n<LineString>\n"
                                      "<tessellate>" + str(line_count) + "</tessellate>\n<coordinates>\n")
                    else:
                        # If not first line
                        outfile.write("</coordinates>\n</LineString>\n</Placemark>\n"
                                      "<Placemark>\n<name>Line " + str(line_count) + "</name>\n"
                                      "<description>" + str(line_count) + "</description>\n"
                                      "<styleUrl>#line-E65100-5456</styleUrl>\n<LineString>\n"
                                      "<tessellate>" + str(line_count) + "</tessellate>\n<coordinates>\n")
                    line_count += 1
            if line_count > 2000:
                break
        outfile.write("</coordinates>\n</LineString>\n</Placemark>\n</Folder>\n</Document>\n</kml>\n")

