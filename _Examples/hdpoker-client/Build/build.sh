HDPOKER_OLD_VERSION=`git rev-list --count HEAD`

# Grab latest source
git reset -q --hard HEAD
git clean -q -f -d
git pull -q origin
git checkout -q HEAD

HDPOKER_VERSION=`git rev-list --count HEAD`

# Perform build # comparison

echo "Building HDPoker version" $HDPOKER_VERSION

# Build & Upload to TestFlight iOS
cd proj.ios_mac
ipa build -s "hdpoker iOS" -m ../Build/HDPoker.mobileprovision -i "iPhone Distribution: Gaming Arts LLC (K8K66SZS7G)"
ipa distribute:itunesconnect -a it@z4.com -p x9HDP9.9 -i 875589196 --upload --verbose
