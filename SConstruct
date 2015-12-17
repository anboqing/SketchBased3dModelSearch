
env = Environment(CPPPATH=['#include/','#include/feature/','#include/quantize/'],
CXXFLAGS="-std=c++0x")
env.Append(CXXFLAGS=["-DNDEBUG"])

#env.Append(CCFLAGS='-std=c++11')

env.ParseConfig("pkg-config --libs --cflags opencv")

# boost library
env.Append(LIBS=['boost_system','boost_filesystem'])
# google log : glog
env.Append(LIBS=['glog'])

lib_paths = ['/usr/local/lib','/usr/lib']

env.Append(LIBPATH=lib_paths)

# include subdirectory scripts and export environments to them 
SConscript(['debug/SConscript','src/SConscript'],exports='env')
