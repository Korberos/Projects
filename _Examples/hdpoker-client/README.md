HDPoker

When checking in new files, be sure to include them across all projects (not sure how to do this yet)

Cocos2d modifications (track any changes made in the cocos library here):
- Inhibit All Warnings project setting in Xcode (Warnings are not see in compiler output for cocos2d library)
- ScrollView setSwallowTouches(true) to prevent touches from going through the view
- CCApplicaiton-ios.m:   [UIApplication sharedApplication].idleTimerDisabled = YES; to disable screen diming
- ParticleSystem:  Removed inflation routine prior to loading image data (image already supports checking and using correct inflation)

- Facebook integration for iOS: AppController.mm

    #import <FBSDKCoreKit/FBSDKCoreKit.h>

    - (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // ...

    //
    // **************************
    // !! IMPORTANT !!
    // **************************
    //
    // call [[FBSDKApplicationDelegate sharedInstance] application:didFinishLaunchingWithOptions
    // before app->run()

    BOOL ret = [[FBSDKApplicationDelegate sharedInstance] application:application
    didFinishLaunchingWithOptions:launchOptions];
    app->run();
    return ret;
    }

    - (BOOL)application:(UIApplication *)application
    openURL:(NSURL *)url
    sourceApplication:(NSString *)sourceApplication
    annotation:(id)annotation {
    return [[FBSDKApplicationDelegate sharedInstance] application:application
    openURL:url
    sourceApplication:sourceApplication
    annotation:annotation];
    }

    - (void)applicationDidBecomeActive:(UIApplication *)application {
        [FBSDKAppEvents activateApp];
    }




Building notes:

iOS/OSX:

  - Added header/library paths to boost


Windows:


For both debug/release configurations:
  - Added header/library paths to boost
  - Use of /FI option to always include stdint.h for use of int64_t type. (See https://msdn.microsoft.com/en-us/library/8c5ztk84.aspx)

Boost library:

- Windows binaries:
  - Built on Windows machine running VS2013 Express via:
    > bootstrap.bat
    > b2 link=static toolset=msvc-12.0 --build-type=complete --abbreviate-paths architecture=x86 address-model=32 install -j8
    
    Then all copied all libraries into single folder:
    > mkdir lib
    > for /r %x in (*.lib) do copy "%x" lib\

- iOS, iOS Simulator & Mac:   Precompiled version downloaded from: https://github.com/danoli3/ofxiOSBoost

- Android:
    1.   ./bootstrap.sh --with-libraries=system,thread

    2.  Open project-config.jam and add:
        using gcc : arm : arm-linux-androideabi-g++ ;

    3. Add NDK bin to path
       export PATH=$PATH:</path/to/>android-ndk-r10e/toolchains/arm-linux-androideabi-4.8/prebuilt/darwin-x86_64/bin/

    4. ./b2 link=static threading=multi target-os=android toolset=gcc-arm include=/Users/nous/Downloads/android-ndk-r10e/sources/cxx-stl/gnu-libstdc++/4.9/include include=/Users/nous/Downloads/android-ndk-r10e/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi/include include=/Users/nous/Downloads/android-ndk-r10e/platforms/android-19/arch-arm/usr/include

    5. TEMPORARY:  Also grab a copy of libgnustl_static.a and toss in lib path

Generating a keystore:

    keytool -genkey -v -keystore hdpoker.keystore -alias hdpoker -keyalg RSA -keysize 2048 -validity 10000
    Andrew Gilgallon
    HDPoker
    Z4
    Las Vegas
    NV
    US
    'yes'

cocos compile -p android -m release