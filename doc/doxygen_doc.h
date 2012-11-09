// this file is merely for doxygen documentation

/** @mainpage DooSelection selection tools framework
 *
 * @section mp_introduction Introduction
 *
 * DooSelection is a ROOT-/TMVA-/RooFit-/Boost-based framework providing central
 * selection functionality which is useful in high energy physics analyses.
 *
 * @section mp_simplesetup Simple setup via global DooSoftware installation
 * 
 * On all machines with a DooSoftware installation setup of DooSelection is very
 * simple. In this case you can skip the next building step. Just call 
 * dooSoftwareSetup after setting up LHCb software and DaVinci/ROOT and the
 * DooSoftware magic will do the rest.
 * 
 * @section mp_installation Manual Installation
 *
 * The intended usage of DooSelection is as a framework and therefore @a not to
 * build custom user code inside of DooSelection. However, DooSelection can be easily 
 * installed and interfaced from other external projects. FindDooSelection CMake 
 * modules are supplied which allow easy integration of DooSelection in any other 
 * CMake project.
 *
 * @subsection mp_building Building DooSelection
 *
 * First set the environment variable @c $DOOSELECTIONSYS to the install location
 * as set above. If not installing into a standard location like @c /usr/local
 * or similar, you also have to set @c $LD_LIBRARY_PATH (on Linux) or 
 * @c $DYLD_LIBRARY_PATH (on Mac OS) to include @c $DOOSELECTIONSYS/lib. Best 
 * practice probably is to set these in your <tt>~/.bashrc</tt> or globally
 * (see remark about DooSoftware below to skip these steps).
 *
 * @code
 * export DOOSELECTIONSYS=/path/to/install/to
 * export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DOOSELECTIONSYS/lib
 * export PATH=$PATH:$DOOSELECTIONSYS/bin
 * @endcode
 *
 * If setting in your <tt>~/.bashrc</tt>, remember to log out and in once or 
 * copy the corresponding lines also into your terminal (in case of DooSoftware
 * installation you can just use ~/LocalDooSoftware.sh, see 
 * https://wiki.e5.physik.tu-dortmund.de/LHCb/Software/DooSoftwareUsage).
 * 
 * To build DooSelection, download it either as a release version or checkout from 
 * SVN. Create a @c build directory where you invoke CMake and install DooSelection.
 * 
 * @code
 * cmake -DCMAKE_INSTALL_PREFIX=$DOOSELECTIONSYS /path/of/doocore/source
 * make
 * make install
 * @endcode
 *
 * 
 * @subsection mp_externalusage Using DooSelection from an external project
 *
 * In the external project, adapt your <tt>CMakeLists.txt</tt> file to find 
 * DooSelection and copy the provided <tt>FindDooSelection.cmake</tt> (can be found in 
 * <tt>external/</tt> directory of DooSelection) into your CMake modules path (which
 * can be configured in CMakeLists.txt). The following example shows this 
 * procedure:
 *
 * @code
 * # Append local directory cmake/Modules to $CMAKE_MODULE_PATH.
 * # This is where you want to put the provided FindDooSelection.cmake.
 * set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
 * 
 * # Try to find DooSelection (this requires $DOOSELECTIONSYS to be set)
 * find_package(DooSelection)
 * 
 * # Tell CMake to also use DooSelection includes and libraries in its search path for
 * # includes and libraries.
 * include_directories(${DooSelection_INCLUDE_DIR})
 * link_directories(${DooSelection_LIBRARY_DIR})
 *
 * # Only if DooSelection is found build this executable and link it.
 * # this will protect the complete project from not compiling for anyone not 
 * # having DooSelection installed. 
 * if (DooSelection_FOUND)
 *   add_executable(MyBinary MainBinaryMain.cpp)
 *   target_link_libraries(MyBinary ${DooSelection_LIBRARIES} ${MY_OTHER_LIBRARIES})
 * endif (DooSelection_FOUND)
 * @endcode
 *
 * @section mp_usage How to use DooSelection objects
 *
 * Using DooSelection from external projects is simple. DooSelection is setup as a 
 * namespace with various sub-namespaces.
 * 
 * Take this example:
 *
 * @code
 * #include "dooselection/reducer/Reducer.h"
 * 
 * int main(int argc, char* argv[]) {
 *   // the following lines might be handy and are only left out for 
 *   // demonstrational reasons
 *   // using namespace dooselection::reducer;
 *   
 *   dooselection::reducer::Reducer myreducer;
 *   myreducer.Run();
 * }
 * @endcode
 *
 */

/**
 * @namespace doocore
 * @brief Global DooSelection namespace
 */
