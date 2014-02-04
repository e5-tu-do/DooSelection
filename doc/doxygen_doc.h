// this file is merely for doxygen documentation

/** @mainpage DooSelection selection tools framework
 *
 * @section mp_introduction Introduction
 *
 * DooSelection is a ROOT-/TMVA-/RooFit-/Boost-based framework providing central
 * selection functionality which is useful in high energy physics analyses.
 *
 * Here is an overview of the current DooSelection functionality:
 *
 * @li @link dooselection::reducer dooselection::reducer: is a framework to process and modify tuples. This includes applying selections, adding new leaves and perform higher level modifications. @endlink
 * @li @link dooselection::triage dooselection::triage: is a framework to optimize cut based selections and calculate efficiencies and performance of selections (regular and multivariate) based on MC, sWeighted data, or using fits on data. @endlink
 * @li Find a list of pre-installed executables: @ref exes
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
 * Git. Create a @c build directory where you invoke CMake and install DooSelection.
 * 
 * @code
 * /path/of/dooselection/source/scripts/rebuild_cmake.sh
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

// Documentation of available executables
/**
 * @page exes Available executables (pre-installed with DooSelection)
 *
 * \tableofcontents
 *
 * @section exec_ArrayFlattenerGrimReaper ArrayFlattenerGrimReaper
 * The ArrayFlattenerGrimReaper is able to flatten a tuple containing array-type
 * entries. 
 * 
 * Parameters needed: name of input file, name of input tree, name of output file,
 * name of output tree (no backslashes!), config file name
 * 
 * See an example of a config file:
 * @verbinclude ArrayFlattenerGrimReaper.cfg
 * 
 * @section exec_CandidateSelectionGrimReaper CandidateSelectionGrimReaper
 * The CandidateSelectionGrimReaper removes multiple candidates depending on a 
 * given branch. The candidate with the smalles entry in the respective branch is
 * chosen.
 * 
 * Parameters needed: name of input file, name of input tree, name of output file,
 * name of output tree (no backslashes!), name of discriminating leaf.
 * 
 * @section exec_FitTupleGrimReaper FitTupleGrimReaper
 * The FitTupleGrimReaper removes all leaves not matching the following regular
 * expressions: "obs.*", "var.*", "cat.*", "par.*", "idx.*", and ".*_sw"
 * 
 * Parameters needed: name of input file, name of input tree, name of output file,
 * name of output tree (no backslashes!)
 *
 * @section exec_MultiCutGrimReaper MultiCutGrimReaper
 * The MultiCutGrimReaper performes cuts following multiple cut strings provided
 * using a config file.
 *
 * Parameters needed: name of input file, name of input tree, name of output file,
 * name of output tree (no backslashes!), config file name
 *
 * See an example of a config file:
 * @verbinclude MultiCutGrimReaper.cfg
 * 
 * @section exec_MultipleCandidateAnalyseGrimReaper MultipleCandidateAnalyseGrimReaper
 * The MultipleCandidateAnalyseGrimReaper analyses a given tuple with a view to
 * multiple candidates present in the tuple due to multiple PVs and B candidates.
 * 
 * Parameters needed: name of input file, name of input tree, name of output file, 
 * name of output tree (no backslashes!)
 *
 * @section exec_SingleCutGrimReaper SingleCutGrimReaper
 * The SingleCutGrimReaper performs a single cut.
 *
 * Parameters needed: name of input file, name of input tree, name of output file,
 * name of output tree, cut string in quotation marks
 * 
 * @section exec_TMVAGrimReaper TMVAGrimReaper
 * The TMVAGrimReaper applies a TMVA trained MVA on a provided tuple and adds a 
 * branch containing the per-event classifier value.
 * 
 * Parameters needed: name of input file, name of input tree, name of output file,
 * name of output tree (no backslashes!), config file name
 * 
 * See an example of a config file:
 * @verbinclude TMVAGrimReaper.cfg 
 *
 * @section exec_VariableCategorizerGrimReaper VariableCategorizerGrimReaper
 * The VariableCategorizerGrimReaper creates one or multiple new variables/categories
 * containing equally filled bins depending on a given distribution of an existing
 * variable. 
 * 
 * Parameters needed: name of input file, name of input tree, name of output file,
 * name of output tree, config file name
 * 
 * See an example of a config file:
 * @verbinclude VariableCategorizerGrimReaper.cfg
 * 
 * @section exec_VariableRenamerGrimReaper VariableRenamerGrimReaper
 * The VariableRenamerGrimReaper renames existing branches according to a provided
 * substitution
 * 
 * Parameters needed: name of input file, name of input tree, name of output file,
 * name of output tree, config file name
 * 
 * See an example of a config file:
 * @verbinclude VariableRenamerGrimReaper.cfg
 * 
 */

/**
 * @namespace dooselection
 * @brief Global DooSelection namespace
 */
