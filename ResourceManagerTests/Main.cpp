#define ALL_PUBLIC

#include <gtest/gtest.h>
#include <Resource.h>
#include <ResourceContainer.h>
#include <ZipArchive.h>
#include <istream>

class MockResource
{
public:
	char mockData[1024];
};

class MockResourceManager
{
public:
	MockResourceManager()
	{
		counter = 0;
	}

	Resource<MockResource> Create()
	{
		MockResource* resource = new MockResource;
		resource->mockData[0] = 'K';
		InternalResource<MockResource>* internal = container.AddResource(counter, resource);
		counter++;

		return Resource<MockResource>(resource, internal, &container);
	}

	ResourceContainer<MockResource> container;
	int counter;
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

	MockResourceManager rm;
};

TEST_F(ResourceManagerTest, ResourceConstructionAssignment)
{
	{
		Resource<MockResource> a;
		ASSERT_EQ(a, nullptr);

		Resource<MockResource> b = rm.Create();
		ASSERT_NE(rm.container.resources[0].resource, nullptr);

		ASSERT_NE(b, a);
		ASSERT_NE(b, nullptr);
		ASSERT_EQ(b.internal->refCount, 1);

		Resource<MockResource> c(b);
		ASSERT_EQ(c, b);
		ASSERT_EQ(c.internal->refCount, 2);
		ASSERT_EQ(b.internal->refCount, 2);

		a = b;
		ASSERT_NE(a, nullptr);
		ASSERT_EQ(a, b);
		ASSERT_EQ(a.internal->refCount, 3);

		ASSERT_EQ(a->mockData[0], 'K');
		ASSERT_EQ((*a).mockData[0], 'K');

		{
			Resource<MockResource> e(a);
			ASSERT_EQ(a.internal->refCount, 4);
		}

		ASSERT_EQ(a.internal->refCount, 3);
	}
	
	ASSERT_EQ(rm.container.resources[0].refCount, 0);
}

/*
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
	ASSERT_TRUE(rm.AddArchive<ZipArchive>("../Assets/Test2.zip"));

	ASSERT_EQ(rm.archives.size(), 2);
	ASSERT_EQ(rm.files.size(), 7);

	std::vector<std::pair<std::string, std::string>> contents;
	contents.push_back(std::pair<std::string, std::string>("Test1.txt", "Test1 content"));
	contents.push_back(std::pair<std::string, std::string>("Test2.txt", "Test2 content"));
	contents.push_back(std::pair<std::string, std::string>("LONGFUCKINGFILENAMEHERECOURTESYOFDANIEL.txt", "FUCK YEAH!"));
	contents.push_back(std::pair<std::string, std::string>("ThisIsADirectory/ThisIsInADirectory.txt", "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."));
	contents.push_back(std::pair<std::string, std::string>("Kevin.txt", "Kevin kom på den här iden."));
	contents.push_back(std::pair<std::string, std::string>("Thomas.txt", "Om det här inte funkar så är det Kevins fel."));
	contents.push_back(std::pair<std::string, std::string>("Mapp/Daniel.txt", "Uuh, tack."));

	for (size_t i = 0; i < contents.size(); ++i)
	{
		// Assert that the file exists.
		ASSERT_NE(rm.files[contents[i].first], nullptr);

		// Assert that the contents are loaded properly.
		File* file = rm.files[contents[i].first];
		ASSERT_TRUE(file->Open());

		size_t filesize = file->GetFileSize();
		ASSERT_EQ(filesize, contents[i].second.size());

		char* filecontent = new char[filesize];
		ASSERT_EQ(file->Read(filecontent, filesize), filesize);
		
		CharBuffer buffer(filecontent, filecontent + filesize);
		std::istream stream(&buffer);

		std::string line;
		std::getline(stream, line);

		ASSERT_EQ(line, contents[i].second);

		file->Close();
	}
}
*/