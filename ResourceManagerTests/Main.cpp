#define ALL_PUBLIC

#include <gtest/gtest.h>
#include <ResourceManager.h>
#include <ZipArchive.h>
#include <istream>

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

TEST(ResourceTest, ResourceConstructionAssignment)
{
	MockResource original;
	original.mockData[0] = 'K';
	ASSERT_EQ(original.refCount, 0);

	{
		Resource<MockResource> a;
		ASSERT_EQ(a, nullptr);
		ASSERT_EQ(original.refCount, 0);

		Resource<MockResource> b(&original);
		ASSERT_EQ(b, &original);
		ASSERT_NE(b, a);
		ASSERT_EQ(original.refCount, 1);

		Resource<MockResource> c(b);
		ASSERT_EQ(c, &original);
		ASSERT_EQ(c, b);
		ASSERT_EQ(original.refCount, 2);

		a = b;
		ASSERT_EQ(a, &original);
		ASSERT_EQ(a, b);
		ASSERT_EQ(original.refCount, 3);

		Resource<MockResource> d;
		d = &original;
		ASSERT_EQ(d, &original);
		ASSERT_EQ(d, b);
		ASSERT_EQ(original.refCount, 4);

		ASSERT_EQ(d->mockData[0], 'K');
		ASSERT_EQ((*d).mockData[0], 'K');

		{
			Resource<MockResource> e(&original);
			ASSERT_EQ(original.refCount, 5);
		}

		ASSERT_EQ(original.refCount, 4);
	}
	
	ASSERT_EQ(original.refCount, 0);
}

TEST(ResourceTest, ResourceCopying)
{
	MockResource A;
	MockResource B;

	Resource<MockResource> a(&A);
	Resource<MockResource> b(&B);

	ASSERT_EQ(A.refCount, 1);
	ASSERT_EQ(B.refCount, 1);

	a = b;

	ASSERT_EQ(A.refCount, 0);
	ASSERT_EQ(B.refCount, 2);
}

TEST_F(ResourceManagerTest, CreateResource)
{
	std::string resourceName = "HerpResource";
	Resource<MockResource> a = rm.Create<MockResource>(resourceName);
	ASSERT_NE(a, nullptr);
	ASSERT_EQ(a.resource->refCount, 1);
	ASSERT_EQ(rm.resources.size(), 1);
	ASSERT_EQ(rm.resources[rm.stringHasher(resourceName)], a.resource);
}

TEST_F(ResourceManagerTest, ArchiveTest)
{
	ASSERT_TRUE(rm.AddArchive<ZipArchive>("../Assets/Test.zip"));

	ASSERT_EQ(rm.archives.size(), 1);
	ASSERT_EQ(rm.files.size(), 4);

	ASSERT_NE(rm.files["Test1.txt"], nullptr);
	ASSERT_NE(rm.files["Test2.txt"], nullptr);
	ASSERT_NE(rm.files["LONGFUCKINGFILENAMEHERECOURTESYOFDANIEL.txt"], nullptr);
	ASSERT_NE(rm.files["ThisIsADirectory/ThisIsInADirectory.txt"], nullptr);
	
	File* file = rm.files["Test1.txt"];
	file->Open();

	CharBuffer buffer = file->GetStreamBuffer();
	std::istream stream(&buffer);

	std::string line;
	std::getline(stream, line);

	ASSERT_EQ(line, "Test1 content");
}