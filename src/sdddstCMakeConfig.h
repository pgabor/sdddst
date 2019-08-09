/* Include the options set in CMakeLists.txt 
 *  and other project related settings
 */
#define SDDDST_VERSION_MAJOR "@sdddst_VERSION_MAJOR@"
#define SDDDST_VERSION_MINOR "@sdddst_VERSION_MINOR@"

#define SDDDST_NAME "@PROJECT_NAME@"

#define PYTHON_LIB_NAME @PYTHON_LIB_NAME@

#if 1 == @sdddst_VERBOSE@
    #define SDDDST_VERBOSE
#endif

