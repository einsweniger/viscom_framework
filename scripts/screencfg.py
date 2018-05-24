from xml.etree import ElementTree as ET
import numpy as np

window_y = 1280
window_x = 720
nodes_y = 2
nodes_x = 2


def genroot():
    root = ET.Element("Cluster", masterAddress="localhost", debug="true")
    return root


def genNodes(count=4):
    port1 = 20401
    port2 = 20501
    master = ET.Element("Node", address="localhost", port=str(port1), dataTransferPort=str(port2))
    yield master
    for nodeid in range(1, count):
        yield ET.Element("Node", address="8.8.8.8", port=str(port1 + nodeid), dataTransferPort=str(port2 + nodeid))


def genWindows():
    xs = np.linspace(0,window_y,nodes_x+1)
    ys = np.linspace(0,window_x,nodes_y+1)
    sy = window_x // nodes_y
    for x in range(nodes_x):
        for y in range(nodes_y):
            window = ET.Element("Window", fullScreen="false", monitor="0")
            ET.SubElement(window, "Stereo", type="none")
            ET.SubElement(window, "Size", x=str(xs[1]), y=str(ys[1]))
            ET.SubElement(window, "Pos", x=str(xs[x]), y=str(ys[y]))
            yield window


def genViewPort():
    vp = ET.Element("Viewport", eye="right")
    ET.SubElement(vp, "Pos", x="0.0", y="0.0")
    ET.SubElement(vp, "Size", x="1.0", y="1.0")
    return vp


def genPlane(x, y):
    right = window_y / window_x
    left = -(right)
    up = 1.0
    down = -(up)

    xs = np.linspace(left, right, nodes_x + 1)
    ys = np.linspace(up, down, nodes_y + 1)
    lstr = str(xs[x])
    rstr = str(xs[x + 1])
    ustr = str(ys[y])
    dstr = str(ys[y + 1])

    lowerleft = {"x": lstr, "y": dstr, "z": str(0.0)}
    upperleft = {"x": lstr, "y": ustr, "z": str(0.0)}
    upperright = {"x": rstr, "y": ustr, "z": str(0.0)}
    return lowerleft, upperleft, upperright


def genViewPlanes():
    for xs in range(nodes_x):
        for ys in range(nodes_y):
            vp = ET.Element("Viewplane")
            ll, ul, ur = genPlane(xs, ys)
            ET.SubElement(vp, "Pos", ll)  # lower left
            ET.SubElement(vp, "Pos", ul)  # upper left
            ET.SubElement(vp, "Pos", ur)  # upper right
            yield vp


if __name__ == '__main__':
    root = genroot()

    windows = genWindows()
    planes = genViewPlanes()

    for node in genNodes(nodes_y * nodes_x):
        window = next(windows)
        port = genViewPort()
        plane = next(planes)
        port.append(plane)
        window.append(port)
        node.append(window)
        root.append(node)

    user = ET.Element("User", eyeSeparation="0.0")
    ET.SubElement(user, "Pos", x="0.0", y="0.0", z="4.0")
    root.append(user)
    tree = ET.ElementTree(root)
    tree.write('../cmake-build-debug/test.xml', xml_declaration=True, encoding="unicode")
