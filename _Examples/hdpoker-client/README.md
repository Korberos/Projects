HDPoker

When checking in new files, be sure to include them across all projects (not sure how to do this yet)


Callbacks & boost::signals2::trackable


When using CCLOG, it has a max log limit - consider this when logging network traffic.

Cocos2d modifications (track any changes made in the cocos library here):
- ScrollView setSwallowTouches(true) to prevent touches from going through the view


Building notes:

iOS/OSX:

  - Added header/library paths to boost

  - The boost static library for iOS, iOS Simulator & Mac was downloaded from: https://github.com/danoli3/ofxiOSBoost


Windows:


For both debug/release configurations:
  - Added header/library paths to boost
  - Use of /FI option to always include stdint.h for use of int64_t type. (See https://msdn.microsoft.com/en-us/library/8c5ztk84.aspx)

Create boost library notes:
  - Boost was built on Windows machine running VS2013 Express via:
    > bootstrap.bat
    > b2 link=static toolset=msvc-12.0 --build-type=complete --abbreviate-paths architecture=x86 address-model=32 install -j8
    
    Then all copied all libraries into single folder:
    > mkdir lib
    > for /r %x in (*.lib) do copy "%x" lib\

