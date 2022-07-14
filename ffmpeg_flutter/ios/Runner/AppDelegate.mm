#import "AppDelegate.h"
#import "GeneratedPluginRegistrant.h"
#include "MP3Encoder.hpp"

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application
    didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [GeneratedPluginRegistrant registerWithRegistry:self];
  
    MP3Encoder *_encoder = new MP3Encoder();
    _encoder->encode();
    
    return [super application:application didFinishLaunchingWithOptions:launchOptions];
}

@end
