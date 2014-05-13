//
//  GLUT_Build_Tests.m
//  GLUT Build Tests
//
//  Created by Ken Kopecky on 4/28/14.
//
//

#import <XCTest/XCTest.h>

@interface GLUT_Build_Tests : XCTestCase

@end

@implementation GLUT_Build_Tests

- (void)setUp
{
	printf("set up test!\n");
    [super setUp];
	printf("done setup\n");
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown
{
	printf("tear down!\n");
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
	printf("done teardown\n");
}

- (void)testExample
{
	printf("example test!\n");
	if(false)
	{
		XCTFail(@"No implementation for \"%s\"", __PRETTY_FUNCTION__);
	}
}


- (void)testExample2
{
	printf("example2 test!\n");
	if(false)
	{
		XCTFail(@"No implementation for \"%s\"", __PRETTY_FUNCTION__);
	}
}

-(void)testSaidKen
{
	if(false)
	{
		XCTFail(@"No implementation for \"%s\"", __PRETTY_FUNCTION__);
	}
}

@end
