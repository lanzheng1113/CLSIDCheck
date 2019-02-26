# -*_coding:gb2312-*-
import os

s = "Windows Registry Editor Version 5.00\r\n\r\n"
file_clsid_missed = open("CLSIDMissed.txt")
for i in file_clsid_missed:
    i = i.strip()
    i = i.strip("\r")
    i = i.strip("\n")
    s += "[-HKEY_LOCAL_MACHINE\\ANew\\Classes\\CLSID\\"
    s += i
    s += "]\r\n"
file_clsid_missed.close()

file_clsid_missed_reg = open("HKLM_Software_Classes_CLSID.reg", "w")
file_clsid_missed_reg.write(s)
file_clsid_missed_reg.close()

s = "Windows Registry Editor Version 5.00\r\n\r\n"
file_interface_missed = open("InterfaceMissed.txt")
for i in file_interface_missed:
    i = i.strip()
    i = i.strip("\r")
    i = i.strip("\n")
    s += "[-HKEY_LOCAL_MACHINE\\ANew\\Classes\\Interface\\"
    s += i
    s += "]\r\n"
file_interface_missed.close()

file_interface_missed_reg = open("HKLM_Software_Classes_Interface.reg", "w")
file_interface_missed_reg.write(s)
file_interface_missed_reg.close()
