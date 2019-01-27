CONFIG += warn_on c++14

INCLUDEPATH += ../../src  \   
               c:/qt/boost_1_55_0

# -Werror \

QMAKE_CXXFLAGS += -Werror \
                  -Werror=return-type \
                  -Wno-unused-function \
                  -Wno-unused-local-typedefs \
                  -Wno-unused-parameter \
                  -Wno-missing-field-initializers \
                  -Wno-reorder

DEFINES += BOOST_NO_AUTO_PTR

CONFIG(debug, debug|release) {
} else {
    DEFINES += NDEBUG
}
