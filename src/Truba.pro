TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = cryptopp geometry secman ui
secman.depends = cryptopp
ui.depends = geometry secman cryptopp

