import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
import os
import time
import xml.etree.cElementTree as ET
import xml.dom.minidom

def decode_origin(o):
    if o == 0:
        return "MANUAL"
    if o == 1:
        return "WELLKNOWN"
    if o == 2:
        return "DHCP"
    if o == 3:
        return "ROUTER ADVERTISEMENT"
    if o == 4:
        return "6TO4"

def print_route_table(xmlout):
    xmlroot = ET.Element("TargetRouteTable")
    routes = api.QueryNetworkRoutes(const.AF_INET)
    eh.ui.Echo("===================================[Active Network Routes]===================================", eh.ECHO_DEFAULT)
    eh.ui.Echo("{:>18} {:>18} {:>18} {:>18} {:>15} {:>10} {:>10}".format("Network Destination", "Netmask", "Gateway", "Interface", "Interface Id", "Metric", "Origin"), eh.ECHO_DEFAULT)
    for route in routes:
        gw = route.gateway.get(str)
        dest = route.destination_address.get(str)
        mask = route.netmask.get(str)
        iface = route.interface_address.get(str)
        iface_id = route.interface_id.get(int)
        metric = route.metric.get(int)
        origin = decode_origin(route.origin.get(int))

        route_xml = ET.SubElement(xmlroot, "NetworkRoute")
        ET.SubElement(route_xml, "Destination").text =  str(dest)
        ET.SubElement(route_xml, "Mask").text =  str(mask)
        ET.SubElement(route_xml, "Gateway").text =  str(gw)
        ET.SubElement(route_xml, "Interface").text =  str(iface)
        ET.SubElement(route_xml, "InterfaceId").text =  str(iface_id)
        ET.SubElement(route_xml, "Metric").text =  str(metric)
        ET.SubElement(route_xml, "Origin").text =  str(origin)

        eh.ui.Echo(" {:>18} {:>18} {:>18} {:>18} {:>15} {:>10} {:>10}".format(dest, mask, gw, iface, iface_id, metric, origin), eh.ECHO_DEFAULT)
    
    if xmlout:
        from datetime import datetime
        xmlroot.set('timestamp', datetime.now().isoformat())
        xmlroot.set('taskuid', eh.TASK_UID)
        tree = ET.ElementTree(xmlroot)
        ET.indent(tree, level=0)
        tree.write(xmlout, xml_declaration=True, encoding="utf-8")

def check_rt_args(args):
    if not args.gw or not args.dest or not args.mask or not args.iface:
        return False
    return True

def add_route(dest, mask, gw, iface, metric):
    result = api.AddNetworkRoute(const.AF_INET, dest, gw, mask, iface, metric)
    if not result:
        eh.ui.Echo("Failed to add new route", eh.ECHO_ERROR)

def delete_route(dest, mask, gw, iface):
    result = api.DeleteNetworkRoute(const.AF_INET, dest, gw, mask, iface)
    if not result:
        eh.ui.Echo("Failed to delete specified route", eh.ECHO_ERROR)

def main(args):
    if not args.query and not args.add and not args.delete:
        eh.ui.Echo("No arguments provided. Use .help", eh.ECHO_ERROR)
        return

    if args.query:
        print_route_table(args.xmlout)

    elif args.add:
        if not check_rt_args(args):
            eh.ui.Echo("Invalid route parameters. Use .help", eh.ECHO_ERROR)
            return
        add_route(args.dest, args.mask, args.gw, args.iface, args.metric)

    elif args.delete:
        if not check_rt_args(args):
            eh.ui.Echo("Invalid route parameters. Use .help", eh.ECHO_ERROR)
            return
        delete_route(args.dest, args.mask, args.gw, args.iface)
    