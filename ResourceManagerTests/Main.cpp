#include <gtest/gtest.h>
#include <ResourceManager.h>

class MockResource : public ResourceBase
{
public:
	char mockData[1024];
};

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	int ret = RUN_ALL_TESTS();

	std::cin.get();

	return ret;
}

class ResourceManagerTest : public ::testing::Test
{
protected:
	void SetUp()
	{

	}

	void TearDown()
	{

	}

	ResourceManager rm;
};

TEST_F(ResourceManagerTest, CreateResource)
{
	Resource<MockResource> r = rm.Create<MockResource>("HerpResource");
}