// this file is merely for doxygen documentation

/** @mainpage DooSelection selection tools framework
 *
 * @section mp_introduction Introduction
 *
 * DooCore is a ROOT-/RooFit-/Boost-based framework providing central base 
 * functionality which is needed in nearly all ROOT/RooFit applications in high
 * energy physics analyses.
 *
 * @section mp_simplesetup Simple setup via global DooSoftware installation
 * 
 * On all machines with a DooSoftware installation setup of DooCore is very 
 * simple. In this case you can skip the next building step. Just call 
 * dooSoftwareSetup after setting up LHCb software and DaVinci/ROOT and the
 * DooSoftware magic will do the rest.
 * 
 * @section mp_installation Manual Installation
 *
 * The intended usage of DooCore is as a framework and therefore @a not to 
 * build custom user code inside of DooCore. However, DooCore can be easily 
 * installed and interfaced from other external projects. FindDooCore CMake 
 * modules are supplied which allow easy integration of DooCore in any other 
 * CMake project.
 *
 * @subsection mp_building Building DooCore
 *
 * First set the environment variable @c $DOOCORESYS to the install location
 * as set above. If not installing into a standard location like @c /usr/local
 * or similar, you also have to set @c $LD_LIBRARY_PATH (on Linux) or 
 * @c $DYLD_LIBRARY_PATH (on Mac OS) to include @c $DOOCORESYS/lib. Best 
 * practice probably is to set these in your <tt>~/.bashrc</tt> or globally
 * (see remark about DooSoftware below to skip these steps).
 *
 * @code
 * export DOOCORESYS=/path/to/install/to
 * export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DOOCORESYS/lib
 * export PATH=$PATH:$DOOCORESYS/bin
 * @endcode
 *
 * If setting in your <tt>~/.bashrc</tt>, remember to log out and in once or 
 * copy the corresponding lines also into your terminal (in case of DooSoftware
 * installation you can just use ~/LocalDooSoftware.sh, see 
 * https://wiki.e5.physik.tu-dortmund.de/LHCb/Software/DooSoftwareUsage).
 * 
 * To build DooCore, download it either as a release version or checkout from 
 * SVN. Create a @c build directory where you invoke CMake and install DooCore.
 * 
 * @code
 * cmake -DCMAKE_INSTALL_PREFIX=$DOOCORESYS /path/of/doocore/source
 * make
 * make install
 * @endcode
 *
 * 
 * @subsection mp_externalusage Using DooCore from an external project
 *
 * In the external project, adapt your <tt>CMakeLists.txt</tt> file to find 
 * DooCore and copy the provided <tt>FindDooCore.cmake</tt> (can be found in 
 * <tt>external/</tt> directory of DooCore) into your CMake modules path (which
 * can be configured in CMakeLists.txt). The following example shows this 
 * procedure:
 *
 * @code
 * # Append local directory cmake/Modules to $CMAKE_MODULE_PATH.
 * # This is where you want to put the provided FindDooCore.cmake.
 * set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
 * 
 * # Try to find DooCore (this requires $DOOCORESYS to be set)
 * find_package(DooCore)
 * 
 * # Tell CMake to also use DooCore includes and libraries in its search path for
 * # includes and libraries.
 * include_directories(${DooCore_INCLUDE_DIR})
 * link_directories(${DooCore_LIBRARY_DIR})
 *
 * # Only if DooCore is found build this executable and link it.
 * # this will protect the complete project from not compiling for anyone not 
 * # having DooCore installed. 
 * if (DooCore_FOUND)
 *   add_executable(MyBinary MainBinaryMain.cpp)
 *   target_link_libraries(MyBinary ${DooCore_LIBRARIES} ${MY_OTHER_LIBRARIES})
 * endif (DooCore_FOUND)
 * @endcode
 *
 * @section mp_usage How to use DooCore objects
 *
 * Using DooCore from external projects is simple. DooCore is setup as a 
 * namespace with various sub-namespaces.
 * 
 * Take this example:
 *
 * @code
 * #include "doocore/lutils/lutils.h"
 * #include "doocore/io/MsgStream.h"
 * 
 * int main(int argc, char* argv[]) {
 *   // the following lines might be handy and are only left out for 
 *   // demonstrational reasons
 *   // using namespace doocore::lutils;
 *   // using namespace doocore::io;
 *   
 *   doocore::io::sinfo << "This is an info message in colour!" << doocore::io::endmsg;
 *   
 *   doocore::lutils::printPlot(...all the printPlot parameters go here...)
 * }
 * @endcode
 *
 */

/**
 * @namespace doocore
 * @brief Global DooCore namespace
 */
