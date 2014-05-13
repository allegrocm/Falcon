//
//  OtherTest.m
//  Millennium Falcon
//
//  Created by Ken Kopecky on 4/28/14.
//
//

#import <XCTest/XCTest.h>
#include "Util.h"
@interface OtherTest : XCTestCase

@end

@implementation OtherTest

- (void)setUp
{
	printf("other setup\n");
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown
{
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
	printf("other teardown!\n");
}

- (void)testExample
{
	osg::MatrixTransform* mt = Util::loadModel("data/models/falcon3DS/milfalcon.3ds", 1.0);
//	osg::MatrixTransform* mt = new osg::MatrixTransform;
	
	if(!mt)
	   XCTFail(@"No implementation for \"%s\"", __PRETTY_FUNCTION__);
	else printf("falcon loaded\n");
}

@end
