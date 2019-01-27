TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = cryptopp geometry geometrytest
geometrytest.depends = geometry cryptopp

