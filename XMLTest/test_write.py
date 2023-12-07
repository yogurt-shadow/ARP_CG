import xml.etree.ElementTree as ET

# root = ET.Element('data')
# root.append()
# country = ET.SubElement(root,'country', {'name':'Liechtenstein'})
# rank = ET.SubElement(country,'rank')
# rank.text = '1'
# year = ET.SubElement(country,'year')
# year.text = '2008'
# tree=ET.ElementTree(root)
# tree.write("write_py.xml")

if __name__ == "__main__":
    root = ET.Element('DBUSER')
    tree = ET.ElementTree(root)
    root.text = "" 
    root.text += "test words"
    test2 = ET.Element("test2")
    root.append(test2)
    tree.write("write_py.xml")
