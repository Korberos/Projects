# Setup
# 1. Build system requires `deliver` (https://github.com/krausefx/deliver)
#    Install with:   sudo gem install deliver
# 2. Double click the provisioning profile in the build folder to add to xcode.
# 3. chmod +x ./build.sh
#
# Run  ./build.sh
#

# Variables
ANDROID_BUILD_DIR="/Users/${USER}/Dropbox/hdPoker/Builds/Development/Client/Android"
DESKTOP_BUILD_DIR="/Users/${USER}/Dropbox/hdPoker/Builds/Development/Client/Desktop"
IOS_BUILD_DIR="/Users/${USER}/Dropbox/hdPoker/Builds/Development/Client/iOS"

# run with `autobuilds` command to hard reset all local files and pull latest
# DANGER don't run on local machine as any repo changes will be nuked
if [ "$#" -eq 1 -a $1 = "autobuilds" ]; then

	BUILD_NUMBER_BEFORE=`git rev-list --count HEAD`

	# Grab latest
	echo "Auto Builds"
	git reset -q --hard HEAD
	git clean -q -f -d
	git pull -q origin
	git checkout -q HEAD

	BUILD_NUMBER=`git rev-list --count HEAD`

	if [ "$BUILD_NUMBER_BEFORE" == "$BUILD_NUMBER" ]; then
		echo "No new code. Aborting..."
		exit 0
	fi

fi

BUILD_NUMBER=`git rev-list --count HEAD`

mkdir -p "$ANDROID_BUILD_DIR/$BUILD_NUMBER"
mkdir -p "$DESKTOP_BUILD_DIR/$BUILD_NUMBER"
mkdir -p "$IOS_BUILD_DIR/$BUILD_NUMBER"

echo "Building HDPoker version" $BUILD_NUMBER

##### Update build string in app
echo "#define HDPOKER_VERSION \"${BUILD_NUMBER}\"" > ../Classes/Version.h

##### Mac build
xcodebuild -project `pwd`/proj.ios_mac/hdpoker.xcodeproj -configuration Release -target "hdpoker Mac"
zip -r hdpokerOSX.zip `pwd`/proj.ios_mac/build/Release/hdpoker\ Mac.app
mv hdpokerOSX.zip "${DESKTOP_BUILD_DIR}/${BUILD_NUMBER}/HDPoker_${BUILD_NUMBER}.zip"

##### iOS build
cd ../proj.ios_mac

# set the build number
/usr/libexec/PlistBuddy -c "Set CFBundleVersion 1.${BUILD_NUMBER}" ./ios/Info.plist
/usr/libexec/PlistBuddy -c "Set CFBundleShortVersionString 1.${BUILD_NUMBER}" ./ios/Info.plist

#ipa build -s "hdpoker iOS" --configuration Release --embed "`pwd`/../Build/HDPoker.mobileprovision" --identity "iPhone Distribution: Gaming Arts LLC (K8K66SZS7G)" --clean --verbose

# Provisioning profile setup in the xcode project:
#  Code Signing Identity: iPhone Distribution: Gaming Arts LLC (K8K66SZS7G)
#  Provisioning Profile:  HDPoker Mobile AppStore Profile
ipa build -s "hdpoker iOS" --configuration Release --clean --verbose
deliver testflight -u it@z4.com -a 875589196
